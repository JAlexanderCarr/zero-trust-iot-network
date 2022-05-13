from sqlcipher3 import dbapi2 as sqlcipher
from datetime import datetime as dt
import getpass

# This password is required to unencrypt and access the database
print("This database is encrypted and will be password protected.")
password = getpass.getpass(prompt = 'Please enter a password: ')

# The database file stores all encrypted database information
# The pressure and temperature tables store data collected from the IoT devices
# The other 4 tables save statistics collected from the MQTT broker
db = sqlcipher.connect('database.db')
db.execute('pragma key = "' + password + '"')
db.execute('create table temperature (ip text, mac text, date text primary key, temperature real)')
db.execute('create table pressure (ip text, mac text, date text primary key, pressure real)')
db.execute('create table clients_conn (clients_conn int, date text primary key)')
db.execute('create table sent_total (sent_total int, date text primary key)')
db.execute('create table received_total (received_total int, date text primary key)')
db.execute('create table subs_count (subs_count int, date text primary key)')
db.commit()
