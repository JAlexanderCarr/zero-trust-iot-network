#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include "common.h"
#include "iot.h"

using namespace std;
using namespace std::chrono;

const string DFLT_ADDRESS { "localhost" };
const string CLIENT_ID { "paho-cpp-data-publish" };

static char certificate[] = "../security/certs/ca.crt";
static char certificate_authority[] = "../security/certs/ca.crt";
static char private_key[] = "../security/certs/ca.key";

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

/////////////////////////////////////////////////////////////////////////////

// Code for DTLS stuff
/*
   * Create unique token for this request for handling unsolicited /
   * delayed responses
   */
  // coap_session_new_token(session, &buflen, buf);
  // if (!coap_add_token(pdu, buflen, buf)) {
  //   coap_log(LOG_DEBUG, "cannot add token to request\n");
  //   goto error;
  // }
  // https://libcoap.net/doc/reference/4.3.0/man_coap_encryption.html

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	string address = (argc > 1) ? string(argv[1]) : DFLT_ADDRESS;
	
	string ip = exec("python3 ../python/get_ip.py");
	string mac = exec("ifconfig wlan0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'");
	string ipshort = ip.substr(0, ip.length() - 1);
	string macshort = mac.substr(0, mac.length() - 1);

  // Declaring COAP variables
	coap_context_t  *ctx = nullptr;
  coap_session_t *session = nullptr;
  coap_address_t dst;
  coap_pdu_t *pdu = nullptr;
  coap_startup();

  // Resolve destination address where server should be sent
  if (resolve_address(address.c_str(), "5683", &dst) < 0) {
    coap_log(LOG_CRIT, "failed to resolve address\n");
    coap_session_release(session);
    coap_free_context(ctx);
    coap_cleanup();
    return EXIT_FAILURE;
  }

  set_certificate(certificate);
  set_certificate_authority(certificate_authority);
  set_private_key(private_key);
  
// 	Create CoAP context and a client session
  ctx = coap_new_context(nullptr);
  // if (!coap_context_set_pki_root_cas(ctx, NULL, certificate_pem)) {
  //   coap_log(LOG_DEBUG, "CAS failed");
  // }
  if (!ctx || !(session = coap_new_client_session(ctx, nullptr, &dst, COAP_PROTO_UDP))) {
    coap_log(LOG_EMERG, "cannot create client session\n");
    coap_session_release(session);
    coap_free_context(ctx);
    coap_cleanup();
    return EXIT_FAILURE;
  }

  // coap_dtls_pki_t *dtls_pki = setup_pki(COAP_DTLS_ROLE_CLIENT);
  // if (!ctx || !(session = coap_new_client_session_pki(ctx, NULL, &dst, COAP_PROTO_DTLS, dtls_pki))) {
  //   coap_log(LOG_EMERG, "cannot create client session\n");
  //   coap_session_release(session);
  //   coap_free_context(ctx);
  //   coap_cleanup();
  //   return EXIT_FAILURE;
  // }

// 	// coap_register_response_handler(ctx, response_handler);
  coap_register_response_handler(ctx, [](auto, auto, const coap_pdu_t *received, auto) {
    coap_show_pdu(LOG_WARNING, received);
    return COAP_RESPONSE_OK;
  });

	// Intializing the pressure and temperature sensors
	float pressure = 0;
	float temperature = 0;
  setup();

	try {

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

			// Constructing CoAP message
      pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_PUT, coap_new_message_id(session), coap_session_max_pdu_size(session));
      if (!pdu) {
        coap_log( LOG_EMERG, "cannot create PDU\n" );
        coap_session_release(session);
        coap_free_context(ctx);
        coap_cleanup();
        return EXIT_FAILURE;
      }

      // Create the payload as a text CSV string
			string payload = to_string(++nsample) + "," + tmbuf + "," + to_string(pressure) + "," + ipshort + "," + macshort;
			cout << payload << endl;

      // Add a Uri-Path option
      coap_add_option(pdu, COAP_OPTION_URI_PATH, 8, reinterpret_cast<const uint8_t *>("pressure"));
      coap_add_data(pdu, payload.length(), reinterpret_cast<const uint8_t *>(payload.c_str()));

      // Sending the message
      coap_show_pdu(LOG_WARNING, pdu);
      coap_send(session, pdu);
      coap_io_process(ctx, COAP_IO_WAIT);
      
      // Reading temperature data
		  temperature = ReadTemperature();

			// Constructing CoAP message
      pdu = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_PUT, coap_new_message_id(session), coap_session_max_pdu_size(session));
      if (!pdu) {
        coap_log( LOG_EMERG, "cannot create PDU\n" );
        coap_session_release(session);
        coap_free_context(ctx);
        coap_cleanup();
        return EXIT_FAILURE;
      }

      // Create the payload as a text CSV string
			payload = to_string(++nsample) + "," + tmbuf + "," + to_string(temperature) + "," + ipshort + "," + macshort;

      // Add a Uri-Path option
      coap_add_option(pdu, COAP_OPTION_URI_PATH, 4, reinterpret_cast<const uint8_t *>("temp"));
      coap_add_data(pdu, payload.length(), reinterpret_cast<const uint8_t *>(payload.c_str()));

      // Sending the message
      coap_show_pdu(LOG_WARNING, pdu);
      coap_send(session, pdu);
      coap_io_process(ctx, COAP_IO_WAIT);


			tm += PERIOD;
		}

	}
	catch (const std::exception& exc) {
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

  coap_session_release(session);
  coap_free_context(ctx);
  coap_cleanup();
 	return EXIT_SUCCESS;
}
