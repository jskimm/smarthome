#include <SPI.h>
#include <WiFi.h>
#include <Servo.h>

/* TEMP & HUMID */
#include "DHT.h"
#define DHTPIN 10      
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
int t = 0; //tmep
int h = 0; //humid
String data; // 출력용

/* WIFI */
char ssid[] = "Sang"; 
char pass[] = "99999999";
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);            // PORT NUMBER 80

Servo servo; //SERVO MOTOR DEC

const int gasPin = A0; //GAS PIN

/* DUST SENSOR */
int measurePin = 1;
int ledPower = 7;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  Serial.begin(9600);      
  
  /* LED PIN */
  pinMode(9, OUTPUT);      // Red led 9 
  pinMode(8, OUTPUT);      // Green led 8,INNER ROOM LED 8
  pinMode(5, OUTPUT);      // Blue led 5
  pinMode(3,OUTPUT);      // Aircon led 3
  pinMode(6,OUTPUT);      // Kitchen room led 6

  /* TEMP & HUMID */
  dht.begin();
  h=(int) dht.readHumidity();
  t=(int) dht.readTemperature();
  data = "";

  /* SERVO MOTOR */
  servo.attach(2);

  /* DUST SENSOR */
  pinMode(ledPower,OUTPUT);
  
  /* WIFI WHIELD CHECK */
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
  Serial.println("WiFi shield not present"); 
   while(true);        // don't continue
   } 

   // attempt to connect to Wifi network:
   while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
     status = WiFi.begin(ssid, pass);

     // wait 10 seconds for connection:
     delay(10000);
   } 

  server.begin();           // start the web server on port 80
  printWifiStatus();        // you're connected now, so print out the        status
}

