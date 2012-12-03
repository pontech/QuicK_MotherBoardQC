#include <SoftSPI.h>
SoftSPI SSPIDAC;


void ltc1257_begin( char cs_pin ) {
  pinMode(cs_pin,OUTPUT);
  digitalWrite(cs_pin,HIGH);
  SSPIDAC.begin(74,3,11,38); // SS, SDO, SDI, SCK
}

void ltc1257_end( char cs_pin ){
  pinMode(cs_pin,INPUT);
}

void ltc1257_write(unsigned char cs_pin, unsigned short int value)
{
    unsigned char output;

    if(value > 0x0fff) {
        // latch high if trying to drive higher than max ouput (to prenvent overflow to a low value)
        value = 0x0fff;
    }
    //spi_mode(1, 0, 1);
    
    output = (value>>8);
    SSPIDAC.transfer(output);
    output = (value & 0xff );
    SSPIDAC.transfer(output);
    
    digitalWrite(cs_pin,LOW);
    digitalWrite(cs_pin,HIGH);
}
