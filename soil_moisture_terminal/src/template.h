#pragma once

#include <string>

namespace settings
{
    // WiFi credentials
    const char *SSID = "<YOUR_WIFI_ID>";
    const char *PASSWORD = "<YOUR_WIFI_PASSWORD>";

    // you should calibrate these values
    const int MOISTURE_ADC_AIR_VALUE = 614;   // adc value when the moisture sensor is completely dry
    const int MOISTURE_ADC_WATER_VALUE = 370; // adc value when the moisture sensor is completely wet

    // MQTT settings
    // create a GUID with a tool like https://guidgenerator.com/
    // we use this because we are using a public MQTT broker
    // adding this GUID ensures that we have a unique MQTT topic.
    const std::string ID = "<YOUR_GUID>";

    const std::string BROKER = "broker.hivemq.com"; //"test.mosquitto.org";
    const std::string CLIENT_NAME = ID + "soilmoisturesensor_client";

    const std::string CLIENT_TELEMETRY_TOPIC = ID + "/telemetry";
    const std::string SERVER_COMMAND_TOPIC = ID + "/commands";

}
