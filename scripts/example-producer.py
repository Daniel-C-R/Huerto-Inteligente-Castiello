import paho.mqtt.client as mqtt
from datetime import datetime
from json import dumps

broker = 'localhost'
port = 1883


def connect_mqtt():
    client = mqtt.Client()
    client.connect(broker, port)
    return client


def publish(client, topic, data):
    msg = dumps(data)
    result = client.publish(topic, msg)
    status = result[0]

    if status == 0:
        print(f'Se ha enviado a {topic} el siguiente mensaje: {msg}')
    else:
        print(f'Error al enviar a {topic} el siguente mensaje: {msg}')


client = connect_mqtt()

for i in range(10):
    humidity = {
        'controller_id': i,
        'timestamp': str(datetime.now()),
        'humidity': i,
    }
    publish(client, 'huerto/humidity', humidity)
    temperature = {
        'controller_id': i,
        'timestamp': str(datetime.now()),
        'humidity': i,
    }
    publish(client, 'huerto/humidity', humidity)
    pressure = {
        'controller_id': i,
        'timestamp': str(datetime.now()),
        'temperature': i,
    }
    publish(client, 'huerto/humidity', humidity)
