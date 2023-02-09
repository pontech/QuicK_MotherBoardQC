// Tested on:
// MPIDE0023 - 20140316

// chipKIT Tool Chain (Add to Additional Boards Manager URL's list):
// https://github.com/chipKIT32/chipKIT-core/raw/master/package_chipkit_index.json

#include "Half_Duplex_Turnaround.h"
#include "pic32lib/core.h"
#include "TokenParser/TokenParser.h"
#include "LTC1257CS8.h"
#include "sha1.h"
#include <SoftSPI.h>
#include <Wire.h>

#define WantNewLine // todo: 2 comment out before finalized

// Serial USB for Serial
// Serial0 TTL to Kard Slots
// Serial1 RS-232
// Serail2 485 sip
// Serail3 485 rail

// Max32 Pin Abstractions
us8 led1 = 37;
us8 led2 = 81;
#line 20

const us8 KARD_JSON_MAX = 9;

String kard_json[] = {
  "{\"org\":\"pontech.com\",\"cn\":\"ISO Common Cathode\",\"rev\":\"D\",\"io\":31}",
  "{\"org\":\"pontech.com\",\"cn\":\"ISO Common Anode\",\"rev\":\"D\",\"io\":31}",
  "{\"org\":\"pontech.com\",\"cn\":\"Drive Source\",\"rev\":\"C\",\"io\":0}",
  "{\"org\":\"pontech.com\",\"cn\":\"Drive Sink\",\"rev\":\"C\",\"io\":0}",
  "{\"org\":\"pontech.com\",\"cn\":\"BigEasy\",\"rev\":\"C\",\"io\":0,\"label\":[\"step\",\"dir\",\"enable\",\"sleep\",\"ms3\"],\"i2cMask\":40}",
  "{\"org\":\"pontech.com\",\"cn\":\"BigEasy\",\"rev\":\"C1\",\"io\":0,\"label\":[\"step\",\"dir\",\"enable\",\"sleep\",\"ms2\"],\"i2cMask\":40}",
  "{\"org\":\"pontech.com\",\"cn\":\"BigEasy\",\"rev\":\"D\",\"io\":0,\"label\":[\"step\",\"dir\",\"enable\",\"ms1\",\"ms2\"],\"i2cMask\":80}",
  "{\"org\":\"pontech.com\",\"cn\":\"BigEasy\",\"rev\":\"E\",\"io\":0,\"label\":[\"step\",\"dir\",\"enable\",\"ms1\",\"ms2\"],\"i2cMask\":82}",
  "{\"org\":\"pontech.com\",\"cn\":\"BO-100-QC\",\"rev\":\"A\",\"io\":0}"
};

us8 KardIO[7][6] = {
                    { 68, 58, 62, 55, 82, 32 }, // Kard 0
                    { 57, 56, 63, 54, 83, 31 }, // Kard 1
                    { 86, 64,  5, 70, 84, 30 }, // Kard 2
                    { 22, 76,  9,  2, 35, 52 }, // Kard 3
                    { 23, 39,  8, 21, 34, 50 }, // Kard 4
                    { 78, 79, 10, 20, 33, 85 }, // Kard 5
                    { 44, 44, 44, 44, 44, 44 }, // Kard Com
                   };

us8 KardUnderTest = 3;
// Kard 2 does not light one LED on KardDrive

//HardwareSerial& MySerial=Serial1;
USBSerial& MySerial=Serial;
us8 buff[0x20];
SerialHalf MySerial2(&Serial2, 1, true);
SerialHalf MySerial3(&Serial3, 71, true);

char waitforCR() {
  while (MySerial.available() == 0)
    ; // do nothing
  return MySerial.read();
}

void setup()
{
  // Clear RCON bits so next reset provide meaningful information
  RCON = 0x00000000;
  Wire.begin();

  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);     

  // initialize serial ports:
  Serial.begin(115200);
  //Serial0.begin(115200);
  Serial1.begin(115200);
  MySerial2.begin(115200);
  MySerial3.begin(115200);

  EEPROM_Init();

  delay(5000);
  Serial.println("Serial - USB Open");
