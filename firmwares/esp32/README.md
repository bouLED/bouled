# ESP32 firmware

## Setup

Make sure bouLED's repository has been cloned using
`git clone --recurse-submodules`, or run
`git submodule update --init --recursive`.

Follow Expressif's [Get Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#)
guide. You're going to need the `esp32-idf` path in `IDF_PATH`,
the Xtensa toolchain in `PATH`, and `$IDF_PATH/tools` in `PATH`
too (for the `idf.py` tool).

## Compiling/flashing

* `idf.py build`
* `idf.py flash` (calls build automatically if needed)
* `idf.py monitor`
* `idf.py help`
