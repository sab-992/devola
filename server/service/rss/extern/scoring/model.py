import re
import threading

from concurrent.futures import ThreadPoolExecutor, as_completed
from rapidfuzz import fuzz
from sentence_transformers import SentenceTransformer, util
from torch.types import Number
from torch.fft import Tensor
from typing import Optional


SKILL_MATCH_WEIGHT = 60
SEMANTIC_MATCH_WEIGHT = 40

class SentenceTransformerModel:
    m_instance: Optional["SentenceTransformerModel"] = None
    m_instance_lock = threading.Lock()
    m_model_lock = threading.Lock()

    def __new__(cls, *args, **kwargs):
        with cls.m_instance_lock:
            if cls.m_instance:
                return cls.m_instance

            instance = super().__new__(cls)
            instance.model = None
            cls.m_instance = instance

        return cls.m_instance

    def __init__(self, model_name: str):
        self.model: Optional[SentenceTransformer]

        with self.m_model_lock:
            if self.model is None:
                self.model = SentenceTransformer(model_name)

    def embed(self, chunked_text: list[str]) -> Tensor:
        if self.model is None:
            raise Exception("Could not retrieve model")

        return self.model.encode(chunked_text, convert_to_tensor=True)

    # resume : list[Resume]
    # website_listings: list[list[Listing]]
    def compute_scores(self, resumes: list, website_listings: list[list]):
        if self.model is None:
            raise Exception("Could not retrieve model")

        resume_content_embeddings: dict[str, Tensor] = self.__embed_resume_contents(resumes)

        results = []
        with ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(self.__compute_similarity, resumes, resume_content_embeddings, website_listings[i]) for i in range(len(website_listings))]
            for future in as_completed(futures):
                results.append(future.result())

        return results

    # listings: list[Listing]
    def __compute_similarity(self, resumes: list, resume_embeddings: dict[str, Tensor], listings: list) -> tuple[list[int], list[list[dict[str, float]]]]:
        ids: list[int] = []
        results: list[list[dict]] = []
        for i in range(len(listings)):
            results.append([])
            ids.append(listings[i].id)

            if not listings[i].content:
                continue

            listing_content = listings[i].content
            for j in range(len(resumes)):
                tag = resumes[j].tag

                skills_score: Number = self.__compute_skills_similarity(resumes[j].skills, listings[i].content)
                semantic_score: Number = self.__compute_content_similarity(resume_embeddings[tag], listing_content)

                final_score = self.__weighted_sum({ SKILL_MATCH_WEIGHT: skills_score, SEMANTIC_MATCH_WEIGHT: semantic_score })

                if final_score > 0:
                    results[i].append({ "tag": tag, "score": final_score })

        return ids, results

    def __weighted_sum(self, scores: dict[int, float]):
        total_weights = 0
        final_score = 0

        for weight, score in scores.items():
            total_weights += weight
            final_score += score * weight

        return final_score / total_weights

    def __compute_content_similarity(self, resume_embedding: Tensor, listing: str) -> Number:
        return util.cos_sim(self.embed(self.__chunk(listing)), resume_embedding).mean().item()

    def __compute_skill_score(self, skill: str, listing_ngrams: list[str]):
        best = 0.0

        for ng in listing_ngrams:
            best = max(best, fuzz.ratio(skill, ng) / 100)

        return best

    def __compute_skills_similarity(self, skills: list[str], listing: str) -> Number:
        mean: float = 0
        for skill in skills:
            mean += self.__compute_skill_score(skill, self.__get_ngrams(listing))
        return mean / len(skills)

    def __embed_resume_contents(self, resumes: list) ->  dict[str, Tensor]:
        content_embeddings: dict[str, Tensor] = {}
        for resume in resumes:
            if not resume.content:
                continue
            content_embeddings[resume.tag] = self.embed(self.__chunk(resume.content))
        return content_embeddings

    def __chunk(self, text: str) -> list[str]:
        MIN_WORDS = 4
        raw_lines = [l.strip("-•*  \t") .strip() for l in text.splitlines()]
        chunks = []
        for line in raw_lines:
            if not line:
                continue
            # Split long lines with multiple sentences.
            for sentence in re.split(r"(?<=[.!?])\s+", line):
                sentence = sentence.strip()
                if sentence and len(sentence.split()) >= MIN_WORDS:
                    chunks.append(sentence)
        return chunks

    def __get_ngrams(self, text: str, max_n=3):
        words = text.lower().split()
        ngrams = []
        for n in range(1, max_n + 1):
            for i in range(len(words) - n + 1):
                ngrams.append(" ".join(words[i:i+n]))
        return ngrams