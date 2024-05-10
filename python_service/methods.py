from typing import Tuple


def hello(name: str, age: int) -> Tuple[str, int]:
    return "ali alef", 21


def invalid_function() -> str:
    return f"function is not a valid!"
