#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "WiFiInformation.h"

WiFiUDP udp;
unsigned int DISCOVERY_PORT = 12345;
unsigned int MESSAGE_PORT = 12346;
IPAddress otherIp;
bool otherDiscovered = false;

const int PIN_BUTTON = 2;
bool lastButtonState = HIGH;

void setup() {
	Serial.begin(9600);
	PinMode(PIN_BUTTON, INPUT_PULLUP);

	while (WiFi.begin(SSID, PASSWORD) != WL_CONNECTED) {
		Serial.println("Connecting to WiFi...");
    	delay(2000);
  	}
  	Serial.print("Connected to IP: "); Serial.println(WiFi.localIP());

	udp.begin(DISCOVERY_PORT);
	Serial.println("Listening on discovery port: "); Serial.println(DISCOVERY_PORT);
}

void loop() {
	handleDiscovery();
	handleButton();
	handleIncomingMessages();
}

void handleDiscovery() {
	udp.beginPacket(IPAddress(255, 255, 255, 255), DISCOVERY_PORT);
	udp.write("READY");
	udp.endPacket();

	int packetSize = udp.parsePacket();
	if (packetSize > 0) {
		char incoming[50];
		int len = udp.read(incoming, packetSize);
		if (len > 0) incoming[len] = 0;

		if (strcmp(incoming, "READY") == 0 && !otherDiscovered) {
			otherIp = udp.remoteIP();
			otherDiscovered = true;
			Serial.println("Discovered IP at: "); Serial.println(otherIp);
		}
	}
}

void handleButton() {
	bool buttonState = digitalRead(PIN_BUTTON);
	if (buttonState == LOW && lastButtonState == HIGH && otherDiscovered) {
		Serial.println("Sending message");
		udp.beginPacket(otherIp, MESSAGE_PORT);
		udp.write("ButtonPressed");
		udp.endPacket();
	}
	lastButtonState = buttonState;
}

void handleIncomingMessages() {
	int packetSize = udp.parsePacket();
	if (packetSize > 0) {
		char incoming[50];
		int len = udp.read(incoming, sizeof(incoming));
		if (len > 0) incoming[len] = 0;

		Serial.print("Received message: "); Serial.println(incoming);
	}
}