/* Linker script for stm32f042 */
MEMORY
{
  flash   : org = 0x08000000, len = 32k
  ram     : org = 0x20000000, len = 6k
}

SECTIONS
{
	. = ORIGIN(flash);
  /* Interrupt Vector Table */
  .text : {
    *(.vectors);
    *(.text);
  } > flash
	. = ORIGIN(ram);
  .data : {
    INIT_DATA_VALUES = LOADADDR(.data);
    INIT_DATA_START = .;
    *(.data);
    INIT_DATA_END = .;
  } > ram AT > flash
	BSS_START = .;
	.bss : {
	  *(.bss);
	} > ram
	BSS_END = .;
}
