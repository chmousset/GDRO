echo "# GDB script automatically connect to STM32F746-DISCO board"
echo "use OpenOCD or st-util to connect to the debugger"

echo "openocd -f interface/stlink.cfg -f target/stm32f7x.cfg"
echo "  or"
echo "st-util"

file build/gdro.elf
target remote :4242

def lrun
	load
	monitor reset init
	continue
end
