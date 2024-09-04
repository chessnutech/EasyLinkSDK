[![easylink](https://github.com/miguno/EasyLinkSDK/actions/workflows/build.yml/badge.svg)](https://github.com/miguno/EasyLinkSDK/actions/workflows/build.yml)

# What's EasyLinkSDK?

This is the C/C++ SDK for [Chessnut](https://www.chessnutech.com/) electronic
chess computers, such as the Chessnut Air.

## Licensing

Licensed under [The MIT License (MIT)](LICENSE).

## Features

- Get the position of the pieces on the chessboard in real-time
- Control the display of the LED lights on the chessboard
- Make a beeping sound
- Query the version of the hardware
- Query the battery level
- Get game recordings from internal storage

## Example Usage

### Supported build commands (via `just`)

Requires [just](https://github.com/casey/just). See [justfile](justfile).

```shell
$ just
Available recipes:
    build                 # build for Debug
    clang-details         # print clang details, including environment and architecture
    clangd-check filename # run a clangd check on a single file
    clean                 # clean
    configure             # configure a build
    coverage              # generate code coverage report
    default               # print available targets
    do                    # clean, compile, build for Debug
    docs                  # generated documentation (requires Doxygen)
    format                # format source code (.c and .h files) with clang-format
    just-vars             # evaluate and print all just variables
    release               # build for Release
    run *args             # run a Debug binary
    run-release *args     # run a Release binary
    system-info           # print system information such as OS and architecture
    tidy                  # run clang-tidy (see .clang-tidy)
    tidy-checks           # show configured checks of clang-tidy
    tidy-config           # show effective configuration of clang-tidy
    tidy-verify-config    # verify configuration of clang-tidy
```

### Application Example

An example application is available at [src/main.c](src/main.c).

### Connect to and disconnect from the chessboard

- Call `cl_connect()` to connect to the chessboard.
- Call `cl_disconnect()` to disconnect from the chessboard.

```c
#include <stdio.h>
#include <stdlib.h>
#include "easy_link_c.h"

int main(void) {
  // Connect to the chessboard device with HID.  If the device is not
  // connected, it will automatically connect when the device is plugged into
  // the computer.
  if (cl_connect() == 1) {
    printf("Successfully connected to chessboard\n");
  } else {
    printf("[ERROR] Failed to connect to chessboard.  Exiting ...\n");
    return EXIT_FAILURE;
  }

  // Disconnect from the board.
  cl_disconnect();
  return EXIT_SUCCESS;
}
```

### Get the position of the pieces on the chessboard in real-time

- Call `cl_connect()` to connect to the chessboard.
- Create a callback function whose parameter `const char *fen` will be
  populated with a FEN string
  ([Forsyth-Edwards Notation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation))
  that describes the board position at the time of the callback, and the
  parameter `int len` will represent the size of this FEN string.
- Register the callback function through the function
  `cl_set_readtime_callback(callback)`.
  - You can disable/unregister the callback function by registering a NULL
    pointer as the callback.
- Finally, switch the chessboard mode to real-time mode with
  `cl_switch_real_time_mode()`. Setting the real-time mode is required,
  otherwise you cannot obtain the FEN of the chessboard's current state.

```c
#include <stdio.h>
#include "easy_link_c.h"

void callback(const char *fen, int len) {
  printf("Board position in FEN: %.*s\n", len, fen);
}

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  cl_set_readtime_callback(callback);
  cl_switch_real_time_mode();
  for (;;) {
    getchar();
  }

  cl_disconnect();
}
```

### Chessboard LEDs

- Call `cl_connect()` to connect to the chessboard.
- Create a `char **led` array of 8 strings with 8 chars each, representing the
  LED states of the 8x8 chessboard.
  - The first string represents row `8`, the last string is row `1` on the
    board.
  - The first character in a string is colum `a`, the last character is
    column `h`.
  - Char `'1'` in a string means enable the LED of the associated square.
    Char `'0'` means disable the LED.
- Set the LED state of the chessboard via `cl_led(const char *leds[8])`.

Illustration for `const char *leds[8]`:

```
   abcdefgh
  8........8  leds[0]
  7........7  leds[1]
  6........6  leds[2]
  5........5  leds[3]
  4..x.....4  leds[4]
  3........3  leds[5]
  2........2  leds[6]
  1........1  leds[7]
   abcdefgh
```

For example, to enable the LED for square `c4` only ('x' above), use:

```c
{ "00000000",
  "00000000",
  "00000000",
  "00000000",
  "00100000", // <-- here
  "00000000",
  "00000000",
  "00000000" };
```

Full example:

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  const char *leds[8] = { "11111111",
                          "00000000",
                          "11111111",
                          "00000000",
                          "11111111",
                          "00000000",
                          "11111111",
                          "00000000" };
  if (cl_led(leds) == 0) {
    printf("[ERROR] Could not enable/disable LEDs\n");
  }

  cl_disconnect();
}
```

### Buzzer (beep)

- Call `cl_connect()` to connect to the chessboard.
- Call `cl_beep(unsigned short frequencyHz, unsigned short durationMs)`.
  The parameters set the frequency (in Hz) and duration (in milliseconds) of
  the buzzer, respectively.

```c
#include "easy_link_c.h"

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  if (cl_beep(1000, 200) == 0) {
    printf("[ERROR] failed to beep\n");
  }

  cl_disconnect();
}
```

### Query the version of the hardware

- Call `cl_connect()` to connect to the chessboard.
- Query the available versions of the hardware by providing a string (`char*`)
  into which the information will be stored.
  - `cl_version(char *version)`: version of the SDK library;
    the `version` parameter should have a length of at least 20
  - `cl_get_mcu_version(char *version)`: version of the MCU hardware;
    the `version` parameter should have a length of at least 100
  - `cl_get_ble_version(char *version)`: version of the BLE hardware
    (Bluetooth Low Energy);
    the `version` parameter should have a length of at least 100
- All three functions return the length of the actual data written to the
  passed parameter, or `0` if the function call failed.

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  char sdk_version[20];
  const size_t sdk_version_length = cl_version(sdk_version);
  if (sdk_version_length > 0) {
    printf("[DEBUG] SDK version: %.*s\n", (int)sdk_version_length, sdk_version);
    // Example output:
    // SDK version: 1.0.0
  } else {
    printf("[ERROR] Could not get SDK version\n");
  }

  char mcu_version[100_mcu_version_length];
  const size_t mcu_version_length = cl_get_mcu_version(mcu_version);
  if (mcu_version_length > 0) {
    printf("MCU hardware version: %.*s\n", (int)mcu_version_length, mcu_version);
    // Example output:
    // MCU hardware version: CN_DVT9_210659
  } else {
    printf("[ERROR] Could not query MCU hardware version\n");
  }

  char ble_version[100_ble_version_length];
  const size_t ble_version_length = cl_get_ble_version(ble_version);
  if (ble_version_length > 0) {
    printf("BLE hardware version: %.*s\n", (int)ble_version_length, ble_version);
    // Example output:
    // BLE hardware version: CNCA100_V201
  } else {
    printf("[ERROR] Could not query BLE hardware version\n");
  }

  cl_disconnect();
}
```

### Query the battery level

- Call `cl_connect()` to connect to the chess board.
- Query the battery level with `cl_get_battery()`. Note that the battery level
  is only an estimate that is not always accurate.

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  const int battery_level = cl_get_battery();
  if (battery_level >= 0) {
    printf("Battery level: %d%%\n", battery_level);
    // Example output:
    // Battery level: 67%
  } else {
    printf("[ERROR] Could not query the battery level\n");
  }

  cl_disconnect();
}
```

### Get game recordings from internal storage

The chess board can store replays of played matches in its internal storage.
The file format is a sequence of FEN strings, separated by `;`. It is
recommended to validate the FEN strings after retrieval, as the board will,
for example, faithfully record board positions verbatim, even illegal ones.

Example game file content:

    rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR;rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR;rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR

To retrieve recorded game files from internal storage:

- Call `cl_connect()` to connect to the chess board.
- It is recommended to query the number of available game files first via
  `cl_get_file_count()` before attempting to retrieve any files.
- CAUTION: Retrieve the next available game file with
  `cl_get_file_and_delete(char* game_data, size_t len)` _and then_
  _automatically delete the game file from the chessboard's internal storage_.
  - **DANGER: If the size of `char *game_data` is too small to fully store
    the game file, then the game file is still irrevocably deleted from the
    chessboard's internal storage!**
  - Use the safer variant `cl_get_file_and_keep()` to perform a dry-run of
    file retrieval first (e.g., to verify that the provided `char *game_data`
    is sufficiently large), and only if this succeeds do you call
    `cl_get_file_and_delete()`.
  - Unfortunately, the SDK does not allow you to retrieve specific game files
    or iterate through them without deletion. The only way to "advance the
    cursor" and iterate through available game files is by repeatedly calling
    `cl_get_file_and_delete()` or its alias `cl_get_file()`, both of which
    will automatically delete the current game file after retrieval; the next
    call will then operate on the next game file.
- Calling this function will automatically set the board's mode to file upload
  mode.

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect(); // we skip error handling here for the sake of brevity

  const int file_count = cl_get_file_count();
  if (file_count >= 0) {
    printf("Stored game files: %d\n", file_count);
    if (file_count > 0) {
      // You must ensure that the retrieved game file fits into `game_content`,
      // otherwise retrieval will fail and the respective game fail is lost!
      char game_content[1024 * 10];
      const int game_file_len = cl_get_file_and_delete(game_content, sizeof(game_content));
      if (game_file_len > 0) {
        printf("Game file content: %.*s\n", game_file_len, game_content);
        //
        // Example output:
        // Game file content: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR;rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR;rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR
        //
      } else if (game_file_len == 0) {
        printf("[WARNING] The game file is empty\n");
      } else {
        // When the provided `game_content` parameter was too small
        printf("[ERROR] Failed to retrieve game file, which is now irrevocably lost\n");
      }
    }
  } else {
    printf("[ERROR] Could not retrieve number of stored game files");
  }

  cl_disconnect();
}
```

## How to build

Supported platforms:

- Windows
- Linux
- Mac

Dependencies:

- C++11
- [CMake](https://cmake.org) 3.20+ (required)
- [Ninja](https://ninja-build.org/) (required)
- [hidapi](https://github.com/libusb/hidapi) (internal)
- [spdlog](https://github.com/gabime/spdlog) (internal)

Further details are provided below.

### Microsoft Windows

#### Option 1: Visual Studio 2022

- Clone this project's git repository. (In "normal" Windows, not in WSL/Linux.)
- Use Visual Studio 2022 (Community Edition) and open the project directory.
- Visual Studio should automatically configure the project via its built-in
  cmake.
- Run `Build > Build All` to compile the project, including the EasyLinkSDK DLL
  `easylink.dll` and the main application `main.exe` that depends on the
  EasyLinkSDK DLL.
- Once the compilation is completed, open a terminal in PowerShell (or
  `cmd.exe`) and run:

  ```shell
  # 1. Make easylink.ddl available to main.exe
  $ cp out\build\x64-Debug\sdk\easylink.dll .\out\build\x64-Debug\src

  # 2. Run main.exe
  $ out\build\x64-Debug\src\main.exe
  ```

#### Option 2: Windows setup when not using Visual Studio 2022 (work in progress)

> TODO: Automate and verify the Windows build setup.
> In the meantime, see the notes below as well as the
> [GitHub workflow configuration](.github/workflows/build.yml), which uses
> Windows.

Install dependencies (e.g., with [choco](https://chocolatey.org/)):

```shell
# clang toolchain
choco install -y cmake llvm ninja
choco install -y doxygen.install # optional, for generating documentation
# https://github.com/casey/just (a command runner)
choco install -y just # optional, but convenient
```

Then run the same configure and compile steps as described in the Linux/macOS
section below.

### Linux and macOS

use with gnu/linux

In order to use EasyLink as a user in the wheel group 
( group can be arbitrary )
You must give the user read and write permissions for the Chessnut air.
This can be done through a udev rule.

create a 99-chessnutair.rules file: /etc/udev/rules.d/99-chessnutair.rules,
with the following:

SUBSYSTEM=="usb", ATTRS{idVendor}:="0x2d80", /
ATTRS{idProduct}:="0x8002", GROUP="wheel", MODE="0660"

#### set the permissions for device files
KERNEL=="hidraw2", GROUP="wheel", MODE="0660"

======== end =========

Currently supported USB Vender ID and Product ID

Vender ID: 0x2d80
Product IDs for different models:

Air : 0x80**
Pro : 0x81**
Air+: 0x82**
Evo: 0x83**
Go: 0x85**



Install dependencies:

```shell
### Debian/Ubuntu (EXPERIMENTAL: project compiles, but can't connect to chessboard)
# clang toolchain
sudo apt-get install -y build-essential clang clang-tidy cmake lldb ninja-build
sudo apt-get install -y doxygen  # optional, for generating documentation
# Dependencies for EasyLinkSDK
sudo apt install libudev-dev libusb-dev libusb-1.0-0-dev

### macOS (EXPERIMENTAL: project compiles, but can't connect to chessboard)
# clang toolchain
brew install cmake llvm ninja
brew install doxygen  # optional, for generating documentation
```

Ensure that the build setup uses clang as defined in [.env](.env):

```
# .env
CC=clang
CXX=clang++
```

Run the build with `just` (or use `cmake` directly):

```shell
# Option 1: You have just (https://github.com/casey/just) installed
$ just do

# Option 2: Use cmake directly
#
# 1. Configure
$ cmake -B build/ -S . -G "Ninja Multi-Config" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
# 2. Compile (pick one of the two)
$ cmake --build build/ --config Debug   --target all  # for a Debug build
$ cmake --build build/ --config Release --target all  # for a Release build
```

If compilation succeeded, you can now run the main application that will
attempt to connect to your chessboard.

1. Turn the chessboard on.
2. Connect your computer directly to the board via a wired USB cable
   - Unfortunately, a wireless connection via BLE (Bluetooth Low Energy) seems
     not to work yet?
3. Execute `just run` (or directly `build/src/Debug/main`) to run the main
   application. See example below.

If the connection to the chessboard is successful, you will see output similar
to:

```shell
# Or, from the top-level project directory:
#
#    $ build/src/Debug/main   (if you build the Debug version)
#    $ build/src/Release/main (if you build the Release version)
#
$ just run
[DEBUG] SDK version: 1.0.0
[DEBUG] Connecting to chessboard via HID...
Successfully connected to chessboard
MCU hardware version: CN_DVT9_210659
BLE hardware version: CNCA100_V201
Battery level: 100%
[DEBUG] Disconnecting from chessboard
$
```
