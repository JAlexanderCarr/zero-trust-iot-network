#!/bin/sh

#TODO: Problems with relative paths here, could be an issue for cert files

# The virtual environment is required to run the server and web interface
source venv/bin/activate

(trap 'kill 0' SIGINT; python3 server.py & flask run & mosquitto -c security/broker.conf)
