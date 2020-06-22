/*  Copyright (C) 2020 Rodolfo Vasquez
 *
 *  Programa para modulo IoT de sensado de consumo electrico, temperatura y humedad
 *  basado en arquitectura nodemcu ESP8266 ESP32
 *  
 *  Desarrollado para el Call4Code en el 2020 por:
 *    Daniel Lema
 *    Ivan Vasquez
 *    Rodolfo Vasquez
 *    
 *  En el pin A0 analogo va conectado un SCT-013-000 con una resistencia de carga de 220 ohmios
 *  y circuito de divisor de voltaje con resistencias de 330 kOhmios
 *      
 *  En el pint D2 digital va conectado un sensor DHT11
 *    
 *  Organization ID         v45txk
 *  Device Type             NodeMCUESP8266
 *  Device ID               Hometer01
 *  Authentication Method   use-token-auth
 *  Authentication Token    ig@mnla)75JO!Bh-!2
 *  Board                   NodeMCU 1.0 ESP-12E Module
 *  Programmer              Arduino as ISP
 *  Port                    /dev/ttyUSB0
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3 of the License.
 *    
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *    
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *    
 */

#include <TimeLib.h>

#include <ESP8266WiFi.h>        // Libreria de WiFi
#include <WiFiUdp.h>            // Libreria de Puerto Udp para NTP
#include <ESP8266WebServer.h>   // Libreria del Servidor web
#include <ESP8266HTTPClient.h>  // Libreria Cliente HTTP
#include <DHTesp.h>             // Libreria del sensor de Temp y humedad
#include "EmonLib.h"            // Libreria de monitoreo de energia
#include <ArduinoJson.h>        // Libreria JSON
#include <PubSubClient.h>       // Libreria de Publicacion Suscripcion

#include "settings.h"           // Configuracion persistente
#include "mainPage.h"           // Pagina web de configuracion local
#include "watsonIot.h"          // Configuracion conectividad a Watson IoT

#define DebugStart() Serial.begin(115200)
#define DebugLn(s) Serial.println((s))
#define Debug(s) Serial.print((s))
#define SETUP_PIN 0

DHTesp dht;
EnergyMonitor emon1;                // Crea el monitor de consumo de energia

const float voltajeRed = 110.0;
const float calibracion = 1.6;
const long utcOffsetInSeconds = 3600;
const String webserver = "webserver.com"; // URL de IBM Cloud
const String weburi = "/data_store.php";  // Script para postear la data data_store.php
String data;

// Objetos MQTT
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// Variables para enviar mensaje JSON
StaticJsonDocument<100> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[100];

//void setupWiFi(char checkAPMode);
WiFiUDP Udp;
const unsigned int localPort = 8888;  // Puerto UDP local
time_t getNtpTime(void);

// Crea una instancia del webserver asociado al puerto 80
ESP8266WebServer server(80);

String httpUpdateResponse;

