# Zero Trust Architecture on Internet of Things Networks

This repository contains the source code for the Zero Trust Architecture on Internet of Things Networks capstone project.  This project was completed by Alexander Carr and Emma Mirabelli during their senior year at Lehigh University as a part of the Electrical and Computer Engineering Department.

## Description

Internet of Things (IoT) devices have become increasingly common in many households.  Due to the wide range of device types and multitudes of data being relayed over a network, there is no standard level of security for IoT devices.  A Zero Trust Archictecture (ZTA) is an advanced security tactic that removes the assumption of trust given to devices and continually verifies and authenticates devices in a network.  Implementing a ZTA on a network of IoT devices increases the security of the network and ensures safe data transfer between devices.

The outcome of this project is a fully functioning, secure network of IoT devices connected over WiFi and a web interface displaying data and statistics.  All devices in the network are verified upon registration and transaction using multi-factor authentication.  The initial plan supported both MQTT and CoAP communication protocols, however, after extensive implementation testing, we concluded that CoAP does not currently support the necessary encryption functionality.  Therefore, all data is transmitted through TLS-encrypted MQTT.

A demonstration of the functioning project can be found [here](https://www.youtube.com/watch?v=nJFPdWGKYn4).
The final report for the project is in this repository and entitled CREG_258_Final_Report.pdf.
 
## Requirements

This code can be used to run an MQTT broker, server, and web interface on a local WiFi network, which receives data from IoT devices.  Below is more information on configuring the Zero Trust Architecture and setting up the network.

The applications developed in this project were used and tested strictly on Linux devices.

* Server, MQTT broker, and web interface tested on a device with Ubuntu v20.04
* IoT Client tested on a Keysight U3810A IoT Educational Kit with Debian 10
  * Had MRAA sensor library support

It is recommended to use these exact versions or devices in order to replicate the intended functions of this project.

While testing this project, the MQTT broker, server, and web interface were all deployed on the same machine.  It is required that these three applications are all running on the same machine in order to function correctly.

### Server & Web Interface

Both the server and web interface are Python files run in a virtual environment.  In order to set up the virtual environment, the following dependencies must be installed:

```
sudo apt-get install openssl python3-pip python3-venv python3-dev libffi-dev python3-setuptools
```

The server also utilizes a version of sqlite3 that is configured to work with python and supports an encrypted database.  Because the server persists data that needs to be encrypted, sqlite must be installed using the commands below:

```
sudo apt-get install libsqlcipher-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install sqlcipher
git clone https://github.com/rigglemania/pysqlcipher3
cd pysqlcipher3
python3 setup.py build
sudo python3 setup.py install
```

After installing sqlcipher, the python virtual environment needs to be created and configured.  This should be done in the source directory of this repo as shown below:

```
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -r requirements.txt
```

### MQTT Broker

The MQTT broker is hosted locally using a service called [mosquitto](https://mosquitto.org/).  The dynamic security plugin is utilized for authentication, which requires mosquitto to be at least v2.0 or greater.  It can be installed as shown below:

```
sudo apt-get install mosquitto
```

The mosquitto version can be checked with ```mosquitto -h```.

### IoT Client

The IoT client is written in C++ using the [MQTT Paho Library].  This library requires the following tools to install:

* CMake v3.5 or newer
* GCC v4.8 or newer or Clang v3.9 or newer
* GNU Make

They can be installed using the commands below:

```
sudo apt-get install build-essential gcc make cmake
```

In order to build and install Paho MQTT, both the C and C++ versions need to be installed.  First, install [paho.mqtt.c](https://github.com/eclipse/paho.mqtt.c) as shown below:

```
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```

Then, install [paho.mqtt.cpp]() as shown below:

```
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

## Deployment

Below are the directions to deploy each of the applications required to set up the network.  On the first deployment, there are some neccessary steps for setup that are not required when restarting the network.  These steps will be indicated as such below.

It is necessary to deploy applications in the order they are listed, as they connect on startup to form the network.

### MQTT Broker

When deploying the MQTT Broker, it is necessary to be in the ```zero-trust-iot-network/``` directory.  First, attain the IP address of the device acting as the broker as it will be needed to run the clients. Then, start the broker using the following command:

```
mosquitto -c security/broker.conf
```

### Server

On the same device that is currently running the MQTT Broker, activate the python virtual environment and move to the ```zero-trust-iot-network/python/``` directory.

Upon first time deployment only, run the following command to create the SQL database:

```
python3 server/create_database.py
```

Run the server by executing the following command:

```
python3 server.py
```

### Web Interface

While in the ```zero-trust-iot-network/python/``` directory, start the live-updating web interface using the following command:

```
flask run
```

### IoT Client

Migrate to the ```zero-trust-iot-network/client/``` directory to compile and run the client script using the following commands:

```
make
./bin/mqtt_client.exe <IP address of device running MQTT Broker>
```

The network is now fully running with messages and data being relayed between the clients and server. The web interface live-updates with statistics about the network.
## Authors

Alexander Carr - [GitHub](https://github.com/JAlexanderCarr/), [LinkedIn](https://www.linkedin.com/in/jalexandercarr/)

Emma Mirabelli - [GitHub](https://github.com/emm422/), [LinkedIn](https://www.linkedin.com/in/emmamirabelli/), [Website](https://emmamirabelli.com/)

