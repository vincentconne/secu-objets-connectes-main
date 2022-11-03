#include <SPI.h>
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>
#include <WiFiEsp.h>
#include <Base64.h>
//#include "main_wip.cpp"
char * Xor(char * in, int inSize, char * key, int keySize, char * out);
#define T 50

// Variables => pins des input et output
const int led_D5 = 13;
const int led_D6 = 12;
const int bouton_S2 = 8;
const int bouton_S3 = 9;
bool ChangeLed_D5 = false, ChangeLed_D6 = false;
bool Bp_S2 = true, Bp_S3 = true;


WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

// Noms des topics
/*
  String subLED  = "isima/TP_5/GROUPE_03/LEDs/#";
  String subBP   = "isima/TP_5/GROUPE_03/BPs/#";
  String pubBP1  = "isima/TP_5/GROUPE_03/BPs/BP1";
  String pubBP2  = "isima/TP_5/GROUPE_03/BPs/BP2";
  String pubLED1 = "isima/TP_5/GROUPE_03/LEDs/LED1";
  String pubLED2 = "isima/TP_5/GROUPE_03/LEDs/LED2";
*/

// XOR

char tmp[T];
char subLED[28] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x03\x11\x16\x3c\x63\x6c";
char subBP[27] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x0d\x04\x21\x60\x6f";
char pubBP1[29] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x0d\x04\x21\x60\x0e\x1f\x65";
char pubBP2[29] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x0d\x04\x21\x60\x0e\x1f\x66";
char pubLED1[31] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x03\x11\x16\x3c\x63\x03\x11\x16\x7e";
char pubLED2[31] = "\x25\x3c\x3d\x3f\x2e\x63\x1b\x04\x0d\x7a\x63\x08\x06\x1d\x1a\x1c\x0a\x0b\x62\x7c\x63\x03\x11\x16\x3c\x63\x03\x11\x16\x7d";

char key[6] = "LOTRO";
int keySize = 5;


void CallBackBpMsg(MQTT::MessageData& md) {
  MQTT::Message &message = md.message;
  MQTTString &topic = md.topicName;

  Serial.print("Message BP arrived: qos ");
  Serial.print(message.qos);
  Serial.print(", retained ");
  Serial.print(message.retained);
  Serial.print(", dup ");
  Serial.print(message.dup);
  Serial.print(", packetid ");
  Serial.println(message.id);

  char* topicName = new char[topic.lenstring.len + 1]();
  memcpy(topicName, topic.lenstring.data, topic.lenstring.len);
  Serial.print(", topic ");
  Serial.println(topicName);

  char* msgPL = new char[message.payloadlen + 1]();
  memcpy(msgPL, message.payload, message.payloadlen);
  Serial.print("Payload ");
  Serial.println(msgPL);

  // Base64
  int msgLen = sizeof(msgPL);
  int decodedLen = base64_dec_len(msgPL, msgLen);
  char decoded[decodedLen];
  base64_decode(decoded, msgPL, msgLen);

  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 3], "BP1", 3)) {
    if (!strncmp(decoded, "ON", 2)) {
      ChangeLed_D6 = true;
    }
  }
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 3], "BP2", 3)) {
    if (!strncmp(decoded, "ON", 2)) {
      ChangeLed_D5 = true;
    }
  }
  delete msgPL;
  delete topicName;
}