void loop() {
  
  /* TEMP & HUMID */
  data = "<p id='tmp'>";
  data.concat(t);
  data.concat("</p>");
  data.concat("<br><p id='hum'>");
  data.concat(h);
  data.concat("</p>");

  /* DUST SENSOR */
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = (0.17 * calcVoltage - 0.1)*1000;
  
  //Serial.println(dustDensity); // unit: mg/m3

  delay(1000);
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";      // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected
     if (client.available()) {             // if there's bytes to read from the client,
     char c = client.read();             // read a byte, then
     Serial.write(c);                    // print it out the serial monitor
     if (c == '\n') {                    // if the byte is a newline character
      // if the current line is blank, you got two newline characters in a row.
      // that's the end of the client HTTP request, so send a response:
      if (currentLine.length() == 0) {  
        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
        // and a content-type so the client knows what's coming, then a blank line:    
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Access-Control-Allow-Origin:*");
        client.println();

        // the content of the HTTP response follows the header:
        client.print(" <a href=\"/HR\">here</a> Red on<br>"); 
        client.print(" <a href=\"/LR\">here</a> Red off<br>");

        client.print(" <a href=\"/HG\">here</a> Green on<br>");
        client.print(" <a href=\"/LG\">here</a> Green off<br>");

        client.print(" <a href=\"/HB\">here</a> Blue on<br>");
        client.print(" <a href=\"/LB\">here</a> Blue off<br>");

        client.print(" <a href=\"/HAIRL\">here</a> AIRCON on<br>");
        client.print(" <a href=\"/LAIRL\">here</a> AIRCON off<br>");
        
        //client.print(" <a href=\"/HINL\">here</a> INNER ROOM on<br>");
        //client.print(" <a href=\"/LINL\">here</a> INNER ROOM off<br>");

        //client.print(" <a href=\"/HKITL\">here</a> KITCHEN on<br>");
        //client.print(" <a href=\"/LKITL\">here</a> KITCHEN off<br>");

        client.print("<a href=\"/LS\">here</a> close<br>");
        client.print("<a href=\"/HS\">here</a> open<br>");

        client.print("<a href=\"/HFUNBU\">here</a> FUN <br>");
        client.print("<a href=\"/HWARMBU\">here</a> WARM <br>");
        
        client.print("<p id='gas'>");
        client.print(analogRead(gasPin));
        client.print("</p>");
        client.print("<br>");
        
        client.print(data);
        
        client.print("<p id='dust'>");
        client.print(dustDensity);
        client.print("</p>");
        
        // The HTTP response ends with another blank line:
        client.println();
        // break out of the while loop:
        break;         
       } 
       else {      // if you got a newline, then clear currentLine:
         currentLine = "";
       }
     }     
     else if (c != '\r') {  // if you got anything else but a carriage return character,
      currentLine += c;      // add it to the end of the currentLine
    }

    /* RGB LED */
    if (currentLine.endsWith("GET /HR")) {
      digitalWrite(9, HIGH);               // HIGH RED LED
      digitalWrite(8, LOW);  
      digitalWrite(5, LOW);  
    }
    if (currentLine.endsWith("GET /LR")) {
      digitalWrite(9, LOW);                // LOW RED LED
    }
     
    if (currentLine.endsWith("GET /HG")) {
      digitalWrite(8, HIGH);               // HIGH GREEN LED
      digitalWrite(9, LOW);
      digitalWrite(5, LOW);    
    }
    if (currentLine.endsWith("GET /LG")) {
      digitalWrite(8, LOW);                // LOW GREEN LED
    }
     
    if (currentLine.endsWith("GET /HB")) {
      digitalWrite(5, HIGH);               // HIGH BLUE LED
      digitalWrite(9, LOW);
      digitalWrite(8, LOW);    
    }
    if (currentLine.endsWith("GET /LB")) {
      digitalWrite(5, LOW);                // LOW BLUE LED
    }

    /* OTHER LED */
    if (currentLine.endsWith("GET /HAIRL")) {
      digitalWrite(3, HIGH);                // HIGH AIRCON LED
    }
    if (currentLine.endsWith("GET /LAIRL")) {
      digitalWrite(3, LOW);                // LOW AIRCON LED
    }
    
    if (currentLine.endsWith("GET /HINL")) {
      digitalWrite(8, HIGH);                // HIGH INNER ROOM LED
    }
    if (currentLine.endsWith("GET /LINL")) {
      digitalWrite(8, LOW);                // LOW INNER ROOM LED
    }
    
    if (currentLine.endsWith("GET /HLIVL ")) {
      digitalWrite(6, HIGH);                // HIGH LIVING ROOM LED
    }
    if (currentLine.endsWith("GET /LLIVL")) {
      digitalWrite(6, LOW);                // LOW LIVING ROOM LED
    }
     
    /*ALL LED CONTROLL*/
    if (currentLine.endsWith("GET /HALL")) {
      digitalWrite(6, HIGH);                // HIGH ALL ROOM LED ON
      digitalWrite(8, HIGH);
    }
    if (currentLine.endsWith("GET /LALL")) {
      digitalWrite(6, LOW);                // LOW ALL ROOM LED OFF
      digitalWrite(8, LOW); 
    }


    /* SERVO MOTOR */
    int angle = 0;
    if (currentLine.endsWith("GET /LS")) {
     for(angle = 80; angle < 210; angle++) 
     { 
        servo.write(angle);                 // WINDOW close 80~210
        delay(15); 
     } 
    }
    if (currentLine.endsWith("GET /HS")) {
      for(angle = 180; angle > 80; angle--) 
      { 
        servo.write(angle);                 // WINDOW open 180~80
        delay(15); 
      } 
    }
      
     /* BUSSOR SENSOR */
    int speakerpin = 4; 
    int note[] = {2093,2349,2637,2793,3136,3520,3951,4186}; //도레미파솔라시도
    if (currentLine.endsWith("GET /HFUNBU")) {
      digitalWrite(5, LOW);
      int elementCount = sizeof(note) / sizeof(int);
      for (int i=0; i < elementCount; i++)
      {
         tone(speakerpin,note[i],500);
         delay(750);
      }
    }
    int note2[] = {4186,3951,3520,3136,2793,2637,2349,2093}; //도시라솔파미레도
    if (currentLine.endsWith("GET /HWARMBU")) {
      digitalWrite(5, LOW);
      int elementCount = sizeof(note2) / sizeof(int);
      for (int i=0; i < elementCount; i++)    
      {
         tone(speakerpin,note2[i],500);
         delay(750);
      }
    }
   }
 }

// close the connection:
client.stop();
Serial.println("client disonnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
 Serial.print("SSID: ");
 Serial.println(WiFi.SSID());

 // print your WiFi shield's IP address:
 IPAddress ip = WiFi.localIP();
 Serial.print("IP Address: ");
 Serial.println(ip);

 // print the received signal strength:
 long rssi = WiFi.RSSI();
 Serial.print("signal strength (RSSI):");
 Serial.print(rssi);
 Serial.println(" dBm");

  // print where to go in a browser:
 Serial.print("To see this page in action, open a browser to http://");
 Serial.println(ip);
 }