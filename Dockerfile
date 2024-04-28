FROM python:3.9-slim

RUN apt-get update && apt-get install -y build-essential
#RUN apt-get install -y busybox && ln -s /bin/busybox /bin/vi

WORKDIR /app

COPY . /app

RUN gcc -shared -fPIC -o ./shared/shared.so ./shared/shared_memory.c

RUN pip install -r ./python_service/requirements.txt

ENV ENV=docker

CMD ["python", "python_service/main.py"]
