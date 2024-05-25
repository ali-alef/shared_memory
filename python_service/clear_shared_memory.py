from utils import write_to_shared_memory, create_shared_memory

shm = create_shared_memory()

for i in range(10):
    write_to_shared_memory(shm, "", i)
