#include <SPI.h>
#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>
#include <WiFiEsp.h>

// Variables => pins des input et output
const int led_D5 = 13;
const int led_D6 = 12;
const int bouton_S2 = 8;
const int bouton_S3 = 9;
bool ChangeLed_D5=false, ChangeLed_D6=false;
bool Bp_S2 =true, Bp_S3 =true;

WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown> client = MQTT::Client<IPStack, Countdown>(ipstack);

// Noms des topics
String subLED  = "isima/SECRET_1793/G3/LEDs/#";
String subBP   = "isima/SECRET_1793/G3/BPs/#";
String pubBP1  = "isima/SECRET_1793/G3/BPs/BP1";
String pubBP2  = "isima/SECRET_1793/G3/BPs/BP2";
String pubLED1 = "isima/SECRET_1793/G3/LEDs/LED1";
String pubLED2 = "isima/SECRET_1793/G3/LEDs/LED2";

void CallBackBpMsg(MQTT::MessageData& md){
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
  
  char* topicName = new char[topic.lenstring.len+1]();
  memcpy(topicName,topic.lenstring.data,topic.lenstring.len);
  Serial.print(", topic ");
  Serial.println(topicName);
  
  char* msgPL = new char[message.payloadlen+1]();
  memcpy(msgPL,message.payload,message.payloadlen);
  Serial.print("Payload ");
  Serial.println(msgPL);
  
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP1",3)){
    if(!strncmp(msgPL,"ON",2)){
      ChangeLed_D6=true;
    }
  }
  if(!strncmp(&topic.lenstring.data[topic.lenstring.len-3],"BP2",3)){
    if(!strncmp(msgPL,"ON",2)){
      ChangeLed_D5=true;
    }
  }
  delete msgPL;
  delete topicName;
}

void CallBackLedMsg(MQTT::MessageData& md){
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
  char* topicName = new char[topic.lenstring.len+1]();
  memcpy(topicName,topic.lenstring.data,topic.lenstring.len);
  Serial.print(", topic ");
  Serial.println(topicName);
    char* msgPL = new char[message.payloadlen+1]();
  memcpy(msgPL,message.payload,message.payloadlen);
  Serial.print("Payload ");
  Serial.println(msgPL);
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED1",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(led_D6,HIGH);
    else
      digitalWrite(led_D6,LOW);
  }      
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len-4],"LED2",4)){
    if (!strncmp(msgPL,"ON",2))
      digitalWrite(led_D5,HIGH);
    else
      digitalWrite(led_D5,LOW);
  }
  delete msgPL;
  delete topicName;
}

void WifiConnect(){
  Serial1.begin(9600);
  while(!Serial1);
    Serial.begin(9600);
  while(!Serial);
  WiFi.init(&Serial1);
  WiFi.begin((char*)"ZZ_HSH","WIFI_ZZ_F5");
  Serial.println(WiFi.localIP());
}

void BrokerConnect(){
  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;
  
  configMQTT.clientID.cstring = (char*)"GROUPE_3";
  configMQTT.username.cstring = (char*)"G3";
  configMQTT.password.cstring = (char*)"isimaG3";
  configMQTT.willFlag = 0;
  ipstack.connect((char*) "192.168.1.136", 5203);
  int rc = client.connect(configMQTT);
  if(rc == 0)
    Serial.println("Connected OK");
  else
    Serial.println("Not Connected ERROR");
  client.subscribe(subBP.c_str(), MQTT::QOS0, CallBackBpMsg);
  client.subscribe(subLED.c_str(),MQTT::QOS0, CallBackLedMsg);
}
void PortsSetup(){
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
  client.yield(100);
  if (Bp_S2 != digitalRead(bouton_S2)){
    Bp_S2 = !Bp_S2;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S2?"OFF":"ON");
    message.payloadlen = strlen(Bp_S2?"OFF":"ON");
    client.publish(pubBP1.c_str(),message);
  }
  if (Bp_S3 != digitalRead(bouton_S3)){
    Bp_S3 = !Bp_S3;
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.payload = (void *)(Bp_S3?"OFF":"ON");
    message.payloadlen = strlen(Bp_S3?"OFF":"ON");
    client.publish(pubBP2.c_str(),message);
  }
  if(ChangeLed_D6){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D6)?"ON":"OFF");
    message.payloadlen = strlen(!digitalRead(led_D6)?"ON":"OFF");
    client.publish(pubLED1.c_str(),message);
    ChangeLed_D6=false;
  }
  if(ChangeLed_D5){
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = true;
    message.payload = (void *)(!digitalRead(led_D5)?"ON":"OFF");
    message.payloadlen = strlen(!digitalRead(led_D5)?"ON":"OFF");
    client.publish(pubLED2.c_str(),message);
    ChangeLed_D5=false;
  }
}
