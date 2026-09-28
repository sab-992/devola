import os
import sys

from pathlib import Path

EXTERN_FOLDER = "extern"
def find_folder(folder_name: str) -> str:
    current_path = Path(os.path.abspath(__file__))

    for parent in [current_path] + list(current_path.parents):
        current_path = parent / folder_name
        if current_path.exists():
            return str(current_path)

    raise Exception("Root folder not found!")

sys.path.append(find_folder(EXTERN_FOLDER))

import json
import argparse

from ipc.pipe import NamedPipe
from parser.listing import ListingParser, Listing
from parser.resume import ResumeParser, Resume
from scoring.model import SentenceTransformerModel

# ========================= MODELS =========================
BI_ENCODER_NAME = "all-MiniLM-L6-v2"

def readySequence():
    parser = argparse.ArgumentParser()
    parser.add_argument("--ready-fd", type=int, default=None)
    args = parser.parse_args()

    if args.ready_fd is not None:
        try:
            os.write(args.ready_fd, b"READY")
            os.close(args.ready_fd)
        except OSError as e:
            print(f"fd {args.ready_fd} error: {e}", file=sys.stderr, flush=True)

def main():
    readySequence()
    pipe = NamedPipe("rss_matcher")
    resumes = []
    try:
        raw_resumes = pipe.read().split("[END]\n")
        resumes: list[Resume] = ResumeParser.parse(raw_resumes)
        pipe.write("OK")
    except Exception as e:
        pipe.write(str(e))

    website_listings: list[list[Listing]] = []
    try:
        for i in range(int(pipe.read())):
            raw_listings = pipe.read().split("[END]\n")
            pipe.write("OK")
            website_listings.append(ListingParser.parse(raw_listings))
    except Exception as e:
        pipe.write(str(e))

    try:
        results = SentenceTransformerModel(BI_ENCODER_NAME).compute_scores(resumes, website_listings)
        pipe.write(json.dumps([{ "listing_ids": result[0], "scores": result[1] } for result in results]))
    except Exception as e:
        pipe.write(str(e))


if __name__ == "__main__":
    main()