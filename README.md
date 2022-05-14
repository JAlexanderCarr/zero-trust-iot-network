# Zero Trust IoT Network

This repository contains the source code for the Zero Trust Architecture on Internet of Things Networks capstone project.  This project was completed by Alexander Carr and Emma Mirabelli during their senior year at Lehigh University as a part of the Electrical and Computer Engineering Department.

This code can be used to run an MQTT broker, server, and web interface on a local WiFi network, which receives data from IoT devices.  Below is more information on configuring the Zero Trust Architecture and setting up the network.
 
## Requirements

The applications developed in this project were used and test strictly on Linux devices.  Specifically, the server, MQTT broker, and web interface were tested on a device running Ubuntu v20.04 and the IoT Client was run on a Keysight U3810A IoT Educational Kit, which had mraa sensor library support and was running Debian 10.  It is recommended to use these exact versions or devices in order to replicate the intended functions of this project.

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

```



On *nix systems CMake creates Makefiles.

The build process currently supports a number of Unix and Linux flavors. The build process requires the following tools:

  * CMake v3.5 or newer
  * GCC v4.8 or newer or Clang v3.9 or newer
  * GNU Make

On Debian based systems this would mean that the following packages have to be installed:

```
$ sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui
```

If you will be using secure sockets (and you probably should):

```
$ sudo apt-get install libssl-dev 
```

Building the documentation requires doxygen and optionally graphviz to be installed:

```
$ sudo apt-get install doxygen graphviz
```

Unit tests are being built using _Catch2_. 

_Catch2_ can be found here: [Catch2](https://github.com/catchorg/Catch2).  You must download and install _Catch2_ to build and run the unit tests locally.

#### Building the Paho C library

Before building the C++ library, first, build and install the Paho C library, if not already present. Note, this version of the C++ library requires Paho C v1.3.8 or greater.

```
$ git clone https://github.com/eclipse/paho.mqtt.c.git
$ cd paho.mqtt.c
$ git checkout v1.3.8
$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

This builds with SSL/TLS enabled. If that is not desired, omit the `-DPAHO_WITH_SSL=ON`.

It also uses the "high performace" option of the C library to disable more extensive internal memory checks. Remove the _PAHO_HIGH_PERFORMANCE_ option (i.e. turn it off) to debug memory issues, but for most production systems, leave it on for better performance.

To install the library to a non-standard location, use the `CMAKE_INSTALL_PREFIX` to specify a location. For example, to install into under the build directory, perhaps for local testing, do this:

```
$ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON \
    -DCMAKE_INSTALL_PREFIX=./build/_install
```

#### Building the Paho C++ library

An example CMake build session might look like this:

```
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp
$ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
    -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

If you did not install Paho C library to a default system location or you want to build against a different version, use the `CMAKE_PREFIX_PATH` to specify its install location. Perhaps something like this:

```
$ cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=ON -DPAHO_BUILD_SAMPLES=ON \
    -DPAHO_BUILD_STATIC=ON \
    -DCMAKE_PREFIX_PATH=$HOME/mqtt/paho.mqtt.c/build/_install
