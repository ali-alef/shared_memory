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
            service_name,
            subscriber,
    ):
        self.function_name = function_name
        self.return_type = return_type
        self.shard_memory = shard_memory
        self.service_name = service_name
        self.subscriber = subscriber

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        self.id = str(uuid.uuid4())

        write_to_shared_memory(
            self.shard_memory,
            json.dumps({
                "operation": "request",
                "uuid": self.id,
                "args": args,
                "service_name": self.service_name,
                "function_name": self.function_name,
            }),
        )

        print("wrote the request waiting for response ...")

        while True:
            message = self.subscriber.recv_string()
            operation, service_name, id = message.split(":")

            if operation == "response" and self.id == id and service_name == self.service_name:
                break

        data = json.loads(read_from_shared_memory(self.shard_memory))
        write_to_shared_memory(self.shard_memory, "")
        return data["response"]



class Bridge:
    def __init__(self, shared_memory, subscriber):
        self.shared_memory = shared_memory
        self.subscriber = subscriber

    def js(self, service_name: str = None, *args, **kwargs):
        def decorator(func):
            func = Function(
                function_name=func.__name__,
                return_type=func.__annotations__["return"],
                shard_memory=self.shared_memory,
                service_name=service_name,
                subscriber=self.subscriber,
            )
            return func

        return decorator
