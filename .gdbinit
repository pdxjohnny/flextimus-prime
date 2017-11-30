# Disable pagination to not have to press enter
set pagination off
# Load the kernel into GDB
file src/main.elf
# TUI (Text User Interface)
layout src
focus cmd
# Connect to st-util
tar extended-remote :4242
# Load the kernel onto the target
load
# Set breakpoints
break *assert_failed
break *button_irq_handler
break main.c:80
# Print backtrace when a breakpoint is hit
# Re-enable pagination now that we are debugging
command 1
backtrace full
set pagination on
end
# Run the kernel
continue
