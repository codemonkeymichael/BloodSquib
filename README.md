# BloodSquib - DMX Trigger Controller

A DMX Core Pi Pico 2-based DMX controller that triggers digital outputs based on DMX channel values. Designed for special effects applications like blood squibs or other pyrotechnic triggers.

## Features

- **DMX Input**: Reads DMX512 signals from a wireless or wired DMX controller
- **Digital Outputs**: Controls GPIO pins with 500ms pulse outputs
- **Edge Detection**: Triggers only on rising edges (when DMX channel goes from ≤50 to >50)
- **Status LEDs**: Visual indicators for DMX signal reception and system status

## DMX Channel Mapping

- **DMX Channel 1** → **IO 2** (GPIO 16) - Triggers when channel value > 50
- **DMX Channel 2** → **IO 3** (GPIO 17) - Triggers when channel value > 50

## Hardware Requirements

- **DMX Core Pi Pico 2** board (Raspberry Pi Pico 2 based)
- DMX input circuit (wireless DMX receiver or DMX-to-serial converter)
- GPIO pins 16 and 17 connected to your trigger devices

## Pin Configuration

- **DMX Input**: GPIO 7 (DMX data input)
- **DMX Enable**: GPIO 3 (DMX receiver enable)
- **IO 2 Output**: GPIO 16 (DMX Channel 1 trigger)
- **IO 3 Output**: GPIO 17 (DMX Channel 2 trigger)
- **Status LED**: GPIO 24 (DMX port A indicator)
- **Indicator LED**: GPIO 25 (System status)

## Building the Project

### Prerequisites

- CMake (3.13 or later)
- Raspberry Pi Pico SDK
- C/C++ compiler (GCC)

### Build Steps

1. Clone or download this repository
2. Ensure Pico SDK is installed and `PICO_SDK_PATH` environment variable is set
3. Open a terminal in the project directory
4. Run the build script:

```bash
.\build.bat
```

Or manually:

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Deploying to Raspberry Pi Pico

1. **Put your DMX Core Pi Pico 2 into bootloader mode**:
   - Hold the BOOTSEL button while plugging in the board
   - Or press the reset button if your board has one

2. **The board will appear as a USB drive** called "RPI-RP2"

3. **Flash the firmware**:
   - Drag and drop the `build\bloodsquib.uf2` file to the RPI-RP2 drive

4. **The board will automatically reboot** and start running the DMX controller

## Usage

1. Connect your DMX input to GPIO 7
2. Connect your trigger devices to GPIO 16 (IO 2) and GPIO 17 (IO 3)
3. Power on the Pico
4. Send DMX signals to channels 1 and 2 with values > 50 to trigger the outputs

## Troubleshooting

- **No DMX signal detected**: Check DMX wiring and ensure DMX enable pin (GPIO 3) is properly configured
- **Outputs not triggering**: Verify GPIO connections and check DMX channel values
- **Build errors**: Ensure Pico SDK is properly installed and environment variables are set

## Technical Details

- **DMX Protocol**: DMX512 at 250kbps
- **Pulse Duration**: 500ms (0.5 seconds)
- **Trigger Threshold**: DMX value > 50 (out of 255)
- **Edge Detection**: Rising edge only (prevents continuous triggering)

## License

This project is open source. Please check individual source files for license information.