void displayAPWait(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void stopAnimation(int status){
  digitalWrite(LED_BUILTIN, status);
}

void displayBusy(){
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

// Variables de medicion
double Irms = 0.0;
double potencia = 0.0;
float humdty = 0;
float temp = 0;
float hic = 0;

// Procesamiento de request de pagina web
void handleRoot() {
  DebugLn("handleRoot");
  String s = FPSTR(MAIN_page);
  time_t t = now();
  s.replace("@@SSID@@", settings.ssid);
  s.replace("@@PSK@@", settings.psk);
  s.replace("@@TZ@@", String(settings.timezone));
  s.replace("@@USDST@@", settings.usdst?"checked":"");
  s.replace("@@HOUR@@", String(hour(t)));
  s.replace("@@MIN@@", String(minute(t)));
  s.replace("@@NTPSRV@@", settings.timeserver);
  s.replace("@@NTPINT@@", String(settings.interval));
  s.replace("@@SYNCSTATUS@@", timeStatus() == timeSet ? "OK" : "Overdue");
  s.replace("@@CLOCKNAME@@", settings.name);
  s.replace("@@IRMS@@", String(Irms));
  s.replace("@@POTEN@@", String(potencia));
  s.replace("@@HUMDTY@@", String(humdty));
  s.replace("@@TEMP@@", String(temp));
  s.replace("@@HIC@@", String(hic));
  s.replace("@@UPDATERESPONSE@@", httpUpdateResponse);
//  s.replace("@@MODE@@", getMode());
  httpUpdateResponse = "";
  server.send(200, "text/html", s);
}

// Procesamiento de medicion
void handleIO() {
  Irms = emon1.calcIrms(6062);
  potencia = Irms * voltajeRed;

  delay(dht.getMinimumSamplingPeriod());
  DebugLn(dht.getStatusString());

  // Leemos la humedad relativa
  humdty = dht.getHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  temp = dht.getTemperature();
  
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(humdty) || isnan(temp)) {
    Serial.println("Error obteniendo los datos del sensor DHT11 (" + String(humdty) + "," + String(temp) + ")");
    return;
  }
 
  // Calcular el indice de calor en grados centígrados
  hic = dht.computeHeatIndex(temp, humdty, false);

  time_t t = now();
  data = String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)) + "\thumidity=" + String(humdty) + "%\ttemperature=" + 
         String(temp) + "°C\thic=" + String(hic) + "°C\tpower=" + String(potencia) + "W\tIrms=" + Irms + "A";
  DebugLn(data);
  displayBusy();

  handleMqtt();
}

void setupMqtt()
{
  // Conexion a MQTT - IBM Watson IoT Platform
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    DebugLn("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);
  } else {
    DebugLn("MQTT Failed to connect!");
    // ESP.reset();
  }
}

void handleMqtt()
{
  if (!mqtt.connected()) {
    Debug("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      DebugLn("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.loop();
    } else {
      DebugLn("MQTT Failed to connect!");
      return;
    }
  }
      
  // Send data to Watson IoT Platform
  status["temp"] = temp;
  status["humidity"] = humdty;
  status["hic"] = hic;
  status["Irms"] = Irms;
  status["watts"] = potencia;
  serializeJson(jsonDoc, msg, 100);
  DebugLn(msg);
  if (!mqtt.publish(MQTT_TOPIC, msg)) {
    DebugLn("MQTT Publish failed");
  }
  
  // Pause - but keep polling MQTT for incoming messages
  for (int i = 0; i < 10; i++) {
    mqtt.loop();
    delay(1000);
  }
}

