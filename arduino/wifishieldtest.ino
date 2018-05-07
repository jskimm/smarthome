#include <SPI.h>
#include <WiFi.h>
#include <Servo.h>
char ssid[] = "Sang";      // 공유기 이름
char pass[] = "99999999";   // 공유기 비밀번호
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
Servo servo;
WiFiServer server(80); // 웹서버 사용 포트 번호 80 81 82 

void setup() {
  Serial.begin(9600);      // 9600
  pinMode(9, OUTPUT);      // Red led 9번 핀 
  pinMode(8, OUTPUT);      // Green led 8
  pinMode(3, OUTPUT);      // Blue led 3
  servo.attach(2); // 서보모터

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

  server.begin();                           // start the web server on port 80
  printWifiStatus();        // you're connected now, so print out the        status
}

void loop() {
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
        client.println();

        // the content of the HTTP response follows the header:
        client.print("Click <a href=\"/HR\">here</a> turn the Red LED on pin 9 on<br>");
        client.print("Click <a href=\"/LR\">here</a> turn the Red LED on pin 9 off<br>");

        client.print("Click <a href=\"/HG\">here</a> turn the Green LED on pin 8 on<br>");
        client.print("Click <a href=\"/LG\">here</a> turn the Green LED on pin 8 off<br>");

        client.print("Click <a href=\"/HB\">here</a> turn the Blue LED on pin 3 on<br>");
        client.print("Click <a href=\"/LB\">here</a> turn the Blue LED on pin 3 off<br>");

        client.print("Click <a href=\"/HS\">here</a> servo motor angle 0~180<br>");
        client.print("Click <a href=\"/LS\">here</a> servo motor angle 180~0<br>");

        client.print("Click <a href=\"/HBU\">here</a> BUSER sound 1sec<br>");
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
    
    // Check to see if the client request was "GET /H" or "GET /L":
    int angle = 0;
    if (currentLine.endsWith("GET /HR")) {
      digitalWrite(9, HIGH);               // GET /HR RED LED ON
      digitalWrite(8, LOW);  
      digitalWrite(3, LOW);  
    }
    if (currentLine.endsWith("GET /LR")) {
      digitalWrite(9, LOW);                // GET /LR RED LED OFF
     }
     
    if (currentLine.endsWith("GET /HG")) {
      digitalWrite(8, HIGH);               // GET /HG 
      digitalWrite(9, LOW);
      digitalWrite(3, LOW);    
    }
    if (currentLine.endsWith("GET /LG")) {
      digitalWrite(8, LOW);                // GET /LG
     }
     
     if (currentLine.endsWith("GET /HB")) {
      digitalWrite(3, HIGH);               // GET /HB 
      digitalWrite(9, LOW);
      digitalWrite(8, LOW);    
    }
    if (currentLine.endsWith("GET /LB")) {
      digitalWrite(3, LOW);                // GET /LB
     }
     
     if (currentLine.endsWith("GET /HS")) {
      for(angle = 0; angle < 180; angle++)  // 서보모터 0~180
      { 
        servo.write(angle); 
        delay(15); 
      } 
    }
    if (currentLine.endsWith("GET /LS")) {
      for(angle = 180; angle > 0; angle--)  // 서보모터 180~0
      { 
        servo.write(angle); 
        delay(15); 
      } 
     }
     // 부저
     int sp = 4; 
     int note[] = {2093,2349,2637,2793,3136,3520,3951,4186}; //도레미파솔라시도
     if (currentLine.endsWith("GET /HBU")) {
       int elementCount = sizeof(note) / sizeof(int);
       for (int i=0; i < elementCount; i++)    //note를 play
       {
          tone(sp,note[i],500);
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
