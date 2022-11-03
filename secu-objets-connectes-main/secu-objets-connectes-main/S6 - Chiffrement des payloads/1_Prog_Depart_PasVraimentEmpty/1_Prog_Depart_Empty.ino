#include <Countdown.h>
#include <IPStack.h>
#include <MQTTClient.h>

#include <WiFiEsp.h>

#include <Base64.h>

#include <AES.h>

#define MY_DEBUG

#define WIFI_SSID (char*)"ZZ_HSH"
#define WIFI_PWD "WIFI_ZZ_F5"

#define MQTT_SERVER (char*)"192.168.1.136"
#define MQTT_PORT 1883
#define MQTT_CLIENTID (char*)"GROUPE_03"

const int MAX_MQTT_PACKET_SIZE = 500;

// Key for AES Cipher
AES aes;
const int key_length = 128;
byte key[key_length >> 3];

int nb_block_data = 1;

byte iv[16];
byte iv2 [N_BLOCK];

// Variables => input and output pins
const int led_D5    = 13;
const int led_D6    = 12;
const int bouton_S2 = 8;
const int bouton_S3 = 9;

// Topic Names
String subLED  = "/ISIMA/S6_AES/GROUPE_03/LEDS/#";
String subBP   = "/ISIMA/S6_AES/GROUPE_03/BPS/#";
String pubBP1  = "/ISIMA/S6_AES/GROUPE_03/BPS/BP1";
String pubBP2  = "/ISIMA/S6_AES/GROUPE_03/BPS/BP2";
String pubLED1 = "/ISIMA/S6_AES/GROUPE_03/LEDS/LED1";
String pubLED2 = "/ISIMA/S6_AES/GROUPE_03/LEDS/LED2";

// Parameters of payload messages : text and salting paramters
const int nbsalt  = 20;
String saltchar   = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
int nbchar        = saltchar.length();
String payloadON  = "ON";
String payloadOFF = "OFF";

byte* plain = new byte[nb_block_data * N_BLOCK];
byte* cipher = new byte[nb_block_data * N_BLOCK];
byte* check = new byte[nb_block_data * N_BLOCK];


// Global parameters for events on device
bool ChangeLed_D5 = false, ChangeLed_D6 = false;
bool Bp_S2 = true, Bp_S3 = true;

// IP Stack and MQTT Client for communicate with broker
WiFiEspClient c;
IPStack ipstack(c);
MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE > client = MQTT::Client<IPStack, Countdown, MAX_MQTT_PACKET_SIZE >(ipstack);

// Convert String to char*
char* StringToChar(String str)
{
  char* char_array = new char[str.length() + 1];
  str.toCharArray(char_array, str.length() + 1);
  return char_array;
}

// Debug function => print value in HEX format
void Print_Debug_HEX(String title, byte* data, int data_length)
{
#ifdef MY_DEBUG
  Serial.print(title) ;
  for (int i = 0; i < data_length; ++i)
  {
    byte val = data[i];
    Serial.print (val >> 4, HEX) ; Serial.print (val & 15, HEX) ; Serial.print (" ") ;
  }
  Serial.println() ;
#endif
}

// Initialisation for Serial connexions (USB connexion between Arduino and PC => serial monitor of IDE)
void SerialConnect()
{
#ifdef MY_DEBUG
  Serial.begin(9600);
  while (!Serial);
  Serial.println("--- Serial Initialisation (USB): OK ---");
#endif
}

// Initialisation for Wifi connexion ( Serial1 )
void WifiConnect()
{
  Serial1.begin(9600);
  while (!Serial1);

#ifdef MY_DEBUG
  Serial.println("--- Serial1 Initialisation (ESP8266): OK ---");
#endif

  WiFi.init(&Serial1);
  WiFi.begin(WIFI_SSID, WIFI_PWD);

#ifdef MY_DEBUG
  Serial.println("--- Wifi Connexion on gateway: OK ---");
#endif
}

// Connexion to MQTT Broker
void BrokerConnect()
{
  ipstack.connect(MQTT_SERVER, MQTT_PORT);

  MQTTPacket_connectData configMQTT = MQTTPacket_connectData_initializer;
  configMQTT.clientID.cstring = MQTT_CLIENTID;

  int rc = client.connect(configMQTT);

#ifdef MY_DEBUG
  if (rc == 0)
    Serial.println("--- MQTT Broker Connexion: OK ---");
  else
  {
    Serial.print("--- MQTT Broker Connexion: KO ---");
    Serial.print("Error connexion: "); Serial.println(rc);
  }
#endif

  client.subscribe(subBP.c_str(), MQTT::QOS0, CallBackBpMsg);
  client.subscribe(subLED.c_str(), MQTT::QOS0, CallBackLedMsg);
}

