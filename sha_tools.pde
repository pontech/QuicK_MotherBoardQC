void printHash(uint8_t* hash) {
  int i;
  for (i=0; i<20; i++) {
    MySerial.print("0123456789abcdef"[hash[i]>>4]);
    MySerial.print("0123456789abcdef"[hash[i]&0xf]);
  }
}


