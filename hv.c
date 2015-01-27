#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#define SDI PA2
#define SII PA3
#define SDO PA4
#define SCI PA6
#define VCC PA5
#define HV PA1
#define WH(PORT,PIN) PORT |= (1 << PIN)
#define WL(PORT,PIN) PORT &= ~(1 << PIN)
#define HV_WH(PIN) WH(PORTA,PIN)
#define HV_WL(PIN) WL(PORTA,PIN)
//#define D+ PA0
//#define D- PA7

void enter_hv(void)
{
  DDRA |= (1 << SDI)|(1 << SII)|(1 << SDO)|(1 << SCI);
  HV_WL(SDI);
  HV_WL(SII);
  HV_WL(SDO);
  HV_WL(VCC);
  HV_WH(HV);//writing 1 to hv pin turns boost converter off
  HV_WH(VCC);//if vcc rise time is too long, may need to use alternate algorithm
  _delay_us(60);
  HV_WL(HV);//turn on boost converter
  //maye need to do this before 60 us delay or even before turning on vcc depending on rise speed for boost converter
  _delay_us(10);
  //release sdo
  DDRA &= ~(1 << SDO);
  _delay_us(300);
  //give instructions
}
void exit_hv(void)
{
  HV_WH(HV);
  HV_WL(VCC);
}

uint8_t rw_byte(uint8_t sdi, uint8_t sii)
{
  HV_WL(SCI);
  uint8_t i;
  uint8_t data;
  for (i = 0; i < 8; i++)
  {
    HV_WH(SCI);
    data |= (PINA & (1 << SDO)) << (7-SDO-i);
    if (sii & (1 << (7-i))) HV_WH(SII);
    if (sdi & (1 << (7-i))) HV_WH(SDI);
  }
  return data;
}
