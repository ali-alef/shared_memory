import uuid
from typing import Any
import json
from utils import (
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
            shared_lock,
    ):
        self.function_name = function_name
        self.return_type = return_type
        self.shard_memory = shard_memory
        self.service_name = service_name
        self.subscriber = subscriber
        self.shared_lock = shared_lock

    def __call__(self, *args: Any, **kwds: Any) -> Any:
        request_id = str(uuid.uuid4())

        self.shared_lock.get_lock()
        write_to_shared_memory(
            self.shard_memory,
            json.dumps({
                "operation": "request",
                "uuid": request_id,
                "args": args,
                "service_name": self.service_name,
                "function_name": self.function_name,
            }),
        )
        self.shared_lock.unlock()

        while True:
            message = self.subscriber.recv_string()
            operation, service_name, uid = message.split(":")

            if operation == "response" and request_id == uid and service_name == self.service_name:
                break

        self.shared_lock.get_lock()
        data = json.loads(read_from_shared_memory(self.shard_memory))
        write_to_shared_memory(self.shard_memory, "")
        self.shared_lock.unlock()

        return data["result"]



class Bridge:
    def __init__(self, shared_memory, subscriber, shared_lock):
        self.shared_memory = shared_memory
        self.subscriber = subscriber
        self.shared_lock = shared_lock

    def js(self, service_name: str = None, *args, **kwargs):
        def decorator(func):
            func = Function(
                function_name=func.__name__,
                return_type=func.__annotations__["return"],
                shard_memory=self.shared_memory,
                service_name=service_name,
                subscriber=self.subscriber,
                shared_lock=self.shared_lock,
            )
            return func

        return decorator
