CC = avr-gcc $(CPP_FLAGS)
LD = avr-gcc $(LD_FLAGS)
AR = avr-ar $(AR_FLAGS)
OBJCP = avr-objcopy
SIZE = avr-size -A --mcu=$(MCU)
AVRDUDE = avrdude

LIBRARIES = 
OBJECTS = src/main.o
INCLUDES = -I"/usr/lib/avr/include/avr/" -I"src"
DEFINES = -DF_CPU=$(F_CPU)L

CPP_FLAGS = -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) $(DEFINES) $(INCLUDES)
LD_FLAGS = -Os -Wl,--gc-sections -mmcu=$(MCU)
AR_FLAGS = rcs

PORT = /dev/ttyACM0 
MCU = atmega328p
F_CPU = 16000000
FORMAT = ihex
UPLOAD_RATE = 115200 
MAX_SIZE = 30720

OUTPUT = nurdpanel

all: $(LIBRARIES) $(OUTPUT).hex

src/main.o: src/main.cpp
	$(CC) -c src/main.cpp -o src/main.o

$(OUTPUT).elf: $(OBJECTS) $(LIBRARIES)
	$(LD) $(OBJECTS) $(LIBRARIES) -lm -o $(OUTPUT).elf

$(OUTPUT).hex: $(OUTPUT).elf
	$(OBJCP) -O ihex -R .eeprom $(OUTPUT).elf $(OUTPUT).hex
  
upload: all
	$(AVRDUDE) -p$(MCU) -carduino -P$(PORT) -b$(UPLOAD_RATE) -D -Uflash:w:$(OUTPUT).hex:i 

clean:
	@rm -f $(LIBRARIES) $(OUTPUT).elf $(OUTPUT).hex $(shell find . -follow -name "*.o")
  
