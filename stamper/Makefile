# UISP = avrdude -c avrispmkII -P /dev/ttyUSB0 -p m168 -F
UISP = avrdude -c stk500v1 -b 38400 -P /dev/ttyUSB0 -p m644p -F

COMPILE = avr-g++ -Wall -Os -Iusbdrv -I. -mmcu=atmega644p #-DDEBUG_LEVEL=1

OBJECTS = Stamper.o Pin.o AvrPort.o
# OBJECTS = main.o

# symbolic targets:
all:	main.hex

.cc.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.cc.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(UISP) -U flash:w:main.hex -v

clean:
	rm -f main.bin main.hex main.eep.hex $(OBJECTS)

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
# DO NOT DELETE
