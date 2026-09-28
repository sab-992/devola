import threading

from concurrent.futures import ThreadPoolExecutor, as_completed
from sentence_transformers import SentenceTransformer, util
from torch.types import Number
from torch.fft import Tensor
from typing import Optional


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
    def compute_scores(self, resumes, website_listings):
        if self.model is None:
            raise Exception("Could not retrieve model")

        resume_embeddings: dict[str, Tensor] = {}
        for resume in resumes:
            if not resume.content_chunks:
                continue

            resume_embeddings[resume.tag] = self.embed(resume.content_chunks)

        results = []
        with ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(self.compute_similarity, resume_embeddings, website_listings[i]) for i in range(len(website_listings))]
            for future in as_completed(futures):
                results.append(future.result())

        return results

    # listings: list[Listing]
    def compute_similarity(self, resume_embeddings: dict[str, Tensor], listings) -> tuple[list[int], list[list[dict[str, float]]]]:
        ids: list[int] = []
        results: list[list[dict]] = []
        for i in range(len(listings)):
            results.append([])
            ids.append(listings[i].id)

            if not listings[i].content_chunks:
                continue

            listing_embedding = self.embed(listings[i].content_chunks)
            for j, (tag, resume_embedding) in enumerate(resume_embeddings.items()):
                score: Number = util.cos_sim(listing_embedding, resume_embedding).mean().item()
                if score > 0:
                    results[i].append({ "tag": tag, "score": score})
        return ids, results
