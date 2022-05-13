import socket
import sys
import json
from datetime import datetime as dt
import _thread
from threading import Thread
from concurrent.futures import ThreadPoolExecutor
import asyncio
import aiocoap.resource as resource
import aiocoap
import random
import os
import ssl
import time
from paho.mqtt import client as mqtt_client
import hashlib
from sqlcipher3 import dbapi2 as sqlcipher
from database import *
import getpass
from getmac import get_mac_address as gma

username = input("Enter MQTT username: ")
password = getpass.getpass(prompt = 'Enter MQTT password: ')
db_pass = getpass.getpass(prompt = 'Enter database password: ')
mac = gma()
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(('10.255.255.255', 1))
broker = s.getsockname()[0]
s.close()
client_username = "device-" + mac
port = 8883
print(client_username)
topicpress = "data/pressure"
topictemp = "data/temperature"
client_id = "device-" + mac
clients_conn = ""
sent_total = ""
received_total = ""
subs_count = ""
clients_reg = ""

CA_FILE	= "../security/certs/ca.crt"
CERT_FILE = "../security/certs/client.crt" 
KEY_FILE =  "../security/certs/client.key" 
PEM_FILE	= "../security/certs/client.pem"

def verify_identity(mac):
  getClient = os.popen("mosquitto_ctrl -u " + username + " -P " + password + " -h " + broker + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec getClient device-" + mac + " 2>&1").read()
  if (getClient.find("Error: Client not found") != -1):
    # device has not been registered 
    return False
  elif (getClient.find("Error") != -1):
    return False
  else:
    return True

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code " + str(rc))
    client = mqtt_client.Client(client_id)
    client.username_pw_set(username=client_username, password=password)
    client.on_connect = on_connect
    client.tls_set(ca_certs="../security/certs/ca.pem", certfile="../security/certs/client.crt", keyfile="../security/certs/client.key")
    client.connect(broker, port)
    return client

def msg_temperature(mosq, obj, msg):
    values = msg.payload.decode().split(",")
    temperature = values[2]
    ip = values[3]
    mac = values[4]
    if(verify_identity(mac)):
      print("Received " + msg.payload.decode() + " from " + msg.topic + " topic")
      insert_temperature(ip, mac, temperature, db_pass)

def msg_pressure(mosq, obj, msg):
    values = msg.payload.decode().split(",")
    pressure = values[2]
    ip = values[3]
    mac = values[4]
    if(verify_identity(mac)):
      print("Received " + msg.payload.decode() + " from " + msg.topic + " topic")
      insert_pressure(ip, mac, pressure, db_pass)

def msg_clients_conn(mosq, obj, msg):
    clients_conn = msg.payload.decode()
    if(verify_identity(mac)):
      insert_clients_conn(clients_conn, db_pass)
      print("CLIENTS: " + clients_conn)

def msg_sent_total(mosq, obj, msg):
    sent_total = msg.payload.decode()
    if(verify_identity(mac)):
      insert_sent_total(sent_total, db_pass)
      print("SENT_TOTAL: " + sent_total)

def msg_received_total(mosq, obj, msg):
    received_total = msg.payload.decode()
    if(verify_identity(mac)):
      insert_received_total(received_total, db_pass)
      print("RECV_TOTAL: " + received_total)

def msg_subs_count(mosq, obj, msg):
    subs_count = msg.payload.decode()
    if(verify_identity(mac)):
      insert_subs_count(subs_count, db_pass)
      print("SUBS_COUNT: " + subs_count)

def run_mqtt():
    client = connect_mqtt()
    client.message_callback_add(topictemp, msg_temperature)
    client.message_callback_add(topicpress, msg_pressure)
    client.message_callback_add('$SYS/broker/clients/connected', msg_clients_conn)
    client.message_callback_add('$SYS/broker/messages/sent', msg_sent_total)
    client.message_callback_add('$SYS/broker/messages/received', msg_received_total)
    client.message_callback_add('$SYS/broker/subscriptions/count', msg_subs_count)
    client.subscribe(topictemp)
    client.subscribe(topicpress)
    client.subscribe('$SYS/broker/clients/connected')
    client.subscribe('$SYS/broker/messages/sent')
    client.subscribe('$SYS/broker/messages/received')
    client.subscribe('$SYS/broker/subscriptions/count')
    client.loop_forever()
    
class PressureResource(resource.Resource):
    def __init__(self):
        super().__init__()

    def set_content(self, content):
        self.content = content

    async def render_put(self, request):
        self.set_content(request.payload)
        data = request.payload.decode('utf-8')
        print('CoAP: \n\tReceived '+data)
        values = data.split(",")
        pressure = values[2]
        ip = values[3]
        mac = values[4]
        insert_pressure(ip, mac, pressure, db_pass)
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content)
        
class TemperatureResource(resource.Resource):
    def __init__(self):
        super().__init__()

    def set_content(self, content):
        self.content = content

    async def render_put(self, request):
        self.set_content(request.payload)
        data = request.payload.decode('utf-8')
        print('CoAP: \n\tReceived '+data)
        values = data.split(",")
        temperature = values[2]
        ip = values[3]
        mac = values[4]
        insert_temperature(ip, mac, temperature, db_pass)
        return aiocoap.Message(code=aiocoap.CHANGED, payload=self.content)

def main():

    getClient = os.popen("mosquitto_ctrl -u " + username + " -P " + password + " -h " + broker + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec getClient " + client_username + " 2>&1").read()
    if (getClient.find("Error: Client not found")):
      # Creating the new client with an iot_device role
      os.popen("mosquitto_ctrl -u " + username + " -P " + password + " -h " + broker + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec createClient " + client_username + " -c " + client_id + " -p " + password + " 2>&1")
      os.popen("mosquitto_ctrl -u " + username + " -P " + password + " -h " + broker + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec addClientRole " + client_username + " admin" + " 2>&1")
    elif (getClient.find("Error")):
      print("ERR: Could not register device")
      return sys.exit(1)

    t1 = Thread(target=run_mqtt, args=())
    t1.start()

    root = resource.Site()
    root.add_resource(['pressure'], PressureResource())
    root.add_resource(['temp'], TemperatureResource())
    asyncio.Task(aiocoap.Context.create_server_context(root))
    print("Connected to CoAP Server!")
    asyncio.get_event_loop().run_forever()
    
    t1.join()
    
    Session.remove()
    db.close()
    
main()
