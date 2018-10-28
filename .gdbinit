echo "# GDB script automatically connect to STM32F746-DISCO board"
echo "# openocd must be started using the following command:"
echo "  openocd -f interface/stlink.cfg -f target/stm32f7x.cfg"

file build/gdro.elf
target remote :3333

def lrun
	load
	monitor reset init
	continue
end
