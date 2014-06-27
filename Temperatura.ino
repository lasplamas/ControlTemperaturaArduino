
/*
  * Control de temperatura
  * @author Ramiro de Jesus Gutierrez Alaniz
  * @date 
  * @materia Proyecto Domotica
  * @profesor Jorge de la Torre
 */
#include <Wire.h>
#include <BH1750FVI.h>
#include <SPI.h>
#include <WiFi.h>
#include <DHT.h>

char ssid[] = "itesm";

#define DHTPIN 2
#define DHTPIN_2 3

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN_2, DHTTYPE);

BH1750FVI LightSensor;

String temperatura;
String humedad;
String lux;
String petition;
String ventiladores;
int toggle = 0;

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;
WiFiClient client_register;
String ip;

// server address:
char server[] = "10.32.197.160";

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

unsigned long lastConnectionTime2 = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected2 = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval2 = 5*60000;
const int salida_vents = 8;

int estado_bot = 0;

void setup() {
  
  Serial.begin(9600);
  
  pinMode( salida_vents, OUTPUT );
  
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_H);
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  
  ventiladores = String("");
  dht.begin();
  
  while (!Serial) { }
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    while(true);
  } 
  
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
    delay(10000);
  } 
  
  printWifiStatus();
}

/***
* loop
* Method that does the thread
*****************************/
void loop() {
  
  
  
  delay(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  
  float tprom = ( t+t2 )/2;
  float hprom = ( h+h2 )/2;
  
  uint16_t lum = LightSensor.GetLightIntensity();
  Serial.println(lum);
  
  
  char chart[10];
  char charh[10];
  char charl[10];
  
  dtostrf(tprom, 4, 2, chart);
  dtostrf(hprom, 4, 2, charh);
  dtostrf(lum, 4, 2, charl);
  
  temperatura = String(chart);
  humedad = String(charh);
  lux = String(charl);
  
  Serial.println(temperatura);
  Serial.println(humedad);
  Serial.println(lux);
  
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
    ventiladores += c;
  }
  
  if( ventiladores == "On"){
    Serial.println( "Prendido bitch!!!" );
    digitalWrite( salida_vents, HIGH );
  }else if( ventiladores == "Off" ){
    Serial.println( "Apagado bitch!!!" );
    digitalWrite( salida_vents, LOW );
  }
  
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    ventiladores = String("");
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }
  
  lastConnected = client.connected();
  
/*************************************************************************************************/
  while (client_register.available()) {
    char c = client_register.read();
    Serial.write(c);
  }

  if (!client_register.connected() && lastConnected2) {
    Serial.println();
    Serial.println("disconnecting.");
    client_register.stop();
  }

  if(!client_register.connected() && (millis() - lastConnectionTime2 > postingInterval2)) {
    httpRequest2();
  }
  
  lastConnected2 = client_register.connected();
  
}//End of the loop Method

/***
* httpRequest
* method for sending a new data to the server
* @params none
* @return none
******************/
void httpRequest() {
 
  if (client.connect(server, 80)) {
    
    Serial.println("connecting...");
    
    petition = "GET /Controllers/DataController.php?action=setAllDataValues&temperatura="+temperatura+"&humedad="+humedad+"&lum="+lux+"&ip="+ip;
    
    client.println(petition);
    client.println("Host: 10.32.197.160");
    client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
  
  } 
  else {
    
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    ventiladores = "";
  
  }
}//End of httpRequest

/***
* httpRequest2
* method for sending a new data register to the server
* @params none
* @return none
******************/
void httpRequest2() {
  
  if ( client_register.connect( server, 80 ) ) {
    
    Serial.println("connecting...");
    
    petition = "GET /Controllers/RegisterController.php?action=setAllRegister&temperatura="+temperatura+"&humedad="+humedad+"&lum="+lux;
    client_register.println( petition );
    client_register.println( "Host: 10.32.197.160" );
    client_register.println( "Connection: close" );
    client_register.println();

    lastConnectionTime2 = millis();
    
  } else {
    
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client_register.stop();
    
  }
  
}//End of httpRequest2 method


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip_local;
  // print your WiFi shield's IP address:
  ip_local = WiFi.localIP();
  
  ip += ip_local[0];
  ip += ".";
  ip += ip_local[1];
  ip += ".";
  ip += ip_local[2];
  ip += ".";
  ip += ip_local[3];
  
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip_local);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}






