#!/bin/sh

# Compiling the client code
echo -n "Compiling Client Code ... "
{
  cd client
  make
  cd ..
} > /dev/null
echo "OK"

# The IP Address of the broker is required to start the client
cd client
echo -n "MQTT Broker IP Address: "
read mqtt_broker
./bin/mqtt_client.exe $mqtt_broker
cd ..
