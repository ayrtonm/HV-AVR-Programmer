//12 MHz with external crystal oscillator to allow V-USB to work
//set fuse bits with the following command
//avrdude -p t84 -U lfuse:w:0x6f:m -U hfuse:w:0xdf:m - Uefuse:w:0xff:m
#define F_CPU 12000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#define RESET_HV PA1
#define SDI PA2
#define SII PA3
#define SDO PA4
#define SCI PA6
#define VCC PA5
//don't need macros for D+ and D- here only for clarification about their purpose
//#define D+ PA0
//#define D- PA7
#define WH(PORT,PIN) PORT |= (1 << PIN)
#define WL(PORT,PIN) PORT &= ~(1 << PIN)
#define HV_WH(PIN) WH(PORTA,PIN)
#define HV_WL(PIN) WL(PORTA,PIN)
#define TOGGLE_CLK HV_WH(SCI);HV_WL(SCI)

void enter_hv(void)
{
  DDRA |= (1 << SDI)|(1 << SII)|(1 << SDO)|(1 << SCI)|(1 << RESET_HV);
  HV_WL(SDI);
  HV_WL(SII);
  HV_WL(SDO);
  HV_WL(VCC);
  HV_WH(RESET_HV);//writing 1 to hv pin turns boost converter off
  HV_WH(VCC);//if vcc rise time is too long, may need to use alternate algorithm
  _delay_us(60);
  HV_WL(RESET_HV);//turn on boost converter
  //maye need to do this before 60 us delay or even before turning on vcc depending on rise speed for boost converter
  _delay_us(10);
  //release sdo
  DDRA &= ~(1 << SDO);
  _delay_us(300);
  //give instructions
}
void exit_hv(void)
{
  HV_WH(RESET_HV);
  HV_WL(VCC);
}

uint8_t rw_byte(uint8_t sdi, uint8_t sii)
{
  //commands are 11 bits with the following format
  // 0_xxxx_xxxx_00
  HV_WL(SDI);
  HV_WL(SII);
  TOGGLE_CLK;
  uint8_t i;
  uint8_t data;
  for (i = 0; i < 8; i++)
  {
    TOGGLE_CLK;
    data |= (PINA & (1 << SDO)) << (7-SDO-i);
    if (sii & (1 << (7-i))) HV_WH(SII);
    if (sdi & (1 << (7-i))) HV_WH(SDI);
  }
  HV_WL(SDI);
  HV_WL(SII);
  TOGGLE_CLK;
  TOGGLE_CLK;
  return data;
}

//host software will take care of warning people from writing fuse bits incorrectly masking efuse to be 0x00 or 0x01
void write_lfuse_bits(uint8_t lfuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(lfuse,0x2C);
  rw_byte(0x00,0x64);
  rw_byte(0x00,0x6C);
}
void write_hfuse_bits(uint8_t hfuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(hfuse,0x2C);
  rw_byte(0x00,0x74);
  rw_byte(0x00,0x7C);
}
void write_efuse_bits(uint8_t efuse)
{
  rw_byte(0x40,0x4C);
  rw_byte(efuse,0x2C);
  rw_byte(0x00,0x66);
  rw_byte(0x00,0x6E);
}
