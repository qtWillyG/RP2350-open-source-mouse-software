# RP2350 Custom USB HID Mouse

A USB HID mouse for the Raspberry Pi Pico 2 (RP2350) built with the
[Adafruit TinyUSB](https://github.com/adafruit/Adafruit_TinyUSB_Arduino) library.

It demonstrates how to override the default USB descriptors (Vendor ID,
Product ID, Manufacturer and Product strings) and switch between multiple
device-identity profiles on boot, plus basic HID mouse movement and clicks.

## Features

- 🪪 **Custom USB descriptors** — set your own VID/PID and Manufacturer/Product strings
- 🗂️ **Profile table** — define multiple device identities in a struct array and pick one on boot
- 🔀 **Boot-time selection** — choose a profile by constant, or from a DIP switch / jumper on GPIO
- 🖱️ **HID mouse basics** — cursor movement and left-click to verify enumeration works

## Hardware

- Raspberry Pi Pico 2 (RP2350)
- USB cable
- *(optional)* a DIP switch / jumpers on GP14 / GP15 for hardware profile selection

## Build & Flash

1. Install the **Arduino IDE** and the
   [Earle Philhower arduino-pico](https://github.com/earlephilhower/arduino-pico) core.
2. Install the **Adafruit TinyUSB Library** via the Library Manager.
3. In **Tools**:
   - **Board:** `Raspberry Pi Pico 2`
   - **USB Stack:** `Adafruit TinyUSB`
4. **Sketch → Export Compiled Binary** to produce a `.uf2`.
5. Hold **BOOTSEL**, plug in the Pico 2, and drag the `.uf2` onto the `RP2350` drive.

## Configuration

Edit the `profiles[]` table to define your own device identities:

```cpp

Notes
Once flashed, the board enumerates as an HID mouse, so re-flashing usually
needs the BOOTSEL button.
Descriptor overrides must be applied before USB enumeration — the sketch
handles re-enumeration automatically on the RP2350 core.
  // ...
};
