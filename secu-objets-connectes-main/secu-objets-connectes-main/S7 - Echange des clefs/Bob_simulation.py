from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PublicKey
import paho.mqtt.client as mqtt
import time
from DH_elliptic import *
import base64


def on_connect(client, userdata, flags, rc):
    print("Connected with result code => "+mqtt.connack_string(rc))


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


def on_message(client, userdata, msg):
    print("Received message '" + str(msg.payload) +
          "' on topic '" + msg.topic + "' with QoS " + str(msg.qos))
    PIPO = base64.decodebytes(msg.payload)
    pub_key=X25519PublicKey.from_public_bytes(PIPO)
    shared_key_from_alice= B_private_key.exchange(pub_key)
    print("Secret partagé=",shared_key_from_alice.hex())


def on_publish(client, userdata, mid):
    print("--on_publish callback --mid: " + str(mid))


client = mqtt.Client()
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish

client.on_connect = on_connect

B_private_key = Bob_private()
B_public_key = Bob_public(B_private_key)

try:
    # client.username_pw_set(username="G3", password="isimaG3")
    client.connect("172.16.32.7", 1883)  # groupes 1 à 4
    # client.connect("m21.cloudmqtt.com", 13197) # groupes 5 à 8
    # client.connect("m21.cloudmqtt.com", 16511) # groupes 9 à 12
    # client.connect("m21.cloudmqtt.com", 10318) # groupes 13 à 16
except:
    print("Connection Failed")

try:
    client.subscribe("/ISIMA/S7_DH/GROUPE_03/PublicKeyIoT", qos=0)
    client.loop_start()

    while True:
        
        (rc, mid) = client.publish(
            topic="/ISIMA/S7_DH/GROUPE_03/PublicKeyServer", payload=B_public_key.public_bytes(encoding=serialization.Encoding.Raw,format=serialization.PublicFormat.Raw), qos=0)
        print("Error return from publish of mid = " +
              str(mid) + " : " + mqtt.error_string(rc))
        time.sleep(5)

except KeyboardInterrupt:
    client.loop_stop()
    client.unsubscribe("/ISIMA/S7_DH/GROUPE_03/PublicKeyIoT")
    client.disconnect()
    print("Done.")
