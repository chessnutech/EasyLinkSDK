# What's EasyLinkSDK?

This is the C/C++ SDK for [Chessnut](https://www.chessnutech.com/) digital
chess computers, such as the Chessnut Air.

## Features

- Get the position of the pieces on the chessboard in real-time
- Control the display of the LED lights on the chessboard
- Control the buzzer to sound
- Query the version of the hardware
- Query battery level
- Get game data for offline play

## Example Usage

### Get the position of the pieces on the chessboard in real-time

- First you need to call the method `cl_connect()` to connect the device
- Create a callback function whose parameter is a FEN string representing the current state of the board
- Set the callback function through the function `cl_set_readtime_callback(callback)`
- Actively switch the chessboard state to real-time mode `cl_switch_real_time_mode()`, only in real-time mode can the chessboard FEN be obtained

```c
#include "easy_link_c.h"
#include <stdio.h>
void realtime_func(const char *data, int len) { printf("%s", data); }
int main() {
  cl_connect();
  cl_set_readtime_callback(realtime_func);
  cl_switch_real_time_mode();
  for (;;) {
    getchar();
  }
}
```

### Chess board LEDs

- First you need to call the method `cl_connect()` to connect the device
- Create an array of strings representing the led states, character '1' means display, character '0' means close. The length of the array is 8, which means there are 8 lines in total, and 8 characters in each line respectively represent the status of the 8 lights in the line.
- Set the led state through the function `cl_led(char **)`

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

For example, to enable the LED for field `c4` only ('x' above), use:

```c
{"00000000",
 "00000000",
 "00000000",
 "00000000",
 "00100000", // <-- here
 "00000000",
 "00000000",
 "00000000"};
```

Full example:

```c
#include "easy_link_c.h"
int main() {
  cl_connect();
  const char *led[8] = {"11111111", "00000000", "11111111", "00000000", "11111111", "00000000", "11111111", "00000000"};
  cl_led(led);
}
```

### Buzzer (beep)

- First you need to call the method `cl_connect()` to connect the device
- call method `cl_beep(frequency,duration)`, The parameters set the frequency and duration of the buzzer respectively

```c
#include "easy_link_c.h"
int main() {
  cl_connect();
  cl_beep(1000, 200);
}
```

### Query the version of the hardware

- First you need to call the method `cl_connect()` to connect the device
- Pass in the string pointer, the data will be written to the passed in string, The method `cl_version()` to get the version of the library, The method `cl_get_mcu_version()` is used to get the MCU hardware version, The method `cl_get_ble_version()` is used to get the ble hardware version
- Make sure the string is long enough, otherwise it will cause an abnormal end

```c
#include "easy_link_c.h"
#include <stdio.h>
int main() {
  cl_connect();

  char version[20];
  cl_version(version);
  printf("%s\n", version);

  char mcu_version[100];
  cl_get_mcu_version(mcu_version);
  printf("%s\n", mcu_version);

  char ble_version[100];
  cl_get_ble_version(ble_version);
  printf("%s\n", ble_version);
}
```

### Query battery level

- First you need to call the method `cl_connect()` to connect the device
- Obtain the battery power by the method `cl_get_battery()`, the battery power is only an estimated value, not necessarily accurate

```c
#include "easy_link_c.h"
#include <stdio.h>
int main() {
  cl_connect();
  printf("%i\n", cl_get_battery());
}
```

### Get game data for offline play

- First you need to call the method `cl_connect()` to connect the device
- You need to get the number of games stored by the method `cl_get_file_count()` first, Only if the number is greater than 1 indicates that there is stored game data
- Then pass in a pointer to a string large enough to receive the data through the method `cl_get_file(char*, int)`, If the length of the incoming string is too small, it will cause an exception and lose the data obtained this time
- This method will cause an automatic switch to file upload mode

```c
#include "easy_link_c.h"
#include <stdio.h>
void realtime_func(const char *data, int len) { printf("%s", data); }
int main() {
  cl_connect();

  int file_count = cl_get_file_count();
  if (file_count > 0) {
    char f[1024 * 10];
    cl_get_file(f, sizeof(f));
    printf("%s\n", f);
  } else {
    printf("no game saved");
  }
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

Install dependencies:

```shell
### Debian/Ubuntu (packages not fully confirmed yet)
# GCC toolchain
sudo apt-get install -y gcc clang clang-tidy cmake lldb ninja-build build-essential
# Dependencies for EasyLinkSDK
sudo apt install libudev-dev libusb-dev libusb-1.0-0-dev
```

Ensure that the build setup uses clang as defined in [.env](.env):

```
CC=clang
CXX=clang++
```

Run the build with `just` (or use `cmake` directly):

```shell
$ just do
```

If compilation succeeded, you can now run the main application that will
attempt to connect to your chess board.

1. Turn the chess board on.
2. Connect your computer directly to the board via a wired USB cable
   - Unfortunatley, a wireless connection via BLE (Bluetooth Low Energy) seems
     not to work yet.
3. Run `just run main` to run the main application.

If the connection to the chess board is successful, you will see output similar to:

```
[DEBUG] SDK version: 1.0.0
[DEBUG] Connecting to chess board via HID...
Successfully connected to chess board
MCU hardware version: CN_DVT9_220627
BLE hardware version: CNCA100_V201
Battery level: 100%
[DEBUG] Disconnecting from chessboard
```

Notes for Windows (until the Windows setup in this project is completed):

- Use Visual Studio 2022 (Community Edition) and open a clone of this project's git repository.
- The IDE should automatically configure this project via cmake.
- Run `Build > Build All` to compile the project, including the EasyLinkSDK DLL (`easylink.dll`)
  and the main application (`main.exe`) that depends on the EasyLinkSDK DLL.
- Once the compilation is completed, open a terminal in PowerShell (or `cmd.exe`) and run:

  ```shell
  # 1. Make easylink.ddl available to main.exe
  # 2. Run main.exe

  $ cp out\build\x64-Debug\sdk\easylink.dll .\out\build\x64-Debug\src \
       && out\build\x64-Debug\src\main.exe
  ```
