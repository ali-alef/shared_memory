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