void CallBackLedMsg(MQTT::MessageData& md) {
  MQTT::Message &message  = md.message;
  MQTTString &topic = md.topicName;

  Serial.print("Message LED arrived: qos ");
  Serial.print(message.qos);
  Serial.print(", retained ");
  Serial.print(message.retained);
  Serial.print(", dup ");
  Serial.print(message.dup);
  Serial.print(", packetid ");
  Serial.println(message.id);
  char* topicName = new char[topic.lenstring.len + 1]();
  memcpy(topicName, topic.lenstring.data, topic.lenstring.len);
  Serial.print(", topic ");
  Serial.println(topicName);
  char* msgPL = new char[message.payloadlen + 1]();
  memcpy(msgPL, message.payload, message.payloadlen);
  Serial.print("Payload ");
  Serial.println(msgPL);

   // Base64
  int msgLen = sizeof(msgPL);
  int decodedLen = base64_dec_len(msgPL, msgLen);
  char decoded[decodedLen];
  base64_decode(decoded, msgPL, msgLen);
  
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 4], "LED1", 4)) {
    if (!strncmp(decoded, "ON", 2))
      digitalWrite(led_D6, HIGH);
    else
      digitalWrite(led_D6, LOW);
  }
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 4], "LED2", 4)) {
    if (!strncmp(decoded, "ON", 2))
      digitalWrite(led_D5, HIGH);
    else
      digitalWrite(led_D5, LOW);
  }
  delete msgPL;
  delete topicName;
}

void WifiConnect() {
  Serial1.begin(9600);
  while (!Serial1);
  Serial.begin(9600);
  while (!Serial);
  WiFi.init(&Serial1);
  WiFi.begin((char*)"ZZ_HSH", "WIFI_ZZ_F5");
  Serial.println(WiFi.localIP());
}

void BrokerConnect() {
  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;

  configMQTT.clientID.cstring = (char*)"GROUPE_03";
  configMQTT.username.cstring = (char*)"";
  configMQTT.password.cstring = (char*)"";
  configMQTT.willFlag = 0;
  ipstack.connect((char *)"192.168.1.136", 1883);
  int rc = client.connect(configMQTT);
  if (rc == 0)
    Serial.println("Connected OK");
  else
    Serial.println("Not Connected ERROR");
  client.subscribe(Xor(subBP, 26, key, keySize, tmp), MQTT::QOS0, CallBackBpMsg);
  client.subscribe(Xor(subLED, 27, key, keySize, tmp), MQTT::QOS0, CallBackLedMsg);
}
void PortsSetup() {
  // initialisation des broches 12 et 13 comme étant des sorties
  pinMode(led_D5, OUTPUT);
  pinMode(led_D6, OUTPUT);
  // initialisation des broches 8 et 9 comme étant des entrées
  pinMode(bouton_S2, INPUT);
  pinMode(bouton_S3, INPUT);
}
void setup() {
  WifiConnect();
  BrokerConnect();
  PortsSetup();
}

void loop() {

  // Base 64
  char on[] = "ON";
  char off[] = "OFF";

  int encodedLenON = base64_enc_len(sizeof(on));
  char encodedON[encodedLenON];
  base64_encode(encodedON, on, sizeof(on));

  int encodedLenOFF = base64_enc_len(sizeof(off));
  char encodedOFF[encodedLenOFF];
  base64_encode(encodedOFF, off, sizeof(off));


  client.yield(100);
  if (Bp_S2 != digitalRead(bouton_S2)) {
    Bp_S2 = !Bp_S2;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S2 ? encodedOFF : encodedON);
    message.payloadlen = strlen(Bp_S2 ? encodedOFF : encodedON);
    client.publish(Xor(pubBP1, 28, key, keySize, tmp), message);
  }
  if (Bp_S3 != digitalRead(bouton_S3)) {
    Bp_S3 = !Bp_S3;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S3 ? encodedOFF : encodedON);
    message.payloadlen = strlen(Bp_S3 ? encodedOFF : encodedON);
    client.publish(Xor(pubBP2, 28, key, keySize, tmp), message);
  }
  if (ChangeLed_D6) {
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D6) ? encodedON : encodedOFF);
    message.payloadlen = strlen(!digitalRead(led_D6) ? encodedON : encodedOFF);
    client.publish(Xor(pubLED1, 30, key, keySize, tmp), message);
    ChangeLed_D6 = false;
  }
  if (ChangeLed_D5) {
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D5) ? encodedON : encodedOFF);
    message.payloadlen = strlen(!digitalRead(led_D5) ? encodedON : encodedOFF);
    client.publish(Xor(pubLED2, 31, key, keySize, tmp), message);
    ChangeLed_D5 = false;
  }
}
