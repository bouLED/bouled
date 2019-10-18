# SPI tester

This only works on STM32F74G-DISCO boards. So far, this sets the 6 SPIs
up in simplex mode, at around 421Kbits/s, and displays a small animation
on the connected triangles. Not all SPIs are accessible, but that's enough
to benchmark SPI speeds.

So far, the SPIs run in blocking mode but they will eventually be non-blocking.

# Pinout

| Pin (on the Arduino header) | Triangle pin
|-----------------------------|------------------
| A2                          | SDI
| A4                          | CKI

# Running

`make startgdbserver` starts the J-Link GDB Server. Then, in another terminal, run
`make debug` to compile, open GDB and flash the board.

# Project structure

```text
.
├── Drivers
│   └── ...      # HAL stuff
├── Makefile
├── Middlewares
│   └── FreeRTOS
├── README.md
├── custom.mk
└── src
    ├── ...      # Our code
    ├── mx/      # Peripheral setup
    ├── rtt/     # J-Link RTT stuff
    └── startup/ # Linker script and startup code
```

This is based on what STM32CubeMX generated, but I changed the project structure.
