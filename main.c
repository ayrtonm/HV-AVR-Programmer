#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#define WRITE_DEFAULT_FUSE 0

USB_PUBLIC uint8_t usbFunctionSetup(uint8_t data[8])
{
  usbRequest_t *rq = (void *)data;
  switch(rq->bRequest)
  {
    case WRITE_DEFAULT_FUSE:
    {
      write_lfuse_bits(0x62);
      write_hfuse_bits(0xdf);
      write_efuse_bits(0xff);
      return 0;
    }
  }
  //should not get here
  return 0;
}
int main()
{
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
