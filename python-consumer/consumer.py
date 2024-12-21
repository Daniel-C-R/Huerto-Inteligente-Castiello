import paho.mqtt.client as mqtt
from database import insert_row
from json import loads


broker = 'mosquitto'
port = 1883

topics = [
    'huerto/humidity',
    'huerto/temperature',
    'huerto/pressure',
    'huerto/altitude',
    'huerto/soil_moisture',
]


def connect_mqtt():
    print("Realizando conexión con el broker.")
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.connect(broker, port)
    return client


def on_message(client, userdata, msg):
    json = msg.payload.decode()
    print(
        f'Recibido desde {msg.topic} el siguiente mensaje: {json}')
    data = loads(json)
    insert_row(msg.topic.split('/')[1], *data.values())


def susbcribe(client, topic):
    client.subscribe(topic)
    client.on_message = on_message


client = connect_mqtt()
for topic in topics:
    susbcribe(client, topic)
client.loop_forever()