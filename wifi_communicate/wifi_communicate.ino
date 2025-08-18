#include <SPI.h>
#include <WiFiNINA.h>

#include "secrets.h"

// wifi
WiFiClient client;
IPAddress SERVER_IP(192, 168, 1, 125); // local IP of server
unsigned int SERVER_PORT = 5006;
int status = WL_IDLE_STATUS;

// pins
unsigned int PIN_BUTTON = 2;

// variables
unsigned long lastSentAt = 0;

void setup() {
	Serial.begin(9600);
	pinMode(PIN_BUTTON, INPUT_PULLUP);

  	while (status != WL_CONNECTED) {
  		status = WiFi.begin(SSID, PASSWORD);
    	Serial.println(WiFi.status());
    	delay(100);
  	}

	if (client.connect(SERVER_IP, SERVER_PORT)) {
		Serial.println("connected");
	}
}

void loop() {
	if (!client.connected()) {
    	Serial.println("reconnecting");
    	client.stop();
    	while (!client.connect(SERVER_IP, SERVER_PORT)) {
        	delay(1000);
    	}
		Serial.println("reconnected");
	}

	if (digitalRead(PIN_BUTTON) == LOW) {
		unsigned long now = millis();
		if (now - lastSentAt >= 250) {
			lastSentAt = now;
  			client.print("READY");
		}
	}

  	if (client.available()) {
    	String msg = client.readStringUntil('\n');
    	Serial.println(msg);
  	}
}