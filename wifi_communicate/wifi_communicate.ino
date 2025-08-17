#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "WiFiInformation.h"

WiFiClient client;
IPAddress SERVER_IP(192, 168, 1, 125); // local IP of server
unsigned int SERVER_PORT = 5006;

void setup() {
	Serial.begin(9600);
	pinMode(PIN_BUTTON, INPUT_PULLUP);

	WiFi.begin(SSID, PASSWORD);
  	while (WiFi.status() != WL_CONNECTED) {
    	delay(1000);
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
	}

  	client.println("READY");

  	if (client.available()) {
    	String msg = client.readStringUntil('\n');
    	Serial.println(msg);
  	}
	delay(1000);
}