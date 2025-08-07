int switch_val = 0;

void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
}

void loop() {
  switch_val = digitalRead(A0);

  if (switch_val == 0) {
    digitalWrite(12,LOW);
  }
  else if (switch_val == 1) {
    digitalWrite(12,HIGH);
  }
}
