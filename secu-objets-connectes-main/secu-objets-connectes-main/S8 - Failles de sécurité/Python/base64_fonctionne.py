import paho.mqtt.client as mqtt
import time
import random
import base64
import my_xor
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import os


def add_zeros(n, taille=4):
    """Ajoute les zéros inutiles devant le nombre"""
    s = str(n)
    while (len(s) < taille):
        s = "0" + s
    return s


def la_baleine(msg, nb_sel=8):
    '''Renvoie le message salé'''
    saltchar = "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    for i in range(nb_sel):
        msg += saltchar[random.randint(0, len(saltchar) - 1)]
    return msg


def on_connect(client, userdata, flags, rc):
    print("Connected with result code => "+mqtt.connack_string(rc))


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


def on_message(client, userdata, msg):
    # Déchiffrer AES
    # decryptor = cipher.decryptor()
    # decryptor.update(ct) + decryptor.finalize()

    # Déchiffrement 64
    decode = base64.standard_b64decode(msg.payload).decode("utf-8")
    # Xor
    decode = my_xor.Xor(decode)
    # Désaler et enlever compteur
    decode = decode[:-12]
    print("Received message '" + decode +
          "' on topic '" + msg.topic + "' with QoS " + str(msg.qos))



def on_publish(client, userdata, mid):
    print("--on_publish callback --mid: " + str(mid))


client = mqtt.Client()
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_publish = on_publish
client.on_connect = on_connect

my_topic = "/ISIMA/TIBO/"
clef = "GWAA"

try:
    # client.username_pw_set(username="G3", password="isimaG3")
    client.connect("localhost", 1883)
except:
    print("Connection Failed")

try:
    client.subscribe(my_topic, qos=0)
    client.loop_start()

    comp = 0

    # AES
    # key = os.urandom(32)
    # iv = os.urandom(16)
    # cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
    # encryptor = cipher.encryptor()
    
    for i in range(16):
        my_payload_salted = la_baleine(
            ("ON" if i % 2 == 0 else "OFF") + add_zeros(comp, 4), 8)
        my_payload_xor = my_xor.Xor(my_payload_salted, clef)
        my_payload_64 = base64.standard_b64encode(
            base64._bytes_from_decode_data(my_payload_xor))
        # my_payload_encrypted = encryptor.update(my_payload_64) + encryptor.finalize()

        comp = (comp + 1) % (10**4)
        (rc, mid) = client.publish(topic=my_topic,
                                   payload=my_payload_64, qos=0)
        print("> Error return from publish of mid = " +
              str(mid) + " : " + mqtt.error_string(rc))
        time.sleep(1)

except KeyboardInterrupt:
    client.loop_stop()
    client.unsubscribe(my_topic)
    client.disconnect()
    print("Done.")
