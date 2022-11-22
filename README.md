## What's  EasyLinkSDK?

---

## Features

---

- Get the position of the pieces on the chessboard in real time
- Control the display of the led lights on the chessboard
- Control the buzzer to sound
- Query the version of the hardware
- Query battery level
- Get game data for offline play

## Usage samples

---

#### Get the position of the pieces on the chessboard in real time

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

#### led

- First you need to call the method `cl_connect()` to connect the device

- Create an array of strings representing the led states, character '1' means display, character '0' means close. the length of the array is 8, which means there are 8 lines in total, and 8 characters in each line respectively represent the status of the 8 lights in the line.
- Set the led state through the function `cl_led(char **)`

```c
#include "easy_link_c.h"
int main() {
  cl_connect();
  char *led[8] = {"11111111", "00000000", "11111111", "00000000", "11111111", "00000000", "11111111", "00000000"};
  cl_led(led);
}
```

#### buzzer

- First you need to call the method `cl_connect()` to connect the device
- call method `cl_beep(frequency,duration)`, The parameters set the frequency and duration of the buzzer respectively

```c
#include "easy_link_c.h"
int main() {
  cl_connect();
  cl_beep(1000, 200);
}
```

#### Query the version of the hardware

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

#### Query battery level

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

#### Get game data for offline play

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

---

Platform:

- Windows
- Linux
- Mac

Dependencies:

- C++11
- [CMake](https://cmake.org)(required)
- [hidapi](https://github.com/libusb/hidapi)(internal)
- [spdlog](https://github.com/gabime/spdlog)(internal)

```shall
git clone ...
cd  EasyLinkSDK && mkdir build && cd build
cmake ..
```

## Licensing
---

licensed under [The MIT License (MIT)](LICENSE).

