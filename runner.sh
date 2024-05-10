#!/bin/bash

./malek_taj/build/malek_taj &

python ./python_service/main.py
python ./python_client/main.py
