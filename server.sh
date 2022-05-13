#!/bin/sh

# The virtual environment is required to run the server and web interface
source venv/bin/activate
python3 server/create_database.py

(trap 'kill 0' SIGINT; python3 server/server.py & FLASK_APP=server/app flask run & mosquitto -c security/broker.conf)
