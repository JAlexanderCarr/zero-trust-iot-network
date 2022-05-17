# Zero Trust IoT Network

This repository contains the source code for the Zero Trust Architecture on Internet of Things Networks capstone project.  This project was completed by Alexander Carr and Emma Mirabelli during their senior year at Lehigh University as a part of the Electrical and Computer Engineering Department.

This code can be used to run an MQTT broker, server, and web interface on a local WiFi network, which receives data from IoT devices.  Below is more information on configuring the Zero Trust Architecture and setting up the network.
 
## Requirements

The applications developed in this project were used and test strictly on Linux devices.

* Server, MQTT broker, and web interface tested on a device with Ubuntu v20.04
* IoT Client tested on a Keysight U3810A IoT Educational Kit with Debian 10
  * Had MRAA sensor library support

It is recommended to use these exact versions or devices in order to replicate the intended functions of this project.

While testing this project, the MQTT broker, server, and web interface were all deployed on the same machine.  It is required that these three applications are all running on the same machine in order to function correctly.

### Server & Web Interface

The server and web interface both are python files that are run in a virtual environment.  In order to set up the virtual environment, the following dependencies must be installed:

```
$ sudo apt-get install openssl python3-pip python3-venv python3-dev \
    libffi-dev python3-setuptools
```

The server also utilizes a version of sqlite3 that is configured to work with python and supports an encrypted database.  Because the server persists data that needs to be encrypted, it must be installed using the commands below:

```
$ sudo apt-get install libsqlcipher-dev
$ sudo apt-get install libsqlite3-dev
$ sudo apt-get install sqlcipher
$ git clone https://github.com/rigglemania/pysqlcipher3
$ cd pysqlcipher3
$ python3 setup.py build
$ sudo python3 setup.py install
```

After installing sqlcipher, the python virtual environment needs to be created and configured.  This should be done in the source directory of this repo as shown below:

```
$ python3 -m venv venv
$ source venv/bin/activate
$ python3 -m pip install -r requirements.txt
$ deactivate
```

### MQTT Broker

The MQTT broker will be hosted locally using a service called [mosquitto].  The dynamic security plugin will be utilized for authentication, which requires mosquitto to be at least v2.0 or greater.  It can be installed as shown below:

```
$ sudo apt-get install mosquitto
```

The mosquitto version can be checked with ```mosquitto -h```.

### IoT Client

The IoT client is written in C++ using the [MQTT Paho Library].  This library requires the following tools to install:

* CMake v3.5 or newer
* GCC v4.8 or newer or Clang v3.9 or newer
* GNU Make

They can be installed using the commands below:

```
$ sudo apt-get install build-essential gcc make cmake
```

In order to build and install Paho MQTT, both the C and C++ versions need to be installed.  First, install [paho.mqtt.c] as shown below:

```
$ git clone https://github.com/eclipse/paho.mqtt.c.git
$ cd paho.mqtt.c
$ git checkout v1.3.8
$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

Then, install [paho.mqtt.cpp] as shown below:

```
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp
$ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
    -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

## Deployment

Below are the directions to deploy each of the applications required to set up the network.  On the first deployment, there are some neccessary steps for setup that are not required when restarting the network.  These steps will be indicated as such below.

It is necessary to deploy applications in the order they are listed, as they connect on startup to form the network.

### MQTT Broker



### Server



### Web Interface



### IoT Client


