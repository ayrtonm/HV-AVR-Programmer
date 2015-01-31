DEVICE     = attiny84
CLOCK      = 12000000
PROGRAMMER = -c usbtiny
OBJECTS    = main.o hv.o usbdrv/usbdrv.o usbdrv/oddebug.o usbdrv/usbdrvasm.o
FUSES      = -u -U lfuse:w:0x6f:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
 
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -O3 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
 
all:	main.hex
 
.c.o:
	$(COMPILE) -c $< -o $@
 
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
 
.c.s:
	$(COMPILE) -S $< -o $@
 
flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i
 
fuse:
	$(AVRDUDE) $(FUSES)
 
install: flash fuse
 
clean:
	rm -f main.hex main.elf $(OBJECTS)
 
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)
 
main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
 
disasm:	main.elf
	avr-objdump -d main.elf
 
cpp:
	$(COMPILE) -E main.c

