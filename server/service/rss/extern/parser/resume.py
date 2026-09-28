import re

from dataclasses import dataclass

@dataclass
class Resume:
    tag: str
    content_chunks: list[str]

class ResumeParser:
    @staticmethod
    def parse(raw_resumes: list[str]) -> list[Resume]:
        resumes: list[Resume] = []

        for i in range(len(raw_resumes)):
            resume = raw_resumes[i].split("[SEP]")
            if (len(resume) != 2 or len(resume[0]) <= 0
                                or len(resume[1]) <= 0):
                continue
            resumes.append(Resume(resume[0], ResumeParser.chunk(resume[1])))

        return resumes

    @staticmethod
    def chunk(text: str) -> list[str]:
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
