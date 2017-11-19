# IMPORTANT!!!
# You must configure the LIBSPEC variable for your own system.  The setting below reflects
# where the Cortex M0 libraries from gcc are installed on *my* system.  *Your* system
# will be different
# Tell the linker where to find the libraries -> important: use thumb versions
LIBSPEC=-L /usr/lib/gcc/arm-none-eabi/5.4.1/armv6-m

# Specify the compiler to use
CC=arm-none-eabi-gcc
# Specify the assembler to use
AS=arm-none-eabi-as
# Specity the linker to use
LD=arm-none-eabi-ld

BOARD=STM32F042

CCFLAGS=-mcpu=cortex-m0 -mthumb -g -D$(BOARD) -I ../include
CCFLAGS+= -I libs/Libraries/CMSIS/Device/ST/STM32F0xx/Include
CCFLAGS+= -I libs/Libraries/CMSIS/Include
CCFLAGS+= -I libs/Libraries/STM32F0xx_StdPeriph_Driver/inc

#$(wildcard libs/Libraries/STM32F0xx_StdPeriph_Driver/src/*.c)
# List the object files involved in this project
SRCS=$(wildcard *.c) \
		 libs/Libraries/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_rtc.c \
		 libs/Libraries/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_rcc.c \
		 libs/Libraries/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_adc.c \
		 libs/Libraries/STM32F0xx_StdPeriph_Driver/src/stm32f0xx_gpio.c #\
		# hd44780.c
OBJS=$(SRCS:.c=.o)

all: main.elf

# The default 'target' (output) is main.elf and it depends on the object files being there.
# These object files are linked together to create main.elf
main.elf : $(OBJS)
	$(LD) $(OBJS) $(LIBSPEC) -lgcc -T linker_script.ld --cref -Map main.map -nostartfiles -o main.elf
# convert binary elf file to hex to help the stm32flash utility
	objcopy -O ihex main.elf main.hex
# The object file main.o depends on main.c.  main.c is compiled to make main.o
%.o:%.c
	$(CC) $(CCFLAGS) -o $@ -c $<

# if someone types in 'make clean' then remove all object files and executables
# associated wit this project
clean:
	rm -f $(OBJS) *.map *.hex main.elf
