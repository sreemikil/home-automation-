#include <ESP8266WiFi.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define D1 5 // GPIO5
#define D2 4 // GPIO5



#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "Jokerjack"
#define AIO_KEY  "4de7f7e5ccc047a5a65bff64d1a944c9"
WiFiClient client;

WiFiServer server(80);


String header;


String output5State = "off";
String output4State = "off";


const int output5 = 5;
const int output4 = 4;


unsigned long currentTime = millis();

unsigned long previousTime = 0; 

const long timeoutTime = 2000;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe myfeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/myfeed");
String  Request;
boolean MQTT_connect();

boolean MQTT_connect() {  int8_t ret; if (mqtt.connected()) {    return true; }  uint8_t retries = 3;  while ((ret = mqtt.connect()) != 0) { mqtt.disconnect(); delay(2000);  retries--;if (retries == 0) { return false; }} return true;}
void setup()
{
  Serial.begin(9600);
Request = "";

  
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
   WiFi.begin("mik!L","qwertyuiopz");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");

  }
  Serial.println("Connected");


  mqtt.subscribe(&myfeed);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  server.begin();

}


void loop()
{
 
 WiFiClient client = server.available();   

  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();         
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                   
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
           
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(D1, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(D1, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(D2, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(D2, LOW);
            }
            
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: white; border: 2px solid #4CAF50; color: black; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: white; border: 2px solid #f44336; color: black;}</style></head>");
            
          
            client.println("<body><h1>Board Controller</h1>");
            
              
            client.println("<p>RED bulb is " + output5State + "</p>");
                 
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
             
            client.println("<p>GREEN bulb is " + output4State + "</p>");
             
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            
            client.println();
          
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    
    header = "";
    
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

    if (MQTT_connect()) {
      Adafruit_MQTT_Subscribe *subscription_name;
      while ((subscription_name = mqtt.readSubscription(5000))) {
        if (subscription_name == &myfeed) {
          Request = ((char *)myfeed.lastread);
                  if (Request == "d1on") {
          digitalWrite(D1, HIGH);
          Serial.println("Red light turn ON");
        }
        if (Request == "d1off") {
          digitalWrite(D1, LOW);
          Serial.println("Red light turn OFF");
        }

        if (Request == "d2on") {
          digitalWrite(D2, HIGH);
          Serial.println("Green light turn ON");
        }
        if (Request == "d2off") {
          digitalWrite(D2, LOW);
          Serial.println("Green light turn OFF");
        }
        }

      }

    }

}
