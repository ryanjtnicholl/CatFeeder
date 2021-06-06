#include "WiFiEsp.h"
#include <ArduinoJson.h>
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"

// set up software serial to allow serial communication to our TX and RX pins
SoftwareSerial Serial1(6, 7);
#endif

// Set  baud rate of so we can monitor output from esp.
#define ESP8266_BAUD 9600

// CHANGE THIS TO MATCH YOUR SETTINGS
char ssid[] = "WIFI_SSID"; 
char pass[] = "WIFI_PASSWORD";
int status = WL_IDLE_STATUS;

// Define an esp server that will listen on port 80
WiFiEspServer server(80);


int motorAin1 = 2;
int motorAin2 = 3;
int instantfeedbtn = 13;
int timefeedbtn = 12;
int revfeedbtn = 11;
int scheduletog = 10;
int feedtime = 10000;

void setup() {
  pinMode(motorAin1, OUTPUT);
  pinMode(motorAin2, OUTPUT);
  pinMode(instantfeedbtn, INPUT);
  pinMode(timefeedbtn, INPUT);
  pinMode(revfeedbtn, INPUT);
  pinMode(scheduletog, INPUT);

  // Open up communications for arduino serial and esp serial at same rate
  Serial.begin(9600);
  Serial1.begin(9600);

  // Initialize the esp module
  WiFi.init(&Serial1);

  // Start connecting to wifi network and wait for connection to complete
  while (status != WL_CONNECTED)
  {
      Serial.print("Conecting to wifi network: ");
      Serial.println(ssid);

      status = WiFi.begin(ssid, pass);
  }

  // Once we are connected log the IP address of the ESP module
  Serial.print("IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());
  Serial.println("You're connected to the network");

  // Start the server
  server.begin();
}

void loop() {
  buttoncheck();
  wificheck();
}

void buttoncheck(){
  if (digitalRead(instantfeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, HIGH);
    digitalWrite(motorAin2, LOW); 
  }

  else if (digitalRead(timefeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, HIGH);
    digitalWrite(motorAin2, LOW);
    delay(feedtime);
     
  }

  else if (digitalRead(revfeedbtn) == HIGH)
  {
    digitalWrite(motorAin1, LOW);
    digitalWrite(motorAin2, HIGH);     
  }

  else if (digitalRead(scheduletog) == HIGH) //toggle currently has same function as instant feed
  {
    digitalWrite(motorAin1, HIGH); 
    digitalWrite(motorAin2, LOW);     
  }

  else 
  {
    digitalWrite(motorAin1, LOW);
    digitalWrite(motorAin2, LOW);  
  }
    
}

void wificheck(){
  WiFiEspClient client = server.available();

  // If a client has connected...
  if (client)
  {
      String json = "";
      Serial.println("A client has connected");
      bool currentLineIsBlank = true;
      
      while (client.connected())
      {   
          // Read in json from connected client
          if (client.available())
          {
              // ignore headers and read to first json bracket
              client.readStringUntil('{');

              // get json body (everything inside of the main brackets)
              String jsonStrWithoutBrackets = client.readStringUntil('}');

              // Append brackets to make the string parseable as json
              String jsonStr = "{" + jsonStrWithoutBrackets + "}";

              // if we managed to properly form jsonStr...
              if (jsonStr.indexOf('{', 0) >= 0)
              {
                  // parse string into json, bufferSize calculated by https://arduinojson.org/v5/assistant/
                  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 20;
                  DynamicJsonBuffer jsonBuffer(bufferSize);

                  JsonObject &root = jsonBuffer.parseObject(jsonStr);

                  // get and print the value of the action key in our json object
                  const char *value = root["action"];
                  Serial.println(value);

                  if (strcmp(value, "feed") == 0)
                  {
                      // Do something when we receive the on command
                      digitalWrite(motorAin1, HIGH);
                      digitalWrite(motorAin2, LOW);
                      delay(feedtime);
                      digitalWrite(motorAin1, LOW);
                      digitalWrite(motorAin2, LOW);  
                      Serial.println("Received on command from client");
                      //currentLineIsBlank = false;
                  }
                  
                  // send response and close connection
                  client.print(
                      "HTTP/1.1 200 OK\r\n"
                      "Connection: close\r\n" // the connection will be closed after completion of the response
                      "\r\n");

                  client.stop();
              }
              else
              {
                  // we were unable to parse json, send http error status and close connection
                  client.print(
                      "HTTP/1.1 500 ERROR\r\n"
                      "Connection: close\r\n"
                      "\r\n");

                  Serial.println("Error, bad or missing json");
                  client.stop();
              }
          }
      }
      delay(10);
      client.stop();
      Serial.println("Client disconnected");              
    }
}
