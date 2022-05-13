#!/bin/sh

# All of these packages are necessary in order to install dependencies and run the server
# sudo apt update
# sudo apt upgrade
# sudo apt-get install build-essential gcc make cmake openssl python3-pip \
#   python3-venv python3-dev libffi-dev python3-setuptools

# The Paho MQTT C++ library is required to run the MQTT client
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
  -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
cd ..
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
  -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
cd ..
rm -rf paho.mqtt.c
rm -rf paho.mqtt.cpp

# The libcoap C++ library is required to run the COAP client
git clone https://github.com/obgm/libcoap.git
cd libcoap
./autogen.sh
./configure --disable-manpages --disable-doxygen
sudo make
sudo make install
ln -s libssl.so.3 libssl.so
sudo ldconfig
cd ..
rm -rf libcoap

# A virtual environment is required to run the server and web interface
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -r requirements.txt

# The sqlcipher library for python is required to setup the database
sudo apt-get install libsqlcipher-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install sqlcipher
git clone https://github.com/rigglemania/pysqlcipher3
cd pysqlcipher3
python3 setup.py build
sudo python3 setup.py install
python3 -m pip install pysqlcipher3
deactivate
cd ..
rm -rf pysqlcipher3
