import paho.mqtt.client as paho
import influxdb_client, os
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import json
import logging
import my_secrets

# Configure the logging system
dirname = os.path.dirname(__file__)
logging.basicConfig(
    filename=os.path.join(dirname, "app.log"),
    level=logging.INFO,
    format="%(levelname)s:%(asctime)s:%(message)s",
)

# suscribe to mosquitto broker
subscriber_client = paho.Client()

# publish to influxdb
influx_client = influxdb_client.InfluxDBClient(
    url=my_secrets.INFLUX_URL, token=my_secrets.INFLUX_TOKEN, org=my_secrets.INFLUX_ORG
)

write_api = influx_client.write_api(write_options=SYNCHRONOUS)


def on_subscribe(client, userdata, mid, granted_qos):
    logging.info("Subscribed: " + str(mid) + " " + str(granted_qos))


def on_message(client, userdata, msg):
    point = (
        Point("soil_mositure")
        .tag("deviceName", "wioTerminal")
        .field("percent", json.loads(msg.payload)["soil_moisture"])
    )
    write_api.write(
        bucket=my_secrets.INFLUX_BUCKET, org=my_secrets.INFLUX_ORG, record=point
    )


subscriber_client.on_subscribe = on_subscribe
subscriber_client.on_message = on_message
subscriber_client.connect(my_secrets.BROKER, 1883)
subscriber_client.subscribe(my_secrets.MOSQUITTO_ID + "/telemetry", qos=1)
subscriber_client.loop_forever()
