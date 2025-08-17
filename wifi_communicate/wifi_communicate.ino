#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "WiFiInformation.h"

WiFiUDP udp;
IPAddress SERVER_IP(192, 168, 1, 125);
unsigned int SERVER_PORT = 5006;
IPAddress otherIp;
bool otherDiscovered = false;

const int PIN_BUTTON = 2;
bool lastButtonState = HIGH;

void setup() {
	Serial.begin(9600);
	pinMode(PIN_BUTTON, INPUT_PULLUP);

	WiFi.begin(SSID, PASSWORD);
  	while (WiFi.status() != WL_CONNECTED) {
    	delay(1000);
  	}
  	udp.begin(SERVER_PORT);
}

void loop() {
  	udp.beginPacket(SERVER_IP, SERVER_PORT);
  	udp.write("READY");
  	udp.endPacket();

  	int packetSize = udp.parsePacket();
  	if (packetSize) {
    	char buffer[255];
    	int len = udp.read(buffer, 255);
    	buffer[len] = '\0';
    	Serial.println(buffer);
  	}

  	delay(2000);
}