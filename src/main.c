#include "easy_link_c.h"
#include <stdio.h>

int main() {
  cl_connect();

  char version[10];
  int num_chars;
  num_chars = cl_version(version);
  printf("SDK version: %.*s\n", num_chars, version);

  char mcu_version[100];
  num_chars = cl_get_mcu_version(mcu_version);
  printf("MCU hardware version: %.*s\n", num_chars, mcu_version);

  char ble_version[100];
  num_chars = cl_get_ble_version(ble_version);
  printf("BLE hardware version: %.*s\n", num_chars, ble_version);
}
