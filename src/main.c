#include "../sdk/easy_link_c.h"
#include <stdio.h>

int main() {
  int num_chars;

  char version[10];
  num_chars = cl_version(version);
  printf("SDK version: %.*s\n", num_chars, version);

  // Connect to chess board device with HID.  If the device is not connected,
  // it will automatically connect when the device is plugged into the computer.
  printf("[DEBUG] Connecting to chess board via HID...\n");
  int success = cl_connect();
  if (success) {
    printf("Successfully connected to chess board\n");
  } else {
    printf("ERROR: Failed to connect to chess board\n");
  }

  char mcu_version[100];
  num_chars = cl_get_mcu_version(mcu_version);
  printf("MCU hardware version: %.*s\n", num_chars, mcu_version);

  char ble_version[100];
  num_chars = cl_get_ble_version(ble_version);
  printf("BLE hardware version: %.*s\n", num_chars, ble_version);

  printf("Battery level: %d%%\n", cl_get_battery());

  printf("[DEBUG] Disconnecting from chessboard\n");
  cl_disconnect();
}
