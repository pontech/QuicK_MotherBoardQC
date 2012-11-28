#include "pic32lib/core.h"
#include "TokenParser/TokenParser.h"

#define WantNewLine // todo: 2 comment out before finalized

// Serial USB for Serial
// Serial0 TTL to Kard Slots
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

us8 KardUnderTest = 3;
// Kard 2 does not light one LED on KardDrive

//HardwareSerial& MySerial=Serial1;
USBSerial& MySerial=Serial;
us8 buff[0x20];

void setup()
{
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     

  // initialize serial ports:
  Serial.begin(115200);
  Serial0.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  delay(5000);
  Serial.println("Serial - USB Open");
  Serial0.println("Serial1 - TTL Kard");
  Serial1.println("Serial1 - RS-232");
  Serial2.println("Serial2 - RS-485");
  Serial3.println("Serial3 - RS-485");
}


void loop() {
  static us8 ctr;
  us8 ch;
  us8 i;
  e16 num1;//temporary values to parse into
  e16 num2;
  e32 num3;

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
      else if( tokpars.compare("V?",'|') ) {
        MySerial.print("QuicK-240 Motherboard Quality Control Sofwtare Version x.x");
        PrintCR();
      }
      else if( tokpars.compare("LOOPBACK" ) ) {
        Serial.write('A');
        Serial0.write('A');
        Serial1.write('A');
        Serial2.write('A');
        Serial3.write('A');
        MySerial.print("OK");
        PrintCR();
      }
      else if( tokpars.compare("QC.IN" ) ) {
          for( KardUnderTest = 0; KardUnderTest < 6; KardUnderTest++ ) {
            for( i = 0; i < 6; i++ ) {
              pinMode(KardIO[KardUnderTest][i], INPUT);
            }
          } 
          while (MySerial.available() == 0)
          {
            for( KardUnderTest = 0; KardUnderTest < 6; KardUnderTest++ ) {
              MySerial.print("K");
              MySerial.print(KardUnderTest,DEC);
              MySerial.print(" ");
              for( i = 0; i < 6; i++ ) {
                MySerial.print(digitalRead(KardIO[KardUnderTest][i]));
                if( i == 3 ) {
                  MySerial.print(" ");
                }
              }
              MySerial.print(" ");
            } 
            PrintCR();
            delay(50);
          }
          ch = MySerial.read();
          MySerial.print("OK");
          PrintCR();
      }
      else if( tokpars.compare("QC.OUT?" ) ) {
        tokpars.advanceTail(6);
        if(tokpars.contains("?"))
        {
          MySerial.print(KardUnderTest,DEC);
          PrintCR();
        }
        else
        {
          num1 = tokpars.to_e16();
          KardUnderTest = num1.value;
          MySerial.print(KardUnderTest,DEC);
          MySerial.print("-");
          if (KardUnderTest >= 0 && KardUnderTest <= 5)
          {
            for( i = 0; i < 6; i++ ) {
              pinMode(KardIO[KardUnderTest][i], OUTPUT);
            }
            for( i = 0; i < 6; i++ ) {
              digitalWrite(KardIO[KardUnderTest][i], HIGH);
              delay(250);
            }
            for( i = 0; i < 6; i++ ) {
              digitalWrite(KardIO[KardUnderTest][i], LOW);
              delay(250);
            }
            for( i = 0; i < 6; i++ ) {
              pinMode(KardIO[KardUnderTest][i], INPUT);
            }
          }
          else
          {
            MySerial.print("N");
          }
          MySerial.print("OK");
          PrintCR();
        }
      }
    }
  }

  while (Serial0.available()) {
    int inByte = Serial0.read();

    Serial.print("0: You typed: '");
    Serial.write(inByte); 
    Serial.println("'");
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
  delay(250);              // wait for a second

  digitalWrite(led1, LOW);    // set the LED off
  digitalWrite(led2, HIGH);   // set the LED on
  delay(250);              // wait for a second
}

void PrintCR() {
  #ifdef WantNewLine
  MySerial.print("\r\n");
  #else
  MySerial.print("\r");
  #endif
}

