// Serial USB for Serial
// Serial1 RS-232
// Serail2 485 sip
// Serail3 485 rail

char led1 = 37;
char led2 = 81;
void setup()
{
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     

  // initialize serial ports:
  Serial.begin(115200);
  Serial1.begin(115200);
//  Serial2.begin(115200);
//  Serial3.begin(115200);
  delay(5000);
  Serial.println("Serial - USB Open");
  Serial1.println("Serial1 - RS-232");
}

void loop() {
  //  Serial2.write(66);
  //  Serial3.write(66);

  while (Serial.available()) {
    int inByte = Serial.read();

    Serial.print("0: You typed: '");
    Serial.write(inByte); 
    Serial.println("'");
  }
  while (Serial1.available()) {
    int inByte = Serial1.read();

    Serial1.print("1: You typed: '");
    Serial1.write(inByte); 
    Serial1.println("'");
  }

  digitalWrite(led1, HIGH);   // set the LED on
  digitalWrite(led2, LOW);    // set the LED off
  delay(500);              // wait for a second
  digitalWrite(led1, LOW);    // set the LED off
  digitalWrite(led2, HIGH);   // set the LED on
  delay(500);              // wait for a second
}
