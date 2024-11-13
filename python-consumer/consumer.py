import paho.mqtt.client as mqtt


broker = 'mosquitto'
port = 1883

topics = [
    'huerto/humidity',
    'huerto/temperature',
    'huerto/pressure'
]


def connect_mqtt():
    print("Realizando conexión con el broker.")
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.connect(broker, port)
    return client


def on_message(client, userdata, msg):
    print(
        f'Recibido desde {msg.topic} el siguiente mensaje: {msg.payload.decode()}')


def susbcribe(client, topic):
    client.subscribe(topic)
    client.on_message = on_message


client = connect_mqtt()
for topic in topics:
    susbcribe(client, topic)
client.loop_forever()