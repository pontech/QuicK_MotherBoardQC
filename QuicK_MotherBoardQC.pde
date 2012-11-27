#include "pic32lib/core.h"
#include "TokenParser/TokenParser.h"

#define WantNewLine // todo: 2 comment out before finalized

// Serial USB for Serial
// Serial1 RS-232
// Serail2 485 sip
// Serail3 485 rail

// Max32 Pin Abstractions
us8 led1 = 37;
us8 led2 = 81;
us8 KardIO[6][6] = {
                    { 68, 58, 62, 55, 82, 32 }, // Kard 0
                    { 57, 56, 63, 54, 83, 31 }, // Kard 1
                    { 86, 64,  5, 70, 84, 30 }, // Kard 2
                    { 22, 76,  9,  2, 35, 52 }, // Kard 3
                    { 23, 39,  8, 21, 34, 50 }, // Kard 4
                    { 78, 79, 10, 20, 33, 85 }, // Kard 5
                   };

us8 KardUnderTest = 2;
// Kard 2 does not light one LED on KardDrive

//HardwareSerial& MySerial=Serial1;
USBSerial& MySerial=Serial;
us8 buff[0x20];

void setup()
{
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     

  pinMode(KardIO[KardUnderTest][0], OUTPUT);     
  pinMode(KardIO[KardUnderTest][1], OUTPUT);     
  pinMode(KardIO[KardUnderTest][2], OUTPUT);     
  pinMode(KardIO[KardUnderTest][3], OUTPUT);     
  pinMode(KardIO[KardUnderTest][4], OUTPUT);     
  pinMode(KardIO[KardUnderTest][5], OUTPUT);     

  // initialize serial ports:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  delay(5000);
  Serial.println("Serial - USB Open");
  Serial1.println("Serial1 - RS-232");
  Serial2.println("Serial2 - RS-485");
  Serial3.println("Serial3 - RS-485");
}


void loop() {
  static us8 ctr;
  us8 ch;

  while (MySerial.available() > 0)
  {
    ch = MySerial.read();
    if( ctr < sizeof(buff)) {
      buff[ctr++] = ch;
    }
    if (ch == '\n')
    {
      buff[ctr-1] = ' ';
    } 
    if (ch == '\r')
    {
      buff[ctr-1] = ' ';
      TokenParser tokpars(buff,ctr);
      tokpars.nextToken();
      ctr = 0;

      if( tokpars.compare("?",'|' ) || tokpars.compare("HELP" ) ) {
        MySerial.print("no help");
        PrintCR();
      }
      else if( tokpars.compare("SERIALTEST" ) ) {
        Serial.write('A');
        Serial1.write('A');
        Serial2.write('A');
        Serial3.write('A');
        MySerial.print("OK");
        PrintCR();
      }
    }
  }

  while (Serial1.available()) {
    int inByte = Serial1.read();

    Serial.print("1: You typed: '");
    Serial.write(inByte); 
    Serial.println("'");
  }

  while (Serial2.available()) {
    int inByte = Serial2.read();

    Serial.print("2: You typed: '");
    Serial.write(inByte); 
    Serial.println("'");
  }

  while (Serial3.available()) {
    int inByte = Serial3.read();

    Serial.print("3: You typed: '");
    Serial.write(inByte); 
    Serial.println("'");
  }

  digitalWrite(led1, HIGH);   // set the LED on
  digitalWrite(led2, LOW);    // set the LED off
  digitalWrite(KardIO[KardUnderTest][0], HIGH);
  digitalWrite(KardIO[KardUnderTest][1], LOW);
  digitalWrite(KardIO[KardUnderTest][2], LOW);
  digitalWrite(KardIO[KardUnderTest][3], LOW);
  digitalWrite(KardIO[KardUnderTest][4], LOW);
  digitalWrite(KardIO[KardUnderTest][5], LOW);
  delay(250);              // wait for a second

  digitalWrite(led1, LOW);    // set the LED off
  digitalWrite(led2, HIGH);   // set the LED on
  digitalWrite(KardIO[KardUnderTest][0], HIGH);
  digitalWrite(KardIO[KardUnderTest][1], HIGH);
  digitalWrite(KardIO[KardUnderTest][2], LOW);
  digitalWrite(KardIO[KardUnderTest][3], LOW);
  digitalWrite(KardIO[KardUnderTest][4], LOW);
  digitalWrite(KardIO[KardUnderTest][5], LOW);
  delay(250);              // wait for a second

  digitalWrite(led1, HIGH);    // set the LED off
  digitalWrite(led2, HIGH);   // set the LED on
  digitalWrite(KardIO[KardUnderTest][0], HIGH);
  digitalWrite(KardIO[KardUnderTest][1], HIGH);
  digitalWrite(KardIO[KardUnderTest][2], HIGH);
  digitalWrite(KardIO[KardUnderTest][3], LOW);
  digitalWrite(KardIO[KardUnderTest][4], LOW);
  digitalWrite(KardIO[KardUnderTest][5], LOW);
  delay(250);              // wait for a second

  digitalWrite(led1, HIGH);    // set the LED off
  digitalWrite(led2, LOW);   // set the LED on
  digitalWrite(KardIO[KardUnderTest][0], HIGH);
  digitalWrite(KardIO[KardUnderTest][1], HIGH);
  digitalWrite(KardIO[KardUnderTest][2], HIGH);
  digitalWrite(KardIO[KardUnderTest][3], HIGH);
  digitalWrite(KardIO[KardUnderTest][4], LOW);
  digitalWrite(KardIO[KardUnderTest][5], LOW);
  delay(250);              // wait for a second

  digitalWrite(led1, LOW);    // set the LED off
  digitalWrite(led2, LOW);   // set the LED on
  digitalWrite(KardIO[KardUnderTest][0], LOW);
  digitalWrite(KardIO[KardUnderTest][1], LOW);
  digitalWrite(KardIO[KardUnderTest][2], LOW);
  digitalWrite(KardIO[KardUnderTest][3], LOW);
  digitalWrite(KardIO[KardUnderTest][4], HIGH);
  digitalWrite(KardIO[KardUnderTest][5], HIGH);
  delay(250);              // wait for a second
}

void PrintCR() {
  #ifdef WantNewLine
  MySerial.print("\r\n");
  #else
  MySerial.print("\r");
  #endif
}