//  Serial0.println("Serial1 - TTL Kard");
//  Serial1.println("Serial1 - RS-232");
//  MySerial2.println("Serial2 - RS-485");
//  MySerial3.println("Serial3 - RS-485");
}
void loop() {
  static us8 ctr;
  us8 ch;
  us8 i;
  us8 j;
  e16 num1;//temporary values to parse into
  e16 num2;
  e32 num3;

//  if (Serial1.available() > 0) {
//    MySerial.write(Serial1.read()); // Send it out via usb
//  }
//  if (MySerial2.available() > 0) {
//    MySerial.write(MySerial2.read()); // Send it out via usb
//  }
//  if (MySerial3.available() > 0) {
//    MySerial.write(MySerial3.read()); // Send it out via usb
//  }

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
        MySerial.print("? This list"); PrintCR();
        MySerial.print("V? Version"); PrintCR();
        MySerial.print("reset, reset the microcontroller"); PrintCR();
        MySerial.print("json, display a list of avaible json strings"); PrintCR();
        MySerial.print("json.read, read json string from kards 0-6"); PrintCR();
        MySerial.print("json.write N, write json string N to kards 0-6"); PrintCR();


        MySerial.print("LOOPBACK Serial loopback testing."); PrintCR();
        MySerial.print("EEWRITE kard address value"); PrintCR();
        MySerial.print("EEREAD?"); PrintCR();
        MySerial.print("EWRITE"); PrintCR();
        MySerial.print("ERASE"); PrintCR();
        MySerial.print("QC.EE"); PrintCR();
        MySerial.print("QC.IN"); PrintCR();
        MySerial.print("QC.OUT n Test output kard where n = 0-5, 10 for all."); PrintCR();
        MySerial.print("QC.STEP"); PrintCR();
        MySerial.print("QC.QAC"); PrintCR();
        MySerial.print("QC.DAC"); PrintCR();
        MySerial.print("QC.DRV"); PrintCR();
        MySerial.print("QC.ISO"); PrintCR();
        MySerial.print("QC.SLOTS, Use the BO-100-QC to test path from uC to rail by shorting pins"); PrintCR();

        PrintCR();
      }
      else if( tokpars.compare("QC.SLOTS") ) {
        qc_slots();
      }
      else if( tokpars.compare("QC.ISO") ) {
        if( qc_iso(1000) ) 
          Serial.println("all iso kards pass");
        else
          Serial.println("at least one iso kard failed");
      }
      else if( tokpars.compare("QC.DRV") ) {
        qc_out(3, 1000);
        qc_out(4, 1000);
        qc_out(5, 1000);
      }
      else if( tokpars.compare("JSON.READ") ) {
        us8 kard = 0;
        for( kard = 0; kard < 7; kard ++ ) {
          EEPROM_Read_JSON(kard);
        }
      }
      else if( tokpars.compare("JSON.WRITE?") ) {
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 json = num1.value;

        for(us8 kard = 0; kard < 7; kard++) {
          EEPROM_JSON_WRITE(json, kard);
        } 
      }
      else if( tokpars.compare("JSON.DRV?") ) {
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 json = num1.value;

        for(us8 kard = 3; kard < 6; kard++) {
          EEPROM_JSON_WRITE(json, kard);
        } 
      }
      else if( tokpars.compare("JSON.ISO?") ) {
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 json = num1.value;

        for(us8 kard = 1; kard < 6; kard++) {
          EEPROM_JSON_WRITE(json, kard);
        } 
      }
      else if( tokpars.compare("JSON") ) {
        for( i = 0; i < KARD_JSON_MAX; i++ ) {
          MySerial.print(i, DEC);
          MySerial.print(": ");
          Sha1.init();
          Sha1.print(kard_json[i]);
          printHash(Sha1.result(), 4);
          MySerial.print(kard_json[i]);
          PrintCR();
        }
        PrintCR();
      }
      else if( tokpars.compare("RESET") ) {
        MySerial.print("Close serial terminal, resetting board in...");
        PrintCR();
        us8 sec;
        for( sec = 5; sec >= 1; sec-- ) {
          MySerial.print(sec, DEC);
          MySerial.print(" seconds...");
          PrintCR();
          delay(1000);
        }
        // Example 7-1 from Section 7 of PIC32 Family Manual

        /* The following code illustrates a software Reset */
        // assume interrupts are disabled
        // assume the DMA controller is suspended
        // assume the device is locked

        /* perform a system unlock sequence */

        // starting critical sequence
#ifdef VIRTUAL_PROGRAM_BUTTON_TRIS
  VIRTUAL_PROGRAM_BUTTON_TRIS = 0; //Set virtual button as output
  VIRTUAL_PROGRAM_BUTTON = 1; //push virtual button
#endif
        SYSKEY = 0x00000000;  //write invalid key to force lock
        SYSKEY = 0xAA996655;  //write key1 to SYSKEY
        SYSKEY = 0x556699AA;  //write key2 to SYSKEY
        // OSCCON is now unlocked
        /* set SWRST bit to arm reset */
        RSWRSTSET = 1;
        /* read RSWRST register to trigger reset */
        unsigned int dummy;
        dummy = RSWRST;
        /* prevent any unwanted code execution until reset occurs*/
        while(1);
      }
      else if( tokpars.compare("V?",'|') ) {
        String vers = "QuicK-240 Motherboard Quality Control Sofwtare Version x.x\r\n";
        MySerial.print(vers.trim());
        PrintCR();
        MySerial.print(vers.toLowerCase());
        PrintCR();
        MySerial.print(vers.toUpperCase());
        PrintCR();
      }
      else if( tokpars.compare("LOOPBACK" ) ) {
        Serial.write('A');
        Serial1.write('A');
        MySerial2.write('A');
        MySerial3.write('A');
        MySerial.print("OK");
        PrintCR();
      }
      else if ( tokpars.compare("EEWRITE?" )){
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 kard = num1.value;
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 address = num1.value;
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 value = num1.value;
        EEPROM_Write( KardIO[kard][5], address, value);
        MySerial.print("OK");
        PrintCR();
      }
      else if ( tokpars.compare("EEREAD?" )){
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 kard = num1.value;
        tokpars.nextToken();
        num1 = tokpars.to_e16();
        us8 address = num1.value;
        us16 bytes_read;
        MySerial.print(EEPROM_Read( KardIO[kard][5], address,&bytes_read),DEC);
        PrintCR();
      }
      else if ( tokpars.compare("EWRITE" )){
        for( KardUnderTest = 0; KardUnderTest < 7; KardUnderTest++ ) {
          pinMode(KardIO[KardUnderTest][5], OUTPUT);
          digitalWrite(KardIO[KardUnderTest][5],HIGH);

        }

        for( i = 0; i < 6; i++ )
        {
          digitalWrite(KardIO[i][5],LOW);
          Wire.beginTransmission(0x50);  //0x53 quicK 0x50 Kard
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x01);
          Wire.send(0x02);
          Wire.send(0x03);
          Wire.send(0x04);
          Wire.send(0x05);
          Wire.send(0x06);
          Wire.endTransmission();

          digitalWrite(KardIO[i][5],HIGH);
        }

        MySerial.print("OK");
        PrintCR();
      }
      else if ( tokpars.compare("ERASE" )){
        for( KardUnderTest = 0; KardUnderTest < 7; KardUnderTest++ ) {
          pinMode(KardIO[KardUnderTest][5], OUTPUT);
          digitalWrite(KardIO[KardUnderTest][5],HIGH);

        }

        for( i = 0; i < 6; i++ )
        {
          digitalWrite(KardIO[i][5],LOW);
          Wire.beginTransmission(0x50);  //0x53 quicK 0x50 Kard
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.send(0x00);
          Wire.endTransmission();

          digitalWrite(KardIO[i][5],HIGH);
        }

        MySerial.print("ERASED");
        PrintCR();
      }
      else if( tokpars.compare("QC.EE" ) ) {
        MySerial.print("QuicK0:");
        EEPROM_Print(0x53);

        for( KardUnderTest = 0; KardUnderTest < 7; KardUnderTest++ ) {
          pinMode(KardIO[KardUnderTest][5], OUTPUT);
          digitalWrite(KardIO[KardUnderTest][5],HIGH);
        }
        for( KardUnderTest = 0; KardUnderTest < 7; KardUnderTest++ ) {
          MySerial.print("KARD-");
          MySerial.print(KardUnderTest,DEC);
          MySerial.print(":");
          digitalWrite(KardIO[KardUnderTest][5],HIGH);

          EEPROM_Print(0x51);
          digitalWrite(KardIO[KardUnderTest][5],LOW);
        }

        //for( KardUnderTest = 0; KardUnderTest < 7; KardUnderTest++ ) {
        //  pinMode(KardIO[KardUnderTest][5], INPUT);
        //}
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
      else if( tokpars.compare("QC.OUT" ) ) {
        //tokpars.advanceTail(6); // use this (or something like this if you do NOT want a space) 
        tokpars.nextToken(); // use this (or something like this if you want a space)
        num1 = tokpars.to_e16();
        KardUnderTest = num1.value;
        MySerial.print(KardUnderTest,DEC);
        MySerial.print("-");
        if (KardUnderTest == 10) 
        {
          for( i = 0; i < 6; i++ )
          {
            for( j = 0; j < 6; j++ ) 
            {
              pinMode(KardIO[i][j], OUTPUT);
              digitalWrite(KardIO[i][j], LOW);
            }

          }

          for( i = 0; i < 6; i++ ) 
          {
            for( j = 0; j < 6; j++ ) 
            {
              digitalWrite(KardIO[i][j], HIGH);
              delay(250);
              digitalWrite(KardIO[i][j], LOW);
            }

          }

          for( i = 0; i < 6; i++ ) 
          {
            for( j = 0; j < 6; j++ ) 
            {
              digitalWrite(KardIO[i][j], HIGH);
            }
            delay(250);
          }

          for( i = 0; i < 6; i++ ) 
          {
            for( j = 0; j < 6; j++ ) 
            {
              digitalWrite(KardIO[i][j], LOW);
            }
            delay(250);
          }
        }
        
        else if (KardUnderTest >= 0 && KardUnderTest <= 5) {
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
        else {
          MySerial.print("N");
        }
        MySerial.print("OK");
        PrintCR();
      }
      else if( tokpars.compare("QC.STEP" ) ) {
        us32 pos;
        //tokpars.advanceTail(6); // use this (or something like this if you do NOT want a space) 
        tokpars.nextToken(); // use this (or something like this if you want a space)
        num1 = tokpars.to_e16();
        tokpars.nextToken(); // use this (or something like this if you want a space)
        num2 = tokpars.to_e16();
        KardUnderTest = num1.value;
        MySerial.print(KardUnderTest,DEC);
        MySerial.print("-");
        if (KardUnderTest >= 0 && KardUnderTest <= 5) {
          pinMode(KardIO[KardUnderTest][0], OUTPUT);
          pinMode(KardIO[KardUnderTest][1], OUTPUT);
          pinMode(KardIO[KardUnderTest][2], OUTPUT);
          pinMode(KardIO[KardUnderTest][3], OUTPUT);
          digitalWrite(KardIO[KardUnderTest][1], HIGH);
          digitalWrite(KardIO[KardUnderTest][2], LOW);
          digitalWrite(KardIO[KardUnderTest][3], HIGH);
          for( pos = 0; pos < num2.value; pos++ ) {
            digitalWrite(KardIO[KardUnderTest][0], HIGH);
            delayMicroseconds(10);
            digitalWrite(KardIO[KardUnderTest][0], LOW);
            delayMicroseconds(250);
          }
          digitalWrite(KardIO[KardUnderTest][1], LOW);
          for( pos = 0; pos < num2.value; pos++ ) {
            digitalWrite(KardIO[KardUnderTest][0], HIGH);
            delayMicroseconds(10);
            digitalWrite(KardIO[KardUnderTest][0], LOW);
            delayMicroseconds(250);
          }
          for( i = 0; i < 6; i++ ) {
            pinMode(KardIO[KardUnderTest][i], INPUT);
          }
        }
        else {
          MySerial.print("N");
        }
        MySerial.print("OK");
        PrintCR();
      }
      else if( tokpars.compare("QC.QAC" ) ) {
        us8 dac;
        tokpars.nextToken(); // use this (or something like this if you want a space)
        num1 = tokpars.to_e16();
        KardUnderTest = num1.value;
        MySerial.print("Kard-");
        MySerial.print(KardUnderTest,DEC);
        MySerial.print(" DAC Test");
        PrintCR();
        for( dac = 0; dac < 4; dac++ ) {
          ltc1257_begin(KardIO[KardUnderTest][dac]);
          unsigned short int value;
          value = (dac + 1) * (0xfff / 10);
          ltc1257_write(KardIO[KardUnderTest][dac], value);
        }
      }
      else if( tokpars.compare("QC.DAC" ) ) {
        us8 dac;
        tokpars.nextToken(); // use this (or something like this if you want a space)
        num1 = tokpars.to_e16();
        KardUnderTest = num1.value;
        MySerial.print("Kard-");
        MySerial.print(KardUnderTest,DEC);
        MySerial.print(" DAC Test");
        PrintCR();
        for( dac = 0; dac < 4; dac++ ) {
          ltc1257_begin(KardIO[KardUnderTest][dac]);
          for( i = 0; i <= 10; i++ ) {
            unsigned short int value;
            value = i * (0xfff / 10);
            ltc1257_write(KardIO[KardUnderTest][dac], value);
            MySerial.print("Kard-");
            MySerial.print(KardUnderTest,DEC);
            MySerial.print(",");
            MySerial.print(dac,DEC);
            MySerial.print(": ");
            MySerial.print(i,DEC);
            MySerial.print("V");
            MySerial.print(": ");
            MySerial.print(value,DEC);
            MySerial.print("DAC");
            PrintCR();
            waitforCR();
          }
          //ltc1257_end(KardIO[KardUnderTest][dac]);
        }
      }
    }
  }

//  while (Serial0.available()) {
//    int inByte = Serial0.read();
//
//    Serial.print("0: You typed: '");
//    Serial.write(inByte); 
//    Serial.println("'");
//  }
//
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
