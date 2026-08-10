import re

from bs4 import BeautifulSoup
from dataclasses import dataclass


@dataclass
class Listing:
    id: int
    content_chunks: list[str]

class ListingParser:
    @staticmethod
    def parse(raw_listings: list[str]) -> list[Listing]:
        listings = []

        for j in range(len(raw_listings)):
            listing = raw_listings[j].split("[SEP]")
            if (len(listing) != 2 or len(listing[0]) <= 0
                                or len(listing[1]) <= 0):
                continue
            listings.append(Listing(int(listing[0]), ListingParser.chunk(ListingParser.clean_listing(listing[1]))))

        return listings

    @staticmethod
    def clean_listing(listing_content):
        MIN_WORDS_PER_LINE = 3
        soup = BeautifulSoup(listing_content, "html.parser")
        noise_tags = ["script", "style",  "noscript", "svg",   "iframe", "canvas",
                      "nav",    "footer", "header",   "aside", "form",   "button",
                      "input",  "select", "option",   "label"]

        for tag in soup(noise_tags):
            tag.decompose()
        for tag in soup.find_all(style=re.compile(r"display:\s*none", re.I)):
            tag.decompose()
        for tag in soup.find_all(lambda t: t.has_attr("hidden")):
            tag.decompose()

        raw_text = soup.get_text(separator="\n")
        lines = []
        for line in raw_text.splitlines():
            line = line.strip()
            if not line or len(line.split()) < MIN_WORDS_PER_LINE:
                continue
            lines.append(line)
        text = "\n".join(lines)
        text = re.sub(r"[ \t]+", " ", text)
        text = re.sub(r"\n{2,}", "\n", text)
        return text.strip()

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