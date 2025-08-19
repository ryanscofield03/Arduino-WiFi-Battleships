#include <Adafruit_NeoPixel.h>

#define PIN 8
#define NUMPIXELS 64

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(10);
  strip.show();
}


void loop() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(255, 19, 159));
  }
  strip.show();
  delay(500);
}
