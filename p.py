import socket

receiver_address = ('127.0.0.1', 5555)

sender_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sender_socket.connect(receiver_address)

sender_socket.send(b'SIGNAL')

sender_socket.close()

