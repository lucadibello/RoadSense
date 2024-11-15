BOARD ?= arduino:mbed_portenta:envie_m7
PORT ?= cu.usbmodem2101

compile:
	@arduino-cli compile --fqbn $(BOARD) .
	@echo "[!] Compilation done!"

upload:
	@arduino-cli upload --fqbn $(BOARD) --port $(PORT) .
	@echo "[!] Upload done!"

all: compile upload
	@echo "All done!"
