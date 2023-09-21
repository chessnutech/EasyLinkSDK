[![easylink](https://github.com/miguno/EasyLinkSDK/actions/workflows/build.yml/badge.svg)](https://github.com/miguno/EasyLinkSDK/actions/workflows/build.yml)

**⚠️ This is a community fork of the [official EasyLinkSDK](https://github.com/chessnutech/EasyLinkSDK). ⚠️**

# What's EasyLinkSDK?

This is the C/C++ SDK for [Chessnut](https://www.chessnutech.com/) electronic
chess computers, such as the Chessnut Air.

## Features

- Get the position of the pieces on the chessboard in real-time
- Control the display of the LED lights on the chessboard
- Control the buzzer to sound
- Query the version of the hardware
- Query battery level
- Get game data for offline play

## Example Usage

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
  if (cl_connect()) {
    printf("Successfully connected to chessboard\n");
  } else {
    printf("ERROR: Failed to connect to chessboard.  Exiting ...\n");
    return EXIT_FAILURE;
  }

  // Disconnect from the board.
  // Unfortunately, the function does not return success or failure information.
  cl_disconnect();
  return EXIT_SUCCESS;
}
```

### Get the position of the pieces on the chessboard in real-time

- Call `cl_connect()` to connect to the chessboard.
- Create a callback function whose parameter is a FEN string representing the current state of the board
- Set the callback function through the function `cl_set_readtime_callback(callback)`
- Actively switch the chessboard state to real-time mode `cl_switch_real_time_mode()`, only in real-time mode can the chessboard FEN be obtained

```c
#include <stdio.h>
#include "easy_link_c.h"

void realtime_func(const char *data, int len) { printf("%s", data); }

int main(void) {
  cl_connect();
  cl_set_readtime_callback(realtime_func);
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
- Set the LED state of the chessboard via `cl_led(const char *data[8])`.

Illustration for `const char *led[8]`:

```
   abcdefgh
  8........8  led[0]
  7........7  led[1]
  6........6  led[2]
  5........5  led[3]
  4..x.....4  led[4]
  3........3  led[5]
  2........2  led[6]
  1........1  led[7]
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
#include "easy_link_c.h"

int main(void) {
  cl_connect();
  const char *led[8] = { "11111111",
                         "00000000",
                         "11111111",
                         "00000000",
                         "11111111",
                         "00000000",
                         "11111111",
                         "00000000" };
  cl_led(led);
  cl_disconnect();
}
```

### Buzzer (beep)

- Call `cl_connect()` to connect to the chessboard.
- Call `cl_beep(unsigned short frequency, unsigned short duration)`.
  The parameters set the frequency and duration of the buzzer, respectively.

```c
#include "easy_link_c.h"

int main(void) {
  cl_connect();
  cl_beep(1000, 200);
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
  cl_connect();

  char sdk_version[20];
  cl_version(sdk_version);
  printf("%s\n", sdk_version);

  char mcu_version[100];
  cl_get_mcu_version(mcu_version);
  printf("%s\n", mcu_version);

  char ble_version[100];
  cl_get_ble_version(ble_version);
  printf("%s\n", ble_version);
  cl_disconnect();
}
```

### Query battery level

- First you need to call the method `cl_connect()` to connect the device
- Obtain the battery power by the method `cl_get_battery()`, the battery power is only an estimated value, not necessarily accurate

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect();
  printf("Battery level: %d %%\n", cl_get_battery());
  cl_disconnect();
}
```

### Get game data for offline play

- First you need to call the method `cl_connect()` to connect the device
- You need to get the number of games stored by the method `cl_get_file_count()` first, Only if the number is greater than 1 indicates that there is stored game data
- Then pass in a pointer to a string large enough to receive the data through the method `cl_get_file(char*, int)`, If the length of the incoming string is too small, it will cause an exception and lose the data obtained this time
- This method will cause an automatic switch to file upload mode

```c
#include <stdio.h>
#include "easy_link_c.h"

int main(void) {
  cl_connect();

  int file_count = cl_get_file_count();
  if (file_count > 0) {
    char f[1024 * 10];
    cl_get_file(f, sizeof(f));
    printf("%s\n", f);
  } else {
    printf("no game saved");
  }

  cl_disconnect();
}
```

## Build

Platform:

- Windows
- Linux
- Mac

Dependencies:

- C++11
- [CMake](https://cmake.org) (required)
- [hidapi](https://github.com/libusb/hidapi) (internal)
- [spdlog](https://github.com/gabime/spdlog) (internal)

```shell
git clone ...
cd  EasyLinkSDK && mkdir build && cd build
cmake ..
```

## Licensing

Licensed under [The MIT License (MIT)](LICENSE).

# miguno

## How to build

### Linux and macOS

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
$ just do

# Or use cmake directly
$ cmake --build build/ --config Debug --target all

# You can also build a release version
$ cmake --build build/ --config Release --target all
```

If compilation succeeded, you can now run the main application that will
attempt to connect to your chessboard.

1. Turn the chessboard on.
2. Connect your computer directly to the board via a wired USB cable
   - Unfortunatley, a wireless connection via BLE (Bluetooth Low Energy) seems
     not to work yet.
3. Run `just run main` (or directly `build/src/Debug/main`) to run the main
   application.

If the connection to the chessboard is successful, you will see output similar to:

```shell
# Or, from the top-level project directory:
#
#    $ build/src/Debug/main (if you build the Debug version)
#    $ build/src/Release/main (if you build the Release version)
#
$ just run main
[DEBUG] SDK version: 1.0.0
[DEBUG] Connecting to chessboard via HID...
Successfully connected to chessboard
MCU hardware version: CN_DVT9_220627
BLE hardware version: CNCA100_V201
Battery level: 100%
[DEBUG] Disconnecting from chessboard
$
```

### Microsoft Windows

> TODO: Automate the Windows build setup.

- Use Visual Studio 2022 (Community Edition) and open a clone of this project's git repository.
- The IDE should automatically configure this project via cmake.
- Run `Build > Build All` to compile the project, including the EasyLinkSDK DLL (`easylink.dll`)
  and the main application (`main.exe`) that depends on the EasyLinkSDK DLL.
- Once the compilation is completed, open a terminal in PowerShell (or `cmd.exe`) and run:

  ```shell
  # 1. Make easylink.ddl available to main.exe
  $ cp out\build\x64-Debug\sdk\easylink.dll .\out\build\x64-Debug\src

  # 2. Run main.exe
  $ out\build\x64-Debug\src\main.exe
  ```
