#!/bin/sh

# Obtaining the ip for the MQTT Broker for certificates
echo -n "MQTT Broker IP Address: "
read mqtt_broker

# Files for Certificate Authority
openssl genrsa -out certs/ca.key 2048
openssl req -new -x509 -days 365 -key certs/ca.key -out certs/ca.crt -subj "/C=US/ST=Pennsylvania/L=Bethlehem/O=Lehigh University/OU=Capstone/CN=root"
openssl x509 -in certs/ca.crt -text > certs/ca.temp
cat certs/ca.temp certs/ca.key > certs/ca.pem
rm certs/ca.temp

# Files for Server Authentication
openssl genrsa -out certs/server.key 2048
openssl req -new -out certs/server.csr -key certs/server.key -subj "/C=US/ST=Pennsylvania/L=Bethlehem/O=Lehigh University/OU=Capstone/CN=server"
echo "subjectAltName=IP:$mqtt_broker" > certs/ext.temp
openssl x509 -req -in certs/server.csr -CA certs/ca.crt -CAkey certs/ca.key -CAcreateserial -out certs/server.crt -days 365 -extfile certs/ext.temp
rm certs/ext.temp
openssl x509 -in certs/server.crt -text > certs/server.temp
cat certs/server.temp certs/server.key > certs/server.pem
rm certs/server.temp

# Files for Client Authentication
openssl genrsa -out certs/client.key 2048
openssl req -new -out certs/client.csr -key certs/client.key -subj "/C=US/ST=Pennsylvania/L=Bethlehem/O=Lehigh University/OU=Capstone/CN=client"
openssl x509 -req -in certs/client.csr -CA certs/ca.crt -CAkey certs/ca.key -CAcreateserial -out certs/client.crt -days 365
openssl x509 -in certs/client.crt -text > certs/client.temp
cat certs/client.temp certs/client.key > certs/client.pem
rm certs/client.temp
