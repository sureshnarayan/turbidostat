int dropTime = 10000;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT); 
  pinMode(8, OUTPUT);
  digitalWrite(7, HIGH);

  
  
  /*
  digitalWrite(7, LOW);
  delay(20);
  digitalWrite(7, HIGH);
  //delay(300);
  */
  
  digitalWrite(8, LOW);
  delay(500);
  digitalWrite(8, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      dropTime = inputString.toInt();
      inputString = "";
      Serial.println(dropTime);
    }
  }
}
