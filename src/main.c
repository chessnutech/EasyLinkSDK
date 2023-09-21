#include "../sdk/easy_link_c.h"
#include <stdio.h>
#include <stdlib.h>

#define RECOMMENDED_SDK_VERSION_LENGTH 20
#define RECOMMENDED_MCU_VERSION_LENGTH 100
#define RECOMMENDED_BLE_VERSION_LENGTH 100

int main(void) {
  char sdk_version[RECOMMENDED_SDK_VERSION_LENGTH];
  int sdk_version_length = cl_version(sdk_version);
  printf("[DEBUG] SDK version: %.*s\n", sdk_version_length, sdk_version);

  // Connect to chess board device with HID.  If the device is not connected,
  // it will automatically connect when the device is plugged into the computer.
  printf("[DEBUG] Connecting to chess board via HID...\n");
  if (cl_connect()) {
    printf("Successfully connected to chess board\n");
  } else {
    printf("[ERROR] Failed to connect to chess board.  Exiting ...\n");
    return EXIT_FAILURE;
  }

  // MCU
  char mcu_version[RECOMMENDED_MCU_VERSION_LENGTH];
  int mcu_version_length = cl_get_mcu_version(mcu_version);
  if (mcu_version_length > 0) {
    printf("MCU hardware version: %.*s\n", mcu_version_length, mcu_version);
  }

  // BLE (Bluetooth Low Energy)
  char ble_version[RECOMMENDED_BLE_VERSION_LENGTH];
  int ble_version_length = cl_get_ble_version(ble_version);
  if (ble_version_length > 0) {
    printf("BLE hardware version: %.*s\n", ble_version_length, ble_version);
  }

  // Battery
  int battery_level = cl_get_battery();
  if (battery_level >= 0) {
    printf("Battery level: %d%%\n", battery_level);
  }

  printf("Enabling LEDs for squares d5 and e4\n");
  const char *led[8] = {"00000000", "00000000", "00000000", "00010000", "00001000", "00000000", "00000000", "00000000"};
  if (!cl_led(led)) {
    printf("[ERROR] Could not enable/disable LEDs\n");
  }

  printf("[DEBUG] Disconnecting from chessboard\n");
  cl_disconnect();
  return EXIT_SUCCESS;
}
