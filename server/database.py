from sqlcipher3 import dbapi2 as sqlcipher
from datetime import datetime as dt

# Name of filed created in create_database.py
DATABASE_FILE = "database.db"

# All of these functions directly insert or obtain data from the database
def insert_temperature(ip, mac, temp, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into temperature values (?, ?, ?, ?)', (ip, mac, dt.now(), temp))
  db.commit()

def insert_pressure(ip, mac, press, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into pressure values (?, ?, ?, ?)', (ip, mac, dt.now(), press))
  db.commit()

def get_press(ip, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT pressure FROM pressure WHERE ip = (?) ORDER BY date DESC', (ip,)).fetchall()[0][0]

def get_temp(ip, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT temperature FROM temperature WHERE ip = (?) ORDER BY date DESC', (ip,)).fetchall()[0][0]

def insert_clients_conn(clients_conn, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into clients_conn values (?, ?)', (clients_conn, dt.now()))
  db.commit()

def insert_subs_count(subs_count, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into subs_count values (?, ?)', (subs_count, dt.now()))
  db.commit()

def insert_sent_total(sent_total, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into sent_total values (?, ?)', (sent_total, dt.now()))
  db.commit()

def insert_received_total(received_total, db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  db.execute('insert into received_total values (?, ?)', (received_total, dt.now()))
  db.commit()

def get_clients_conn(db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT clients_conn FROM clients_conn ORDER BY date DESC LIMIT 1').fetchall()[0][0]

def get_subs_count(db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT subs_count FROM subs_count ORDER BY date DESC LIMIT 1').fetchall()[0][0]

def get_sent_total(db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT sent_total FROM sent_total ORDER BY date DESC LIMIT 1').fetchall()[0][0]
  
def get_received_total(db_pass):
  db = sqlcipher.connect(DATABASE_FILE)
  db.execute('pragma key= "' + db_pass + '"')  
  return db.execute('SELECT received_total FROM received_total ORDER BY date DESC LIMIT 1').fetchall()[0][0]
