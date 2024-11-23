
void setup() {

  Serial.begin(9600);
  
}

void loop() {
  if (Serial.available() > 0) {
      String message = Serial.readStringUntil('\n');
      Serial.print("Server: ");
      Serial.print(message);
      Serial.println(" - changed!");
  }
}