// Arduino ports setup
void PortsSetup()
{
  // initialisation des broches 12 et 13 comme étant des sorties
  pinMode(led_D5, OUTPUT);
  pinMode(led_D6, OUTPUT);
  // initialisation des broches 8 et 9 comme étant des entrées
  pinMode(bouton_S2, INPUT);
  pinMode(bouton_S3, INPUT);

#ifdef MY_DEBUG
  Serial.println("--- GPIO Initilisation: OK ---");
#endif
}

// Callback for event on buttons
void CallBackBpMsg(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  MQTTString &topic      = md.topicName;

#ifdef MY_DEBUG
  Serial.println("--- MQTT Callback for button events ---");
  Serial.print(  "Quality of Service ( QoS ): "); Serial.println(message.qos);
  Serial.print(  "Retained message          : "); Serial.println(message.retained);
  Serial.print(  "Duplicate                 : "); Serial.println(message.dup);
  Serial.print(  "Message Identifier        : "); Serial.println(message.id);
#endif

  char* topicName = new char[topic.lenstring.len + 1]();
  memcpy(topicName, topic.lenstring.data, topic.lenstring.len);

#ifdef MY_DEBUG
  Serial.print(  "Topic name                : "); Serial.println(topicName);
#endif

  char* msgPL = new char[message.payloadlen + 1]();
  memcpy(msgPL, message.payload, message.payloadlen);
#ifdef MY_DEBUG
  Serial.print(  "Cipher Payload            : "); Serial.println(msgPL);
#endif

  String dmsgPL  = AESDecrypt(String(msgPL));
  char* dmsgPL_c = StringToChar(dmsgPL);
#ifdef MY_DEBUG
  Serial.print(  "Decipher Payload          : "); Serial.println(dmsgPL);
#endif

  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 3], "BP1", 3))
  {
    if (!strncmp(dmsgPL_c, "ON", 2))
      ChangeLed_D6 = true;
  }
  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 3], "BP2", 3))
  {
    if (!strncmp(dmsgPL_c, "ON", 2))
      ChangeLed_D5 = true;
  }
  delete[] dmsgPL_c;
  delete[] msgPL;
  delete[] topicName;
}

// Callback for event on LEDs
void CallBackLedMsg(MQTT::MessageData& md)
{
  MQTT::Message &message  = md.message;
  MQTTString &topic = md.topicName;

#ifdef MY_DEBUG
  Serial.println("--- MQTT Callback for LED events ---");
  Serial.print(  "Quality of Service ( QoS ): "); Serial.println(message.qos);
  Serial.print(  "Retained message          : "); Serial.println(message.retained);
  Serial.print(  "Duplicate                 : "); Serial.println(message.dup);
  Serial.print(  "Message Identifier        : "); Serial.println(message.id);
#endif

  char* topicName = new char[topic.lenstring.len + 1]();
  memcpy(topicName, topic.lenstring.data, topic.lenstring.len);
#ifdef MY_DEBUG
  Serial.print(  "Topic name                : "); Serial.println(topicName);
#endif

  char* msgPL = new char[message.payloadlen + 1]();
  memcpy(msgPL, message.payload, message.payloadlen);
#ifdef MY_DEBUG
  Serial.print(  "Cipher Payload            : "); Serial.println(msgPL);
#endif

  String dmsgPL  = AESDecrypt(String(msgPL));
  char* dmsgPL_c = StringToChar(dmsgPL);
#ifdef MY_DEBUG
  Serial.print(  "Decipher Payload          : "); Serial.println(dmsgPL);
#endif

  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 4], "LED1", 4))
    if (!strncmp(dmsgPL_c, "ON", 2))
      digitalWrite(led_D6, HIGH);
    else
      digitalWrite(led_D6, LOW);

  if (!strncmp(&topic.lenstring.data[topic.lenstring.len - 4], "LED2", 4))
    if (!strncmp(dmsgPL_c, "ON", 2))
      digitalWrite(led_D5, HIGH);
    else
      digitalWrite(led_D5, LOW);

  delete[] dmsgPL_c;
  delete[] msgPL;
  delete[] topicName;
}

// Salted the plain payload
String AESSalted(String payLoad)
{
  String saltedPl = payLoad;
  for (int i = 0; i < nbsalt; i++)
    saltedPl  = saltedPl  + saltchar[random(0, nbchar)];
  return saltedPl;
}

// AES cipher setup : set the symmetric key by a random generation
void AESSetup()
{
  for (int i = 0; i < 16; ++i) {
    key[i] = random(0, 256);
  }
}

