
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


