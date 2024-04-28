from typing import Tuple

from utils import create_shared_memory, read_from_shared_memory, Bridge


shared_memory = create_shared_memory()

bridge = Bridge(shared_memory)


@bridge.js(path="./script.js")
async def hello(name: str = None, age: int = None) -> Tuple[str, int]: ...
