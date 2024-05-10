from typing import Tuple


def hello(name: str, age: int) -> Tuple[str, int]:
    return name + " alef", 10 + age


def invalid_function() -> str:
    return f"function is not a valid!"
