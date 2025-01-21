import paho.mqtt.client as mqtt
from database import insert_row
from json import loads
from datetime import datetime

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

    # Decodificar el mensaje JSON
    data = loads(json)

    # Añadir el timestamp actual
    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    print(f'Recibido desde {msg.topic} con timestamp {current_time} el siguiente mensaje: {json}')

    # Llamar a la función para insertar en la base de datos
    values = [*data.values()]
    insert_row(msg.topic.split('/')[1], values[0], current_time, values[1])


def susbcribe(client, topic):
    client.subscribe(topic)
    client.on_message = on_message


client = connect_mqtt()
for topic in topics:
    susbcribe(client, topic)
client.loop_forever()
