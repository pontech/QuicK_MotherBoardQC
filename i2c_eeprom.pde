
void EEPROM_Print(us8 address)
{
  Wire.beginTransmission((int)address);  //0x53 quicK 0x50 Kard
  //Wire.send(0xFA);
  Wire.send(0x00);
  Wire.endTransmission();
  Wire.requestFrom((int)address, 6);    // request 6 bytes from slave device #2

  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.receive(); // receive a byte as character
    MySerial.print(c,HEX);         // print the character
    MySerial.print(".");
  }
  PrintCR();
}

us8 EEPROM_Init()
{
  //set all eeprom chip selects to low
  us8 i;
  for( i = 0; i < 7; i++ ) {
    pinMode(KardIO[i][5], OUTPUT);
    digitalWrite(KardIO[i][5],LOW);
  }
  delay(10);
}

us8 EEPROM_Read(us8 cs, us8 ADDR, us16 *bytes_read)
{
  *bytes_read = 0;
  us8 c;
  digitalWrite(cs,HIGH);
  Wire.beginTransmission(0x51);//0x53 quicK 0x51 Kard
  Wire.send(ADDR);
  Wire.endTransmission();
  Wire.requestFrom(0x51, 1);   // request 1 bytes from slave device
                               // can be up to limit of memory
  while(Wire.available())    // slave may send less than requested
  { 
    c = Wire.receive(); // receive a byte as character
    (*bytes_read)++;
  }
  digitalWrite(cs,LOW);
  return c;
}

void EEPROM_Write(us8 cs, us8 ADDR, us8 val)
{
//  MySerial.print(ADDR, HEX);
//  MySerial.print(":[");
//  MySerial.print(val, HEX);
//  MySerial.print("]");
//  MySerial.write(val);
//  PrintCR();
  digitalWrite(cs,HIGH);
  Wire.beginTransmission(0x51);//0x53 quicK 0x51 Kard
  Wire.send(ADDR);
  Wire.send(val);//up to 8 in sequence
  Wire.endTransmission();
  delay(10); //delay while writing
  digitalWrite(cs,LOW);
}

void EEPROM_Write(us8 cs, us8 ADDR, String str)
{
  EEPROM_Init();
  us16 len = str.length();
  for(int i = 0; i < len; i++) {
     EEPROM_Write(cs, ADDR + i, str[i]);
  }
}

void EEPROM_Write_Pad(us8 cs, us16 address, us16 last_address, us8 pad)
{
  EEPROM_Init();
  for(int i = address; i <= last_address; i++) {
     EEPROM_Write(cs, i, pad);
  }
}

String EEPROM_Write_JSON(us8 kard, us16 json)
{
  EEPROM_Init();
  MySerial.print("Kard");
  MySerial.print(kard, DEC);
  MySerial.print(":");
  Sha1.init();
  Sha1.print(kard_json[json]);
  
  String sha_result = HashToString(Sha1.result(), 4);
  EEPROM_Write(KardIO[kard][5], 0, sha_result);
  EEPROM_Write(KardIO[kard][5], 8, kard_json[json]);
  EEPROM_Write_Pad(KardIO[kard][5], 8 + kard_json[json].length(), 208, 0);
  EEPROM_Write_Pad(KardIO[kard][5], 209, 255, 255);
  
  MySerial.print(sha_result);
  MySerial.print(kard_json[json]);
  PrintCR();
  return sha_result + kard_json[json];
}

String EEPROM_Read_JSON(us8 kard)
{
  EEPROM_Init();
  String json;
  us16 address;
  MySerial.print("kard");
  MySerial.print(kard,HEX);
  MySerial.print(":");
  us16 bytes_read;
  for( address = 0; address < 209; address++ ) {
    us8 c = EEPROM_Read( KardIO[kard][5], address, &bytes_read);
    if( bytes_read == 0 )
    {
      MySerial.print("EEPROM Not Found");
      break;
    }
    if( c == 0 ) ; // do nothing
    else if( c < 128 ) {
      MySerial.write(c);
      json += c;
    }
    else {
      MySerial.print("[");
      MySerial.print(c,HEX);
      MySerial.print("]");
    }
  }
  PrintCR();
  return json;
}


