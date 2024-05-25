import uuid


class SharedMemoryLock:
    def __init__(self, req_socket):
        self.req_socket = req_socket
        self.lock_id = None

    def get_lock(self):
        self.lock_id = str(uuid.uuid4())

        while True:
            self.req_socket.send_string(f"lock:{self.lock_id}")
            reply = self.req_socket.recv_string()
            print(f'response lock -> {reply}')
            if reply.startswith("200"):
                return int(reply.split(':')[1])

    def unlock(self):
        if not self.lock_id:
            print('noting is locked')

        self.req_socket.send_string(f"unlock:{self.lock_id}")
        reply = self.req_socket.recv_string()
        self.lock_id = None