```

To use another compiler, either the CXX environment variable can be specified in the configuration step:

```
$ CXX=clang++ cmake ..
```

or the `CMAKE_CXX_COMPILER` flag can be used:

```
$ cmake -DCMAKE_CXX_COMPILER=clang++
```

#### Building a Debian/Ubuntu package

```
$ cmake -Bbuild -H. -DPAHO_WITH_SSL=ON -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_DEB_PACKAGE=ON
$ cmake --build build
$ (cd build && cpack)
```
will generate a `.deb` file.


### Windows

On Windows systems CMake creates Visual Studio project files.

The build process currently supports a number Windows versions. The build process requires the following tools:
  * CMake GUI v3.5 or newer
  * Visual Studio 2015 or newer

First install and open the cmake-gui application. This tutorial is based on cmake-gui 3.5.2.

Second, select the path to the Paho MQTT C library (CMAKE_PREFIX_PATH) if not installed in a standard path. Remember that the Paho MQTT C must be installed on the system. Next, choose if it is supposed to build the documentation (PAHO_BUILD_DOCUMENTATION) and/or the sample applications (PAHO_BUILD_SAMPLES).

Once the configuration is done, click on the Configure button, select the version of the Visual Studio, and then click on Generate button.

At the end of this process you have a Visual Studio solution.

Alternately, the libraries can be completely built at an MSBuild Command Prompt. Download the Paho C and C++ library sources, then open a command window and first compile the Paho C library:

```
> cd paho.mqtt.c
> cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=C:\mqtt\paho-c
> cmake --build build/ --target install
```

Then build the C++ library:

```
> cd ..\paho.mqtt.cpp
> cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=C:\mqtt\paho-cpp -DPAHO_BUILD_SAMPLES=ON -DPAHO_WITH_SSL=OFF -DCMAKE_PREFIX_PATH=C:\mqtt\paho-c
> cmake --build build/ --target install
```
This builds and installs both libraries to a non-standard location under `C:\mqtt`. Modify this location as desired or use the default location, but either way, the C++ library will most likely need to be told where the C library was built using `CMAKE_PREFIX_PATH`.

It seems quite odd, but even on a 64-bit system using a 64-bit compiler, MSVC seems to default to a 32-bit build target. 

The 64-bit target can be selected using tge CMake generator switch, *-G*, at configuration time. The full version must be provided. For Visual Studio 2015 which is v14 do this to first build the Paho C library:

```
> cmake -G "Visual Studio 14 Win64" -Bbuild -H. -DCMAKE_INSTALL_PREFIX=C:\mqtt\paho-c
...
```

Then use it to build the C++ library:

```
> cmake -G "Visual Studio 14 Win64" -Bbuild -H. -DCMAKE_INSTALL_PREFIX=C:\mqtt\paho-cpp -DPAHO_WITH_SSL=OFF -DCMAKE_PREFIX_PATH=C:\mqtt\paho-c
...
```

*Note that it is very important that you use the same generator (target) to build BOTH libraries, otherwise you will get lots of linker errors when you try to build the C++ library.*

## Supported Network Protocols

The library supports connecting to an MQTT server/broker using TCP, SSL/TLS, and websockets (secure and unsecure). This is chosen by the URI supplied to the connect() call. It can be specified as:

```
"tcp://<host>:<port>"  - TCP (unsecure)
"ssl://<host>:<port>"  - SSL/TLS
"ws://<host>:<port>"   - Unsecure websockets
"wss://<host>:<port>"  - Secure websockets
```

Note that to use "ssl://" or "wss://" you must compile the library with OpenSSL, and you _must_ supply a set of `ssl_options` with the `connect_options`.

## Example

Sample applications can be found in the source repository at _src/samples_:
https://github.com/eclipse/paho.mqtt.cpp/tree/master/src/samples

This is a partial example of what a typical example might look like:

```cpp
int main(int argc, char* argv[])
{
    sample_mem_persistence persist;
    mqtt::client cli(ADDRESS, CLIENT_ID, &persist);
    callback cb;
    cli.set_callback(cb);
    auto connOpts = mqtt::connect_options_builder() 
        .keep_alive_interval(20);
        .clean_session()
        .finalize();
    try {
        cli.connect(connOpts);
        // First use a message pointer.
        mqtt::message_ptr pubmsg = mqtt::make_message(PAYLOAD1);
        pubmsg->set_qos(QOS);
        cli.publish(TOPIC, pubmsg);
        // Now try with itemized publish.
        cli.publish(TOPIC, PAYLOAD2, strlen(PAYLOAD2)+1, 0, false);
        // Disconnect
        
        cli.disconnect();
    }
    catch (const mqtt::persistence_exception& exc) {
        cerr << "Persistence Error: " << exc.what() << " ["
            << exc.get_reason_code() << "]" << endl;
        return 1;
    }
    catch (const mqtt::exception& exc) {
        cerr << "Error: " << exc.what() << " ["
            << exc.get_reason_code() << "]" << endl;
        return 1;
    }
    return 0;
}
```

-----------

The original API organization and documentation were adapted from:

The Paho Java library
by Dave Locke et al.
Copyright (c) 2012, IBM Corp

 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v1.0
 which accompanies this distribution, and is available at
 http://www.eclipse.org/legal/epl-v10.html

-----------

This code requires:

The Paho C library by Ian Craggs
Copyright (c) 2013-2018, IBM Corp.

 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v1.0
 and Eclipse Distribution License v1.0 which accompany this distribution.

 The Eclipse Public License is available at
    http://www.eclipse.org/legal/epl-v10.html
 and the Eclipse Distribution License is available at
   http://www.eclipse.org/org/documents/edl-v10.php.
