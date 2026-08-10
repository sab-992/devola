import os
from pathlib import Path


class NamedPipe:
    def __init__(self, name: str, mode: int = 0o666):
        self.name = f"/tmp/{name}"
        if not os.path.exists(self.name):
            os.mkfifo(self.name, mode)
        elif not self.is_fifo(self.name):
            raise ValueError(f"{self.name!r} exists and is not a FIFO")

    @staticmethod
    def is_fifo(path: str) -> bool:
        return Path(path).is_fifo()

    def write(self, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        fd = os.open(self.name, os.O_WRONLY)
        try:
            os.write(fd, data)
        finally:
            os.close(fd)

    def read(self, size: int = 65536) -> str:
        fd = os.open(self.name, os.O_RDONLY)
        try:
            chunks = []
            while True:
                chunk = os.read(fd, size)
                if not chunk:
                    break
                chunks.append(chunk)
        finally:
            os.close(fd)
        return (b"".join(chunks)).decode()