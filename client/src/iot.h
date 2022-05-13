#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include "mraa.h"

const auto TIMEOUT = std::chrono::seconds(10);
const auto PERIOD = std::chrono::seconds(5);
const int MAX_BUFFERED_MSGS = 120;	// 120 * 5sec => 10min off-line buffering

// Helper functions needed for collecting IoT sensor data
void setup();
void SetupPressureSensor();
void toggleOneShot();
float ReadPressure();
float ReadTemperature();