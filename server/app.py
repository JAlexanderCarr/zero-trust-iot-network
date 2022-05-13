from flask import Flask, render_template, Response, make_response
from datetime import datetime
import time
import json
import io
import os
from collections import OrderedDict
from database import *
import getpass

app = Flask(__name__)

username = input("Enter MQTT username: ")
password = getpass.getpass(prompt = 'Enter MQTT password: ')
db_pass = getpass.getpass(prompt = 'Enter database password: ')

@app.errorhandler(404)
def page_not_found(e):
  return render_template("404.html"), 404

@app.errorhandler(403)
def insufficient_permissions(e):
  return render_template("403.html"), 403

@app.route("/")
def index():
  return render_template("home.html")

@app.route("/plot/<ip>")
def plot(ip):
  t = time.time() * 1000
  temp = get_temp(ip, db_pass)
  press = get_press(ip, db_pass)
  data = [t, float(temp), float(press)]
  response = make_response(json.dumps(data))
  response.content_type = 'application/json'
  return response

@app.route("/stats/clients_conn")
def clients_conn():
  value = get_clients_conn(db_pass)
  return str(value - 1)

@app.route("/stats/subs_count")
def subs_count():
  value = get_subs_count(db_pass)
  return str(value)

@app.route("/stats/sent_total")
def sent_total():
  value = get_sent_total(db_pass)
  return str(value)

@app.route("/stats/received_total")
def received_total():
  value = get_received_total(db_pass)
  return str(value)

@app.route("/stats/clients_reg")
def clients_reg():
  file = open("../security/mqtt/security.json")
  security = json.load(file)
  file.close()
  return str(len(security["clients"]) - 2)

@app.route("/stats/broker")
def broker():
  getBroker = os.popen("mosquitto_ctrl -u " + username + " -P " + password + " -h 192.168.1.153 --cafile ../security/certs/ca.crt --key ../security/certs/client.key --cert ../security/certs/client.crt dynsec listClients 2>&1").read()
  if (getBroker.find("Error") != -1):
    return { "status": "Offline" }
  return { "status": "Online" }

if __name__ == "__main__":
    app.run()
  