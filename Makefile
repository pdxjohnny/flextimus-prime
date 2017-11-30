all:
	+$(MAKE) -C src

gdb: all
	arm-none-eabi-gdb

clean:
	+$(MAKE) -C src clean