void postData()
{
  // Envia el post con la data a un webserver
  String httpPost = "http://"+webserver+weburi;
  HTTPClient http;
  DebugLn(httpPost);
  http.begin(httpPost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  DebugLn(data);
  int httpCode = http.POST(data);
  String payload = http.getString();

  DebugLn(httpCode);    // this return 200 when success
  //DebugLn(payload);     // this will get the response
  http.end();
}

// Procesamiento del formulario Web
void handleForm() {
  DebugLn("handleForm");
  DebugLn("mode "+String(WiFi.status()));
  String update_wifi = server.arg("update_wifi");
  String t_ssid = server.arg("ssid");
  String t_psk = server.arg("psk");
  String t_timeserver = server.arg("ntpsrv");
  t_timeserver.toCharArray(settings.timeserver, EEPROM_TIMESERVER_LENGTH, 0);
  if (update_wifi == "1") {
    settings.ssid = t_ssid;
    settings.psk = t_psk;
  }
  String tz = server.arg("timezone");

  if (tz.length()) {
    settings.timezone = tz.toInt();
  }

  String usdst = server.arg("usdst");
  settings.usdst = (usdst == "1");

  String syncInt = server.arg("ntpint");
  settings.interval = syncInt.toInt();

  settings.name = server.arg("clockname");
  settings.name.replace("+", " ");

  setSyncProvider(getNtpTime);
  setSyncInterval(settings.interval);

  httpUpdateResponse = "The configuration was updated.";

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Moved");

  settings.Save();
  
  if (update_wifi == "1") {
    delay(500);
    setupWiFi(0);             // connect to Wifi
  }
}

void setupNTP()
{
  // Obtencion de hora ntp para marca de tiempo
  Udp.begin(localPort);
  Debug("Local port: ");
  Debug(Udp.localPort());
  DebugLn(" waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(settings.interval);
}

// Inicializacion del board
void setup() {
  DebugStart();
  pinMode(LED_BUILTIN, OUTPUT);
  setupWiFi(1);               // Inicia Wifi en modo AP

  // Inicia el servidor Web
  server.on("/", handleRoot);
  server.on("/form", handleForm);
  server.on("/io", handleIO);
  server.begin();

  // Inicia el sensor DHT
  dht.setup(D2, DHTesp::DHT11);
  emon1.current(A0, calibracion);

  setupNTP();               // Inicia cliente NTP

  // setupMqtt();              // Inicia cliente Mqtt
  
  // Abre los contactos del rele
  pinMode(D5, OUTPUT);
  digitalWrite(D5, 0);
}

int timer = 1001;

// Procesamiento por cada ciclo
void loop() {
  static unsigned long lastSetupPin = 0;
  
  delay(100);
  if (timer > 1000){
    timer = 0;
    handleIO();
  }else{                  // Mientras espera recibe los requerimientos http
    timer++;
    server.handleClient();    
  }

}

// Configuracion del WiFi
void setupWiFi(char checkAPMode) {
  settings.Load();
  if (checkAPMode) {
    // Espera 5s para que SETUP_PIN vaya a low para entrar al modo AP/setup.
    pinMode(SETUP_PIN, INPUT);
    digitalWrite(SETUP_PIN, HIGH);
    displayAPWait();
    long start = millis();
    DebugLn("Started at "+String(start));
    while (millis() < start + 5000) {
      if (!digitalRead(SETUP_PIN) || !settings.ssid.length()) {
        DebugLn("Setting up AP");
        //stopAnimation();
        setupAP();
        DebugLn("Done with AP");
        return;
      }
      delay(50);
    }
    //stopAnimation();
  }
  setupSTA();
}

int timeOut=0;

// Configuracion del WiFi
void setupSTA()
{
  char ssid[32];
  char psk[64];

  memset(ssid, 0, 32);
  memset(psk, 0, 64);
  

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  settings.ssid.toCharArray(ssid, 32);
  settings.psk.toCharArray(psk, 64);
  if (settings.psk.length()) {
    WiFi.begin(ssid, psk);
  } else {
    WiFi.begin(ssid);
  }
  
  DebugLn("Connecting to "+String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    if (timeOut < 10){          // Si no hay timeout, re-intente
      delay(100);
      Debug(String(WiFi.status()));
      displayBusy();
      timeOut ++;
    } else{
      timeOut = 0;
      Debug("Timeout de conexion a WiFi");
      settings.ssid = "";     // Borra el ssid y el psk si hay timeout
      settings.psk = "";
      settings.Save();
      break;
    }
  }
  DebugLn("Conectado");
  // Print the IP address
  DebugLn(WiFi.localIP());  
}

// Inicializacion del WiFi
void setupAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(WIFI_AP_NAME);
}

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// Handler de procesamiento de señal NTP
time_t getNtpTime()
{
  IPAddress ntpServerIP; // Direccion del servidor NTP

  while (Udp.parsePacket() > 0) ; // Elimina paquetes previos
  DebugLn("Tx requerimiento NTP");
  WiFi.hostByName(settings.timeserver, ntpServerIP);
  Debug(settings.timeserver);
  Debug(": ");
  DebugLn(ntpServerIP);

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(ntpServerIP, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();

  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // lee el paquete en el buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + settings.timezone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // retorna 0 si no obtiene respuesta
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);
}
