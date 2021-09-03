void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial.println("Data, blah");
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print(random(50));
Serial.print(",");
Serial.println(random(60));
delay(random(300));
}
