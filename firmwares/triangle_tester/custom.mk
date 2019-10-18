ELF=build/$(TARGET).elf
GDB=$(PREFIX)gdb

startgdbserver:
	@pidof JLinkGDBServer > /dev/null || JLinkGDBServer -if swd -speed auto -device STM32F746NG -endian little

stopgdbserver:
	@pidof JLinkGDBServer > /dev/null && killall JLinkGDBServer || true

debug: $(ELF) startgdbserver
	$(GDB) $(ELF)

.PHONY: startgdbserver stopgdbserver debug
