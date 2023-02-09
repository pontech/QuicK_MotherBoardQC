
//////////////////////////////////////////////////////
// QC Quck240 Motherboard Kard Slots
//////////////////////////////////////////////////////
// PRESS ENTER TO SWITCH TO THE NEXT KARD SLOT
// This test requires a BO-100 card with Px and Ux
// shorted and a 10k pull-up on +3.3V for each
// position on the kard.  Then when the test runs
// put the kard in slots 0 through 5 and short 
// adjacent pins to test for ability of signals to
// get from the uC to the rail and back again.
//////////////////////////////////////////////////////
void qc_slots(void)
{
  char s[100];
  
  for( int8_t kard = 0; kard < 6; kard++ )
  {
    snprintf(s, 100, "Testing Kard[%d] PRESS ENTER TO SWITCH TO THE NEXT KARD SLOT", kard);
    Serial.println(s);
    
    while(Serial.available() == 0)
    {
      // Set all pins as inputs (pull up)
      for( int8_t pin = 0; pin < 6; pin++ ) {
        pinMode(KardIO[kard][pin], INPUT_PULLUP);
      }

      for( int8_t drive = 0; drive < 4; drive++ )
      {
        // Drive pin x output & low
        pinMode(KardIO[kard][drive], OUTPUT);
        digitalWrite(KardIO[kard][drive], LOW);
        delay(1);
        
        for( int8_t check = 0; check < 4; check++ )
        {
          if( check != drive && digitalRead(KardIO[kard][check]) == LOW  )
          {
            snprintf(s, 100, "Kard[%d] pin %d to %d", kard, drive, check);
            Serial.println(s);
            delay(50);
            // if(Serial.available() != 0) goto exit_slots;
          }
        }
        pinMode(KardIO[kard][drive], INPUT_PULLUP);
      }
    }
    Serial.read();
  }

//exit_slots:
}

///////////////////////////////////////
// QC Output Kards (Like DRIVE-KARD
///////////////////////////////////////
uint8_t qc_out(uint8_t KardUnderTest, uint32_t on_time)
{
  int8_t i;
  
  if (KardUnderTest >= 0 && KardUnderTest <= 5) {
    // set the pins as output but shut off.
    for( i = 0; i < 6; i++ ) {
      pinMode(KardIO[KardUnderTest][i], OUTPUT);
      digitalWrite(KardIO[KardUnderTest][i], LOW);
    }

    // set each output high for on_time ms
    for( i = 0; i < 6; i++ ) {
      digitalWrite(KardIO[KardUnderTest][i], HIGH);
      delay(on_time);
      digitalWrite(KardIO[KardUnderTest][i], LOW);
    }

    for( i = 0; i < 6; i++ ) {
      pinMode(KardIO[KardUnderTest][i], INPUT);
    }
    return true;
  }
  return false;
}

///////////////////////////////////////
// QC Iso Kards
///////////////////////////////////////
uint8_t qc_iso(uint32_t on_time)
{
  uint8_t kardpass[6];
  uint8_t pass = true;
  int8_t i;
  int8_t o;
  int8_t value;
  
  uint8_t KardDrive = 0;
  uint8_t KardUnderTest;

  // Kard 0 is the drive kard output
  for( i = 0; i < 6; i++ ) {
    pinMode(KardIO[KardDrive][i], OUTPUT);
    digitalWrite(KardIO[KardDrive][i], LOW);
  }

  // Kard 1-5 are the iso kard inputs
  Serial.print("       ");
  for( KardUnderTest = 1; KardUnderTest < 6; KardUnderTest++ ) {
    Serial.print("k[");
    Serial.print(KardUnderTest, DEC);
    Serial.print("] ");
    kardpass[KardUnderTest] = true;
    for( i = 0; i < 6; i++ ) {
      pinMode(KardIO[KardUnderTest][i], INPUT);
      digitalWrite(KardIO[KardUnderTest][i], LOW);
    }
  }
  Serial.println();

  // set each output high for on_time ms
  for( o = 0; o < 5; o++ ) {
    Serial.print("pin[");
    Serial.print(o, DEC);
    Serial.print("]=");
    digitalWrite(KardIO[KardDrive][o], HIGH);
    delay(on_time);
    // print out the input kards
    for( KardUnderTest = 1; KardUnderTest < 6; KardUnderTest++ ) {
      for( i = 0; i < 4; i++ ) {
        value = digitalRead(KardIO[KardUnderTest][i]);
        if( i == o && value != 0 ) kardpass[KardUnderTest] = false;
        if( i != o && value != 1 ) kardpass[KardUnderTest] = false;
        Serial.print(value, DEC);
      }
      Serial.print(" ");
    }
    digitalWrite(KardIO[KardDrive][o], LOW);
    Serial.println();
  }
  Serial.println();
  
  // print out pass/fail per kard
  Serial.print("       ");
  for( KardUnderTest = 1; KardUnderTest < 6; KardUnderTest++ ) {
    pass = pass && kardpass[KardUnderTest];
    if( kardpass[KardUnderTest] == true ) 
      MySerial.print("pass ");
    else 
      MySerial.print("FAIL ");
  }
  Serial.println();
  Serial.println();

  return pass;
}

