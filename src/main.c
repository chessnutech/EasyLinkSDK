#include "../sdk/easy_link_c.h"
#include <stdio.h>

const int kRecommendedVersionLength = 10;
const int kRecommendedMCUVersionLength = 10;
const int kRecommendedBLEVersionLength = 10;

int main() {
  int num_chars = 0;

  char version[kRecommendedVersionLength];
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

  // MCU
  char mcu_version[kRecommendedMCUVersionLength];
  num_chars = cl_get_mcu_version(mcu_version);
  printf("MCU hardware version: %.*s\n", num_chars, mcu_version);

  // Bluetooth
  char ble_version[kRecommendedBLEVersionLength];
  num_chars = cl_get_ble_version(ble_version);
  printf("BLE hardware version: %.*s\n", num_chars, ble_version);

  // Battery
  printf("Battery level: %d%%\n", cl_get_battery());

  printf("[DEBUG] Disconnecting from chessboard\n");
  cl_disconnect();
}
