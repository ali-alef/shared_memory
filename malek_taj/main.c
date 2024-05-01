#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void *context;
void *publisher;

void publisher_zmq_up() {
    context = zmq_ctx_new ();
    publisher = zmq_socket (context, ZMQ_PUB);
    int rc = zmq_bind (publisher, "tcp://*:5555");
    if (rc != 0) {
        perror("Error binding socket");
    }
}

void publisher_zmq_down() {
    zmq_close (publisher);
    zmq_ctx_destroy (context);
}

void publish_message(char *message) {
    zmq_send(publisher, message, strlen(message), 0);
}

int main () {
    publisher_zmq_up();

    while(1) {
        usleep(100000);
        printf("inja\n");
        publish_message("ail");
    }
    publisher_zmq_down();

    return 0;
}
