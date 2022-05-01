

#include <SHT1x.h>
#include "defines.h"
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFiNINA_Generic.h>
#include <PubSubClient.h>
#include <avr/dtostrf.h>

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;


static char outstr[15];
static char outstr2[15];


int status = WL_IDLE_STATUS;
WiFiClient client;

#define dataPin 3
#define clockPin 2
SHT1x sht1x(dataPin, clockPin);

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);


float temp_c;
float humidity;


void setup()
{

  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("Falla de conexion wifi"));
    while (true);
  }

  while (status != WL_CONNECTED)
  {
    Serial.print(F("Intentando conectarse al SSID: "));
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println(F("Conectado a wifi"));
  printWiFiStatus();
  mqtt.setServer(mqtt_server, 1883);

}


void reconnect() {

  while (!mqtt.connected()) {
    if (mqtt.connect(DEVICEID, TOKEN, NULL)) {
      Serial.println("Conectado a  MQTT Broker");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      Serial.print("Fallo, rc=");
      Serial.print(mqtt.state());
      Serial.println("Inténtalo de nuevo en 5 segundos");
      digitalWrite(LED_BUILTIN, LOW);
      delay(5000);

    }
  }
}



void loop()
{

  lecturaSensores();
  mqtt.loop();

  dtostrf(temp_c, 6, 1, outstr);
  dtostrf(humidity, 6, 0, outstr2);

  mqtt.publish(publishTopic, outstr);
  mqtt.publish(publishTopic2, outstr2);
  delay(5000);

  if (!mqtt.connected())
  {
    reconnect();
  }
}

void lecturaSensores() {

  temp_c = sht1x.readTemperatureC();
  humidity = sht1x.readHumidity();

  //  Serial.print("Temperature: ");
  //  Serial.print(temp_c);
  //  Serial.print("C / ");
  //  Serial.print("Humidity: ");
  //  Serial.print(humidity);
  //  Serial.println("%");
  //  delay(100);

}

void printWiFiStatus()
{

  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print(F("Dirección IP : "));
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print(F("Intensidad de señal(RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}
