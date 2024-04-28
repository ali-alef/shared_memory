import socket

listen_address = ('127.0.0.1', 5555)

receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

receiver_socket.bind(listen_address)

receiver_socket.listen(1)

connection, _ = receiver_socket.accept()

signal_data = connection.recv(1024)

connection.close()
receiver_socket.close()

if signal_data == b'SIGNAL':
    print("Signal received!")

