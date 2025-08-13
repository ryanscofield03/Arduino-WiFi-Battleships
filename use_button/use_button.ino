#define BUTTON 2

// note to self:
// to setup, connect one button pin to ground and other button pin to pin 2
// both button pins should be on the same side of the bread board, with the
// button going over the center gap

bool toggle = false;
bool lastButtonState = HIGH;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop() {
  int buttonState = digitalRead(BUTTON);
  if (buttonState == LOW && lastButtonState == HIGH) {
        toggle = !toggle;
  }
  Serial.println(toggle);

  lastButtonState = buttonState;
}
