# Arduino 0015 Makefile
# Arduino adaptation by mellis, eighthave, oli.keller
# Modified by Johannes Hoff <johannes@johanneshoff.com> to
# work on linux command line and sort out dependencies.
#
# This makefile allows you to build sketches from the command line
# without the Arduino environment (or Java).
#
# Detailed instructions for using the makefile:
#
#  1. Copy this file into the folder with your sketch. There should be a
#     file with the same name as the folder and with the extension .pde
#     (e.g. foo.pde in the foo/ folder).
#
#  2. Modify the line containg "INSTALL_DIR" to point to the directory that
#     contains the Arduino installation (for example, under Mac OS X, this
#     might be /Applications/arduino-0015).
#
#  3. Modify the line containing "PORT" to refer to the filename
#     representing the USB or serial connection to your Arduino board
#     (e.g. PORT = /dev/tty.USB0).  If the exact name of this file
#     changes, you can use * as a wildcard (e.g. PORT = /dev/tty.usb*).
#
#  4. Set the line containing "MCU" to match your board's processor.
#     Older one's are atmega8 based, newer ones like Arduino Mini, Bluetooth
#     or Diecimila have the atmega168.  If you're using a LilyPad Arduino,
#     change F_CPU to 8000000.
#
#  5. At the command line, change to the directory containing your
#     program's file and the makefile.
#
#  6. Type "make" and press enter to compile/verify your program.
#
#  7. Type "make upload", reset your Arduino board, and press enter to
#     upload your program to the Arduino board.

TARGET = $(notdir $(CURDIR))
INSTALL_DIR = /usr/share/arduino
PORT = usb
UPLOAD_RATE = 19200
MCU = atmega1280
F_CPU = 8000000UL


############################################################################
# Below here nothing should be changed...

ARDUINO = $(INSTALL_DIR)/hardware/arduino/cores/arduino
ARDUINO_VARIANT = $(INSTALL_DIR)/hardware/arduino/variants/mega

SRC_DIRS = . 
SRC_DIRS += ./drivers 
SRC_DIRS += ./applications
SRC_DIRS += $(wildcard ./applications/*)
SRC_DIRS += $(wildcard ./controller/*)
SRC_DIRS += $(wildcard ./libraries/*) 
SRC_DIRS += $(ARDUINO)
SRC_DIRS += $(ARDUINO_VARIANT)

SRC = $(wildcard $(SRC_DIRS:%=%/*.c))
CXXSRC = $(wildcard $(SRC_DIRS:%=%/*.cpp))

FORMAT = ihex

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Place -I options here
CINCS = -I$(ARDUINO) -I$(ARDUINO_VARIANT)
CXXINCS = -I$(ARDUINO) -I$(ARDUINO_VARIANT)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CMINE = -ffunction-sections -fdata-sections -ffreestanding
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA) $(MINE)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT) $(MINE)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
LDFLAGS = -lm


# Programming support using avrdude. Settings and variables.

AVRDUDE_PROGRAMMER = jtagmkii
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:bin/$(TARGET).hex
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AR = avr-ar
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f


# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build sizeafter

build: elf hex

# Here is the "preprocessing".
# It creates a .cpp file based with the same name as the .pde file.
# On top of the new .cpp file comes the WProgram.h header.
# At the end there is a generic main() function attached.
# Then the .cpp file will be compiled. Errors during compile will
# refer to this new, automatically generated, file.
# Not the original .pde file you actually edit...
bin/$(TARGET).cpp: $(TARGET).cpp
	test -d bin || mkdir bin
	cat $(TARGET).cpp > bin/$(TARGET).cpp

elf: bin/$(TARGET).elf
hex: bin/$(TARGET).hex
eep: bin/$(TARGET).eep
lss: bin/$(TARGET).lss
sym: bin/$(TARGET).sym

# Program the device.
upload: bin/$(TARGET).hex
	# pulsedtr.py $(PORT) # Will not need this due to my programmer
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) bin/$(TARGET).hex
ELFSIZE = $(SIZE)  bin/$(TARGET).elf
sizebefore:
	@if [ -f bin/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:
	@if [ -f bin/$(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(HEXSIZE); echo; fi


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: bin/$(TARGET).elf
	$(COFFCONVERT) -O coff-avr bin/$(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr bin/$(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

# Link: create ELF output file from library.
bin/$(TARGET).elf: bin/$(TARGET).cpp bin/core.a
	$(CC) $(ALL_CFLAGS) -o $@ bin/$(TARGET).cpp -L. bin/core.a $(LDFLAGS)

bin/core.a: $(OBJ)
	@for i in $(OBJ); do echo $(AR) rcs bin/core.a $$i; $(AR) rcs bin/core.a $$i; done



# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
	$(REMOVE) bin/$(TARGET).hex bin/$(TARGET).eep bin/$(TARGET).cof bin/$(TARGET).elf \
	bin/$(TARGET).map bin/$(TARGET).sym bin/$(TARGET).lss bin/core.a \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend sizebefore sizeafter
