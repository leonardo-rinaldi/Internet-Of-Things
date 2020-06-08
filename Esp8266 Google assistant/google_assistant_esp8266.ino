/*
 * Questo sketch una volta creato un account su Adafruit.io e configurato gli applet su IFTT permette
 * di comandare la scheda esp8266 tramite Google assistant (Google Home) o in alternativa con un
 * Amazon Alexa.
 * E' possibile aggiungere degli output modificando la sezione del codice relativa alle funzioni.
 * 
 * Prima di iniziare è necessario configurare nel codice l'SSID e la password della rete.
 * Nel campo AIO_USERNAME e AIO_KEY devi inserire i tuoi dati reperibili su Adafruit.io
 * 
 */

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define DOOR_PIN D0
#define DOOR_PIN_1 D1      //Pin a cui collegheremo il led
#define DOOR_PIN_2 D2      //Pin a cui collegheremo il led
#define WLAN_SSID       "" //Il nome della tua connessione wifi
#define WLAN_PASS       "" //La password della tua connessione wifi
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883 //per il protocollo SSL usare la porta 8883
#define AIO_USERNAME    "" //Username creato su Adafruit
#define AIO_KEY         "" //IO_key creata su Adafruit


WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe openDoorFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ProvaIOT");


void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(DOOR_PIN, OUTPUT);
  pinMode(DOOR_PIN_1, OUTPUT);
  pinMode(DOOR_PIN_2, OUTPUT);
  digitalWrite(DOOR_PIN, LOW);
  digitalWrite(DOOR_PIN_1, LOW);
  digitalWrite(DOOR_PIN_2, LOW);

  Serial.println(); Serial.println();
  Serial.print("Connessione alla rete Wi-FI ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }
  Serial.println();

  Serial.println("Wi-FI Connesso!");
  Serial.println("IP del modulo ESP8266 : "); 
  Serial.println(WiFi.localIP());


  mqtt.subscribe(&openDoorFeed);
  
}

uint32_t x=0;

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    
    if (subscription == &openDoorFeed) {
      
      Serial.print(F("Dati ricevuti : "));
      Serial.println((char *)openDoorFeed.lastread);
      digitalWrite(DOOR_PIN, (int)openDoorFeed.lastread);
      
      
/*
 * Qui puoi aggiungere degli output 
 * 
 */
   

      if(!strcmp((char *)openDoorFeed.lastread, "primoOn")) accendi_il_primo_led();
      if(!strcmp((char *)openDoorFeed.lastread, "primoOff")) spegni_il_primo_led();
      
      if(!strcmp((char *)openDoorFeed.lastread, "secondoOn")) accendi_il_secondo_led();
      if(!strcmp((char *)openDoorFeed.lastread, "secondoOff")) spegni_il_secondo_led(); 

      
    }
    
  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

/*
 * Funzioni per accendere o spegnere un led
 */

void accendi_il_primo_led() {
  digitalWrite(DOOR_PIN_1, HIGH); 
}

void spegni_il_primo_led() {
  
  digitalWrite(DOOR_PIN_1, LOW);
}

void accendi_il_secondo_led() {
  digitalWrite(DOOR_PIN_2, HIGH);
}

void spegni_il_secondo_led() {
  digitalWrite(DOOR_PIN_2, LOW);
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connessione al server MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Tentativo di connessione tra 5 secondi...");
       mqtt.disconnect();
       delay(5000);  
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT connesso!");
}