// AES-CBC Cipher
String AESEncrypt(String plainStr)
{
  // Payload Salted
  String saltedPlain = AESSalted(plainStr);

  // base64
  /*int encodedLen=base64_enc_len(saltedPlain.length());
    char encoded[encodedLen];
    base64_encode(encoded,StringToChar(saltedPlain),saltedPlain.length());*/
  // AES
  byte succ = aes.set_key (key, key_length) ;
  Serial.print("Affectation de la clef : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");

  int plain_length = saltedPlain.length();
  nb_block_data = (plain_length + 1) / N_BLOCK + 1;


  saltedPlain.getBytes(plain, saltedPlain.length() + 1);


  for (int i = 0; i < 16; ++i) {
    iv[i] = random(0, 256);
  }

  // Chiffrement des données
  
  for (byte i = 0 ; i < N_BLOCK ; i++) iv2[i] = iv[i] ;

  succ = aes.cbc_encrypt (plain, cipher, nb_block_data, iv) ;
  Serial.print("Chiffrement AES        : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");



#ifdef MY_DEBUG
  Serial.println( "   ******** Salt value of message ********");
  Serial.print(   "   Original message                     : "); Serial.println(plainStr);
  Serial.print(   "   Size of salted information (string)  : "); Serial.println(nbsalt);
  Serial.print(   "   Salted message                       : "); Serial.println(saltedPlain);
  Serial.println( "");
#endif



  return String(cipher);
}

// AES-CBC Decipher
String AESDecrypt(String cipherStr)
{
  // base64
  /*int msglen=cipherStr.length();
    int decodedLen=base64_dec_len(StringToChar(cipherStr),msglen);
    char decoded[decodedLen];
    base64_decode(decoded,StringToChar(cipherStr),msglen);*/

  // Dechiffrement des données
  for (byte i = 0 ; i < 16 ; i++)
    iv2[i] = iv[i] ;
  succ = aes.cbc_decrypt(cipher, check, nb_block_data, iv) ;
  Serial.print("Dechiffrement AES      : "); (succ == SUCCESS) ? Serial.println("OK") : Serial.println("KO");
  //Serial.print ("Le message : '") ; Serial.print (check) ; Serial.print ("' possède ") ; Serial.print (plain_length) ; Serial.print (" octets, il faudra donc ") ; Serial.print (nb_block_data) ; Serial.println(" blocs de 16 octets.") ;
  String cipher_data = (char *)(cipher);
  Serial.println("");

  String non_sardoche = String(check);

  non_sardoche.remove(non_sardoche.length() - nbsalt);

  return non_sardoche;
}

void setup()
{
  SerialConnect();
  WifiConnect();
  BrokerConnect();
  PortsSetup();
  AESSetup();
}

void loop()
{
  client.yield(100);

  if (Bp_S2 != digitalRead(bouton_S2))
  {
    Bp_S2 = !Bp_S2;

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    char* payload = Bp_S2 ? StringToChar(AESEncrypt("OFF")) : StringToChar(AESEncrypt("ON"));
    message.payload = (void *)(payload);
    message.payloadlen = strlen(payload);
    int err = client.publish(pubBP1.c_str(), message);
#ifdef MY_DEBUG
    Serial.println("--- MQTT Publish Message ---");
    Serial.print(  "Return code : "); Serial.print(err); Serial.println(" (BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0)");
    Serial.println( "");
#endif
    delete[] payload;
  }
  if (Bp_S3 != digitalRead(bouton_S3))
  {
    Bp_S3 = !Bp_S3;

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    char* payload = Bp_S3 ? StringToChar(AESEncrypt("OFF")) : StringToChar(AESEncrypt("ON"));
    message.payload = (void *)(payload);
    message.payloadlen = strlen(payload);
    int err = client.publish(pubBP2.c_str(), message);
#ifdef MY_DEBUG
    Serial.println("--- MQTT Publish Message ---");
    Serial.print(  "Return code : "); Serial.print(err); Serial.println(" (BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0)");
    Serial.println( "");
#endif
    delete[] payload;
  }

  if (ChangeLed_D6)
  {
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;

    char* payload = !digitalRead(led_D6) ? StringToChar(AESEncrypt("ON")) : StringToChar(AESEncrypt("OFF"));
    message.payload = (void *)(payload);
    message.payloadlen = strlen(payload);
    int err = client.publish(pubLED1.c_str(), message);
#ifdef MY_DEBUG
    Serial.println("--- MQTT Publish Message ---");
    Serial.print(  "Return code : "); Serial.print(err); Serial.println(" (BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0)");
    Serial.println( "");
#endif
    delete[] payload;

    ChangeLed_D6 = false;
  }
  if (ChangeLed_D5)
  {
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;

    char* payload = !digitalRead(led_D5) ? StringToChar(AESEncrypt("ON")) : StringToChar(AESEncrypt("OFF"));
    message.payload = (void *)(payload);
    message.payloadlen = strlen(payload);
    int err = client.publish(pubLED2.c_str(), message);
#ifdef MY_DEBUG
    Serial.println("--- MQTT Publish Message ---");
    Serial.print(  "Return code : "); Serial.print(err); Serial.println(" (BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0)");
    Serial.println( "");
#endif
    delete[] payload;

    ChangeLed_D5 = false;
  }
}
