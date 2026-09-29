import re

from dataclasses import dataclass

@dataclass
class Resume:
    tag: str
    skills: list[str]
    content: str

class ResumeParser:
    @staticmethod
    def parse(raw_resumes: list[str]) -> list[Resume]:
        resumes: list[Resume] = []

        for i in range(len(raw_resumes)):
            resume = raw_resumes[i].split("[SEP]")
            if (len(resume) != 3 or len(resume[0]) <= 0
                                or len(resume[2]) <= 0):
                continue
            resumes.append(Resume(resume[0], resume[1].split(","), resume[2]))

        return resumes