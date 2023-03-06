#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <rpcWiFi.h>
#include <SPI.h>

#include "config.h"

// TODO: LCD Display Soil Moisture + turn off display after a period of time
// TODO: use SERVER_COMMAND_TOPIC to issue a display turn off/on command

WiFiClient wioClient;
PubSubClient client(wioClient);
int moisture_adc;

/// @brief connect wio terminal to wifi
void connectWiFi()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting to WiFi..");
        WiFi.begin(settings::SSID, settings::PASSWORD);
        delay(500);
    }

    Serial.println("Connected!");
}

/// @brief function to call whenever `client.loop` is run
/// @param topic the MQTT topic
/// @param payload the value to send to the topic
/// @param length length of the payload
void clientCallback(char *topic, uint8_t *payload, unsigned int length)
{
    char buff[length + 1];
    for (unsigned int i = 0; i < length; i++)
    {
        buff[i] = (char)payload[i];
    }
    buff[length] = '\0';

    Serial.print("Message received:");
    Serial.println(buff);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, buff);
    JsonObject obj = doc.as<JsonObject>();

    bool relay_on = obj["relay_on"];

    if (relay_on)
        digitalWrite(PIN_WIRE_SCL, HIGH);
    else
        digitalWrite(PIN_WIRE_SCL, LOW);
}

/// @brief connects to MQTT broker if disconnected or not connected at all
void reconnectMQTTClient()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");

        if (client.connect(settings::CLIENT_NAME.c_str()))
        {
            Serial.println("connected");
            client.subscribe(settings::SERVER_COMMAND_TOPIC.c_str());
        }
        else
        {
            Serial.print("Retying in 5 seconds - failed, rc=");
            Serial.println(client.state());

            delay(5000);
        }
    }
}

/// @brief Setup connection to MQTT broker. Also uses the `clientCallback` as the
///        callback function (the function called every iteration of of `client.loop()).
void createMQTTClient()
{
    client.setServer(settings::BROKER.c_str(), 1883);
    client.setCallback(clientCallback);
    reconnectMQTTClient();
}

void setup()
{
    Serial.begin(9600);

    // pin setup
    pinMode(A0, INPUT);
    pinMode(PIN_WIRE_SCL, OUTPUT);

    // mqtt client setup
    connectWiFi();
    createMQTTClient();
}

void loop()
{
    reconnectMQTTClient();
    client.loop();

    moisture_adc = analogRead(A0);

    DynamicJsonDocument doc(1024);
    doc["soil_moisture"] = map(
        moisture_adc,
        settings::MOISTURE_ADC_AIR_VALUE,
        settings::MOISTURE_ADC_WATER_VALUE,
        0,
        100);

    std::string telemetry;
    serializeJson(doc, telemetry);

    Serial.print("Sending telemetry ");
    Serial.println(telemetry.c_str());
    Serial.println(settings::CLIENT_TELEMETRY_TOPIC.c_str());
    client.publish(settings::CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str());

    delay(10000); // 10 seconds
}
