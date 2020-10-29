#include <WiFi.h>
#include <ESPServo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


// Replace with your network credentials
const char* ssid = "MySpectrumWiFiD8-2G";
const char* password = "anchorjacket176";
const char* mqtt_server = "192.168.1.49";
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;



Servo servo1;
Servo servo2;

//function definitions
void attachMotors();
void detachMotors();
void forward();
void backward();
void left();
void right();
void wirelessSetup(const char* ssid, const char* password, const char* mqtt_server);
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs

  // Connect to Wi-Fi network with SSID and password
  wirelessSetup(ssid, password, mqtt_server);
}

void loop(){

}

void wirelessSetup(const char* ssid, const char* password, const char* mqtt_server){
                                                          /////////////????????????????

  //setup_wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


}

void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "bot1") {
    Serial.print("Changing output to ");
    if(messageTemp == "left"){
      Serial.println("left");
      attachMotors();
			left();
			detachMotors();
    }
    else if(messageTemp == "right"){
      Serial.println("right");
      attachMotors();
			right();
			detachMotors();
    }
  }
	// can add more of the above block for different bots


}

void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("bot1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void attachMotors(){
servo1.attach(12);
servo2.attach(14);

}

void detachMotors(){
servo1.detach();
servo2.detach();
}

void forward(){
      servo1.write(180);
      servo2.write(0);
      delay(1000);
}
void backward(){
      servo1.write(0);
      servo2.write(180);
      delay(1000);
}
void left(){
      servo1.write(0);
      servo2.write(0);
      delay(100);
}
void right(){
      servo1.write(180);
      servo2.write(180);
      delay(100);
}
