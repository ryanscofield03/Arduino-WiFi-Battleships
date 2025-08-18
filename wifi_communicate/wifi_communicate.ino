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

			// messages

			// start game message
			// 1 byte - 1 byte for message type (shouldn't need data)

			// ready message (e.g., when you have placed all of your ships)
			// 1 byte - 1 byte for message type (shouldn't need data)

			// hit or miss message
			// 2 bytes - 1 byte for message type, and 1 byte for true/false (could be 1 byte total, but easier to give message type a byte)

			// shoot message (e.g., I shoot at (3,4) using my 2x2 or 1x1 shot)
			// 3-4 bytes - 1 for message type, 1-2 for x, y coordinates (depends if we do index based, or one-hot), 1 byte for the shoot type

			// grid + coordinate (e.g., the state of your board, for 'streaming' it to the other player)
			// 10-11 bytes - 1 for message type, 8 for 64 true/false values (true if I have shot at that position on the board), 1-2 bytes for coordinate
			// alternatively send 2 grid and coordinate as separate messages, but probably easier to pair them
		}
	}

  	if (client.available()) {
    	String msg = client.readStringUntil('\n');
    	Serial.println(msg);
  	}
}