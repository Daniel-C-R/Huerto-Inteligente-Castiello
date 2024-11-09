import paho.mqtt.client as mqtt
import time

broker = 'localhost'
port = 1883
topic = "test/topic"

def connect_mqtt():
    client = mqtt.Client()
    client.connect(broker, port)
    return client

def publish(client):
    msg_count = 0
    while True:
        time.sleep(1)
        msg = f"Mensaje {msg_count}"
        result = client.publish(topic, msg)
        status = result[0]
        if status == 0:
            print(f"Enviado `{msg}` al tema `{topic}`")
        else:
            print(f"Error al enviar mensaje al tema {topic}")
        msg_count += 1

client = connect_mqtt()
publish(client)
