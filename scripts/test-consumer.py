import paho.mqtt.client as mqtt

broker = 'localhost'
port = 1883
topic = "test/topic"

def connect_mqtt():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.connect(broker, port)
    return client

def on_message(client, userdata, msg):
    print(f"Recibido `{msg.payload.decode()}` del tema `{msg.topic}`")

def subscribe(client):
    client.subscribe(topic)
    client.on_message = on_message

client = connect_mqtt()
subscribe(client)
client.loop_forever()
