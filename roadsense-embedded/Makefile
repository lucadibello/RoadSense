BOARD ?= arduino:mbed_portenta:envie_m7

list-boards:
	@arduino-cli board list

info-board:
	@arduino-cli board details -b ${BOARD}

install:
	@arduino-cli core update-index
	@arduino-cli core install arduino:mbed_portenta

