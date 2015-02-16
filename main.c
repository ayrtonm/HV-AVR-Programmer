#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "hv.h"
#include "usbdrv/usbdrv.h"
#define F_CPU 12000000
#define WRITE_DEFAULT_FUSE 0

// Called by V-USB after device reset
void hadUsbReset()
{
}

USB_PUBLIC uint8_t usbFunctionSetup(uint8_t data[8])
{
  usbRequest_t *rq = (void *)data;
  switch(rq->bRequest)
  {
    case WRITE_DEFAULT_FUSE:
    {
      write_lfuse_bits((~0x62) & 0xff);
      write_hfuse_bits((~0xdf) & 0xff);
      write_efuse_bits((~0xff) & 0x01);
      return 0;
    }
  }
  //should not get here
  return 0;
}
int main()
{
  //comment to turn on boost converter
  DDRA = (1 << PA1);
  PORTA = (1 << PA1);
  uint8_t i;
  wdt_enable(WDTO_1S);
  usbInit();
  usbDeviceDisconnect();
  for(i = 0; i < 250; i++)
  {
    wdt_reset();
    _delay_ms(2);
  }
  usbDeviceConnect();
  sei();
  while(1)
  {
    wdt_reset();
    usbPoll();
  }
  return 0;
}
