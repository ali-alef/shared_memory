import subprocess
import time
import uuid
from typing import Any
import json
from utils import (
    create_shared_memory,
    read_from_shared_memory,
    write_to_shared_memory,
)


class Function:
    def __init__(
            self,
            path: str,
            compiler_options: str,
            name: str,
            return_type: str,
            shard_memory,
    ):
        self.path = path
        self.compiler_options = compiler_options
        self.name = name
        self.return_type = return_type
        self.shard_memory = shard_memory

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        id = str(uuid.uuid4())
        write_to_shared_memory(
            self.shard_memory,
            json.dumps({
                "op": "call",
                "uuid": id,
                "args": args,
                "path": self.path,
                "name": self.name,
            }),
        )

        while True:
            data = read_from_shared_memory(self.shard_memory)
            data = json.loads(data)
            if data["uuid"] == id and data["op"] == "response":
                return data["result"]

            time.sleep(0.1)


class Bridge:
    def __init__(self, shared_memory):
        self.shared_memory = shared_memory

    def js(self, path: str = None, compiler_options: str = None, *args, **kwargs):
        def decorator(func):
            func = Function(
                path=path,
                compiler_options=compiler_options,
                name=func.__name__,
                return_type=func.__annotations__["return"],
                shard_memory=self.shared_memory,
            )
            return func

        return decorator
