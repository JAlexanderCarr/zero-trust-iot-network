#!/bin/sh

# Compiling the client code
echo -n "Compiling Client Code ... "
{
  cd client
  make
  cd ..
} &> /dev/null
echo "OK"

# The IP Address of the broker is required to start the client
echo -n "MQTT Broker IP Address: "
read mqtt_broker
cd client
./bin/client.exe $mqtt_broker
cd ..
