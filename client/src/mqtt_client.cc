// This is a Paho MQTT C++ client, sample application.
//
// It's an example of how to collect and publish periodic data to MQTT, as
// an MQTT publisher using the C++ asynchronous client interface.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker
//  - Publishing messages
//  - Using a topic object to repeatedly publish to the same topic.
//  - Automatic reconnects
//  - Off-line buffering
//  - User file-based persistence with simple encoding.
//
// This just uses the steady clock to run a periodic loop. Each time
// through, it generates a random number [0-100] as simulated data and
// creates a text, CSV payload in the form:
//  	<sample #>,<time stamp>,<data>
//
// Note that it uses the steady clock to pace the periodic timing, but then
// reads the system_clock to generate the timestamp for local calendar time.
//
// The sample number is just a counting integer to help test the off-line
// buffering to easily confirm that all the messages got across.
//

/*******************************************************************************
 * Copyright (c) 2013-2020 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <termios.h>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mqtt/async_client.h"
#include "iot.h"

using namespace std;
using namespace std::chrono;

const string DFLT_ADDRESS { "localhost" };

const string CA_FILE	{ "../security/certs/ca.crt" };
const string CERT_FILE { "../security/certs/client.crt" };
const string KEY_FILE { "../security/certs/client.key" };
const string PEM_FILE	{ "../security/certs/client.pem" };

const string LWT_TOPIC { "events/disconnect" };
const string LWT_PAYLOAD { "Last will and testament." };
const int QOS = 1;

/////////////////////////////////////////////////////////////////////////////

// A callback class for use with the main MQTT client
class callback : public virtual mqtt::callback {
  public:
    void connection_lost(const string& cause) override {
      cout << "\nConnection lost" << endl;
      if (!cause.empty())
        cout << "\tcause: " << cause << endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override {
      cout << "OK" << endl;
    }
};

string exec(string command) {
   char buffer[128];
   string result = "";
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }
   while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }
   pclose(pipe);
   return result;
}

class Terminal {
  termios oldt;
  termios newt;
  public:
    Terminal() {
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~ECHO;
    }
    void echoOff() { tcsetattr(STDIN_FILENO, TCSANOW, &newt); }
    void echoOn() { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }
};

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  // Setting the address of the broker and grabbing client information
	string address = (argc > 1) ? string(argv[1]) : DFLT_ADDRESS;
	string ip = exec("python3 ../python/get_ip.py");
	string mac = exec("ifconfig wlan0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'");
	ip = ip.substr(0, ip.length() - 1);
	mac = mac.substr(0, mac.length() - 1);
  string client_username = "device-" + mac;
  string client_id = "device-" + mac;

  // Checking certificates
  cout << endl << "Starting device " + mac + " on " + ip + " ... ";
	{
		ifstream tstore(CA_FILE);
		if (!tstore) {
			cerr << "ERR: The ca file does not exist" << endl;
			return EXIT_FAILURE;
		}
		ifstream kstore(PEM_FILE);
		if (!kstore) {
			cerr << "ERR: The pem file does not exist" << endl;
			return EXIT_FAILURE;
		}
	}
  cout << "OK" << endl << endl;
  
  // Setting up the terminal
  Terminal* terminal = new Terminal();

  // Obtaining the admin username and password
  string username = "";
  string password = "";
  cout << "Please enter your MQTT-Admin username and password." << endl;
  cout << "Username: ";
  cin >> username;
  cout << "Password: ";
  terminal->echoOff();
  cin >> password;
  terminal->echoOn();
  cout << endl << endl;

  // Registering the client
  cout << "Registering device ... ";
  string getClient = exec("mosquitto_ctrl -u " + username + " -P " + password + " -h " + address + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec getClient " + client_username + " 2>&1");
  if (getClient.find("Error: Client not found") != string::npos) {
    // Creating the new client with an iot_device role
    exec("mosquitto_ctrl -u " + username + " -P " + password + " -h " + address + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec createClient " + client_username + " -c " + client_id + " -p " + password + " 2>&1");
    exec("mosquitto_ctrl -u " + username + " -P " + password + " -h " + address + " --cafile " + CA_FILE + " --cert " + CERT_FILE + " --key " + KEY_FILE + " dynsec addClientRole " + client_username + " iot_device" + " 2>&1");
  }
  else if (getClient.find("Error") != string::npos) {
    cerr << "ERR: Could not register device" << endl;
		return EXIT_FAILURE;
  }
  cout << "OK" << endl;

  address = "ssl://" + address + ":8883";
	mqtt::async_client client(address, client_id, MAX_BUFFERED_MSGS, NULL);
	callback cb;
	client.set_callback(cb);

	auto sslopts = mqtt::ssl_options_builder()
    .trust_store(CA_FILE)
		.key_store(PEM_FILE)
		.error_handler([](const string& msg) {
		  cerr << "SSL Error: " << msg << endl;
		})
		.finalize();

	auto willmsg = mqtt::message(LWT_TOPIC, LWT_PAYLOAD, QOS, true);

	auto connOpts = mqtt::connect_options_builder()
		.keep_alive_interval(MAX_BUFFERED_MSGS * PERIOD)
		.clean_session(true)
		.automatic_reconnect(true)
    .user_name(client_username)
		.password(password)
		.will(move(willmsg))
		.ssl(move(sslopts))
		.finalize();

	// Intializing the pressure and temperature sensors
	float pressure = 0;
	float temperature = 0;
	setup();

	try {
		// Connect to the MQTT broker
		cout << "Connecting ... ";
		mqtt::token_ptr conntok = client.connect(connOpts);
		conntok->wait();
		cout << "OK" << endl;

		char tmbuf[32];
		unsigned nsample = 0;

		// The time at which to reads the next sample, starting now
		auto tm = steady_clock::now();

		while (true) {
			// Pace the samples to the desired rate
			this_thread::sleep_until(tm);

			// Get a timestamp and format as a string
			time_t t = system_clock::to_time_t(system_clock::now());
			strftime(tmbuf, sizeof(tmbuf), "%F %T", localtime(&t));

			// Reading pressure data
			pressure = ReadPressure();

			// Create the payload as a text CSV string
			cout << "\nSending message ... ";
			string payload = to_string(++nsample) + "," + tmbuf + "," + to_string(pressure) + "," + ip + "," + mac;
			auto msg = mqtt::make_message("data/pressure", payload, QOS, false);
			client.publish(msg)->wait_for(TIMEOUT);
      cout << payload << endl;
			
			// Reading temperature data
			temperature = ReadTemperature();
			
			// Create the payload as a text CSV string
			cout << "\nSending message ... ";
			payload = to_string(++nsample) + "," + tmbuf + "," + to_string(temperature) + "," + ip + "," + mac;
			msg = mqtt::make_message("data/temperature", payload, QOS, false);
			client.publish(msg)->wait_for(TIMEOUT);
      cout << payload << endl;

			tm += PERIOD;
		}

		// Disconnect
		cout << "\nDisconnecting ... " << flush;
		client.disconnect()->wait();
		cout << "OK" << endl;
	}
	catch (const mqtt::exception& exc) {
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

 	return EXIT_SUCCESS;
}
