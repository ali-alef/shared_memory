FROM docker.arvancloud.ir/python:3.9-slim

RUN apt-get update && apt-get install -y build-essential
RUN apt-get install -y cmake
RUN apt-get install -y cmake libzmq3-dev

WORKDIR /app

COPY . /app

RUN gcc -shared -fPIC -o ./shared/shared_memory.so ./shared/shared_memory.c

RUN pip install -r ./python_service/requirements.txt
RUN pip install -r ./python_client/requirements.txt

RUN cd malek_taj && \
    mkdir build && \
    cd build && \
    cmake .. && \

RUN chmod +x ./runner.sh

ENV ENV=docker

CMD ["./runner.sh"]
