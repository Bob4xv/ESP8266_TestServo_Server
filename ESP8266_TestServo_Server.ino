/*********
  Complete project details at https://github.com/Bob4xv
*********/

#include <ESP8266WiFi.h>
#include <Servo.h>

#define debug_servo
#define first_pos_offset 15
#define angle 30
#define first_pos 1
#define top_pos 6

unsigned char position;

// create servo
Servo myservo0; // Cap 1 01
Servo myservo1; // Cap 2 02
Servo myservo2; // Switch 10

// GPIO the servo is attached to
static const int servoPin0 = D3;
static const int servoPin1 = D4;
static const int servoPin2 = D5;

// Replace with your network credentials
const char* ssid     = "MyWiFiName";
const char* password = "WiFipassword";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(03);
String servoID = String(12);

//VARCHAR vstring1[111],
//        vstring2[222]="mydata",
//        vstring3[333]="more data";	// test declare var in C to be used in xml

int pos1 = 0;
int pos2 = 0;
int pos3 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void sw_to_pos(int sw_pos){
	myservo2.write(((sw_pos-1)*angle)+first_pos_offset);
#ifdef debug_servo
	Serial.print("Switch= ");
	Serial.println((((sw_pos-1)*angle)+first_pos_offset));
#endif
	delay(15);
}

void setup() {
  Serial.begin(115200);

  myservo0.attach(servoPin0);  // attaches the servo on the servoPin to the servo object
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);
  delay(100);
  myservo2.write(first_pos);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 300px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

            // Web Page
            client.println("</head><body><h1>ESP32 with Servos</h1>");
            client.println("<p>Cap_C1: <span id=\"servoPos\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\""+valueString+"\"/>");
            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&01$\"); {Connection: close};}</script>");	// "&12$ points to servoID No.x

            client.println("<p>Cap_C2: <span id=\"servoPos1\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider1\" onchange=\"servo1(this.value)\" value=\""+valueString+"\"/>");
            client.println("<script>var slider1 = document.getElementById(\"servoSlider1\");");
            client.println("var servoP1 = document.getElementById(\"servoPos1\"); servoP1.innerHTML = slider1.value;");
            client.println("slider1.oninput = function() { slider1.value = this.value; servoP1.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function servo1(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&02$\"); {Connection: close};}</script>");	// "&12$ points to servoID No.x

            client.println("<p><label for=\"position\">Switch (1-6):</label>/p>");
            client.println("<input type=\"number\" id=\"switch1\" name=\"switch\" min=\"1\" max=\"6\" onchange=msg1()\"/>");
            client.println("<input type=\"submit\" onclick=\"servo2(switch1.value)\" value=\"Enter\"/>");

            client.println("<script>$.ajaxSetup({timeout:1000}); function servo2(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&10$\"); {Connection: close};}</script>");	// "&10$ points to servoID No.x

            client.println("<script> function msg() {alert(\"Submit!\"); }</script>");
            client.println("<script> function msg1() {alert(\"NUM onchange!\"); }</script>");

            client.println("</body></html>");
            Serial.print("Header= ");
            Serial.println(header);

            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              pos3 = header.indexOf('$');
              valueString = header.substring(pos1+1, pos2);
              servoID = header.substring(pos2+1,pos3);

              //Rotate the servo
              if(servoID=="01"){
              myservo0.write(valueString.toInt());
              }
              if(servoID=="02"){
            	  myservo1.write(valueString.toInt());
              }
              if(servoID=="10"){
            	  int pos = valueString.toInt();
            	  sw_to_pos(pos);
              }
              Serial.print("valueString= ");
              Serial.println(valueString);
              Serial.print("servoID= ");
              Serial.println(servoID);
           }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
