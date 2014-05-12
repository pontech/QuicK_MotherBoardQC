void printHash(uint8_t* hash) {
  MySerial.print(HashToString(hash,20));
}

void printHash(uint8_t* hash, uint8_t bytes) {
  MySerial.print(HashToString(hash,bytes));
}

String HashToString(uint8_t* hash, uint8_t bytes) {
  String hash_string = "";
  int i;
  for (i=0; i<bytes; i++) {
    hash_string += String("0123456789abcdef"[hash[i]>>4]);
    hash_string += String("0123456789abcdef"[hash[i]&0xf]);
  }
  return hash_string;
}
