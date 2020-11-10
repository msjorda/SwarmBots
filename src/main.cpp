
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <EEPROM.h>
#include "esp_system.h"
#include "QueueArray.h"
#include "time.h"
#include <esp_wifi.h>
#include <iostream>
//OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <pthread.h>
#include <ESP32Servo.h>
#include <PubSubClient.h>



// Replace with your network credentials
const char* ssid = "MySpectrumWiFiD8-2G";
const char* password = "anchorjacket176";
const char* mqtt_server = "192.168.1.49";
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
String prevmsg = "";
String goal_dir = "";
int steering_ang;

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
void calibrate();
void drive(int steering_ang);

void setup() {
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servo1.setPeriodHertz(50);
	servo2.setPeriodHertz(50);
  Serial.begin(115200);
	Serial.println("Serial functional");
  // Initialize the output variables as outputs

  // Connect to Wi-Fi network with SSID and password
  wirelessSetup(ssid, password, mqtt_server);
	attachMotors();
}

void loop(){
//calibrate();

	if (!client.connected()) {
	    reconnect();
	  }
	  client.loop();

		Serial.println(steering_ang);
		drive(steering_ang);






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
  if (String(topic) == "[1]") {
    Serial.print("Changing output to ");
		goal_dir = messageTemp;

		steering_ang = goal_dir.toInt();

    if(messageTemp == "left"){
      Serial.println("left");
			//client.unsubscribe("[0]");
      //attachMotors();
			//delay(10);
			//left();
			//detachMotors();
			//client.subscribe("[0]", 0);
			//delay(200);
    }
    else if(messageTemp == "right"){
      Serial.println("right");
			//client.unsubscribe("[0]");
      //attachMotors();
			//delay(10);
			//right();
			//detachMotors();
			//client.subscribe("[0]", 0);
			//delay(200);
    }
  }
	// can add more of the above block for different bots


}

void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client1")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("[1]", 0);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}


void attachMotors(){
	Serial.println("attaching!");
if (servo1.attach(12) == 0){
	attachMotors();
}
if (servo2.attach(14) == 0){
	attachMotors();
}
//delay(50);

}

void detachMotors(){
	Serial.println("detaching!");
servo1.detach();
servo2.detach();
}

void forward(){
      servo1.write(120);
      servo2.write(60);
      delay(1000);
}
void backward(){
      servo1.write(60);
      servo2.write(120);
      delay(1000);
}
void left(){
			attachMotors();
			Serial.println("turning left!");
      servo1.write(60);
      servo2.write(60);
      delay(100);
}
void right(){
	attachMotors();
	Serial.println("turning right!");
      servo1.write(120);
      servo2.write(120);
      delay(100);
}

void drive(int steering_ang){
	attachMotors();
	if (steering_ang <= 90){
		//left
		//servo2 lock forward
		servo2.write(0);
		//at 90, forward
		//90 ==> 180
		servo1.write(steering_ang*2);
	}else if (steering_ang > 90 && steering_ang <= 180){
		//right
		//servo1 lock forward
		servo1.write(180);
		//at 180, forward
		//(180, 90) ==> (90, 180)
		//-(steering_ang)+270
		servo2.write(360-(steering_ang*2));
	}else if (steering_ang == 200){
		detachMotors();
	}
	delay(100);
}

void calibrate(){
	attachMotors();
	servo1.write(90);	// forward 90->180
	servo2.write(90); // forward 0<-90
	delay(5000);
}
