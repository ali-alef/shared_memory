import zmq

import settings


def init_subscriber_socket():
    context = zmq.Context()
    subscriber = context.socket(zmq.SUB)
    subscriber.connect("tcp://localhost:%s" % settings.SUBSCRIBER_PORT)
    subscriber.setsockopt_string(zmq.SUBSCRIBE, "")
    print("Subscriber connected to port:", settings.SUBSCRIBER_PORT)
    return subscriber



def init_push_socket():
    context = zmq.Context()

    # Create a PUSH socket
    push_socket = context.socket(zmq.PUSH)
    push_socket.connect("tcp://localhost:5556")

    return push_socket


def init_request_socket():
    context = zmq.Context()
    req_socket = context.socket(zmq.REQ)
    req_socket.connect("tcp://127.0.0.1:5557")

    return req_socket
