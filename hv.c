//12 MHz with external crystal oscillator to allow V-USB to work
//set fuse bits with the following command
//avrdude -p t84 -U lfuse:w:0x6f:m -U hfuse:w:0xdf:m - U efuse:w:0xff:m
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "hv.h"
//turns on pmos for boost converter and 555 timer
#define RESET_HV PA1
#define SDI PA2
#define SII PA3
#define SDO PA4
#define SCI PA6
#define VCC PA5
//supplies to 12V from boost converter to reset pin
//connected to bjts
//can't use HV_ macros since it's PORTB
#define RESET_EN PB2
//don't actually need macros for D+ and D- here only for information about what other pins are used for
//#define D+ PA0
//#define D- PA7
#define WH(PORT,PIN) PORT |= (1 << PIN)
#define WL(PORT,PIN) PORT &= ~(1 << PIN)
#define HV_WH(PIN) WH(PORTA,PIN)
#define HV_WL(PIN) WL(PORTA,PIN)
#define TOGGLE_CLK HV_WH(SCI);HV_WL(SCI)
#define BOOST_WAIT_TIME 3

void enter_hv(void)
{
  //sdo temporarily made output, released later
  DDRA |= (1 << SDI)|(1 << SII)|(1 << SCI)|(1 << RESET_HV)|(1 << VCC)|(1 << SDO);
  //make reset_en output
  DDRB |= (1 << RESET_EN);
  //disable boost converter output
  PORTB &= ~(1 << RESET_EN);
  //supply voltage to boost converter
  HV_WL(RESET_HV);
  //wait 3 ms for boost converter to stabilize to 12V
  _delay_ms(BOOST_WAIT_TIME);

  //start of actual enter hvsp on attinyx5
  HV_WL(SDI);
  HV_WL(SII);
  HV_WL(SDO);
  HV_WL(VCC);
  //wait
  _delay_us(100);
  //if vcc rise time is too long, may need to use alternate algorithm
  HV_WH(VCC);
  //wait 40 us
  _delay_us(40);
  //enable bjt amp to supply 12V
  PORTB |= (1 << RESET_EN);
  //making sure prog enable signature is latched
  _delay_us(10);
  //release sdo to avoid contention
  DDRA &= ~(1 << SDO);
  //wait before giving instructions
  _delay_us(300);
  //next delay is to give me enough time to read the output on the voltmeter
  _delay_ms(20000);
  //give instructions
}
void exit_hv(void)
{
  PORTB &= ~(1 << RESET_EN);//remove 12V supply
  HV_WH(RESET_HV);//turn off boost converter
  HV_WL(VCC);//remove power from attiny85
}

uint8_t rw_byte(uint8_t sdi, uint8_t sii)
{
  //commands are 11 bits with the following format
  // 0_xxxx_xxxx_00
  HV_WL(SDI);
  HV_WL(SII);
  TOGGLE_CLK;
  uint8_t i;
  uint8_t data = 0;
  for (i = 0; i < 8; i++)
  {
    TOGGLE_CLK;
    //data |= (PINA & (1 << SDO)) << (7-SDO-i);
    data |= (PINA & (1 << SDO)) << i;
    if (sii & (1 << (7-i))) HV_WH(SII);
    if (sdi & (1 << (7-i))) HV_WH(SDI);
  }
  HV_WL(SDI);
  HV_WL(SII);
  TOGGLE_CLK;
  TOGGLE_CLK;
  //flip data
  data = (data & 0x0f) << 4| (0xf0 & data) >> 4;
  data = (data & 0x33) << 2| (0xcc & data) >> 2;
  data = (data & 0x55) << 1| (0xaa & data) >> 1;
  return data;
}

//host software will take care of warning people from writing fuse bits incorrectly masking efuse to be 0x00 or 0x01
void write_lfuse_bits(uint8_t lfuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(lfuse,0x2C);
  rw_byte(0x00,0x64);
  rw_byte(0x00,0x6C);
  _delay_ms(3);
  //while(~(PINA & (1 << SDO))) {_delay_us(1);}
}
void write_hfuse_bits(uint8_t hfuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(hfuse,0x2C);
  rw_byte(0x00,0x74);
  rw_byte(0x00,0x7C);
  _delay_ms(3);
  //while(~(PINA & (1 << SDO))) {_delay_us(1);}
}
void write_efuse_bits(uint8_t efuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(efuse,0x2C);
  rw_byte(0x00,0x66);
  rw_byte(0x00,0x6E);
  _delay_ms(3);
  //while(~(PINA & (1 << SDO))) {_delay_us(1);}
}
