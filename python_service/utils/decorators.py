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
            function_name,
            return_type,
            shard_memory,
            origin_service,
            destination_service,
    ):
        self.function_name = function_name
        self.return_type = return_type
        self.shard_memory = shard_memory
        self.origin_service = origin_service
        self.destination_service = destination_service

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        id = str(uuid.uuid4())

        write_to_shared_memory(
            self.shard_memory,
            json.dumps({
                "op": "call",
                "uuid": id,
                "args": args,
                "destination_service": self.destination_service,
                "origin_service": self.origin_service,
                "function_name": self.function_name,
            }),
        )

        # while True:
        #     data = read_from_shared_memory(self.shard_memory)
        #     data = json.loads(data)
        #     if data["uuid"] == id and data["op"] == "response":
        #         return data["result"]
        #
        #     time.sleep(0.1)


class Bridge:
    def __init__(self, shared_memory, origin_service):
        self.origin_service = origin_service
        self.shared_memory = shared_memory

    def js(self, destination_service: str = None, *args, **kwargs):
        def decorator(func):
            func = Function(
                function_name=func.__name__,
                return_type=func.__annotations__["return"],
                shard_memory=self.shared_memory,
                origin_service=self.origin_service,
                destination_service=destination_service,
            )
            return func

        return decorator
