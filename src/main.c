#include "../sdk/easy_link_c.h"
#include <stdio.h>
#include <stdlib.h>

enum { kRecommendedSdkVersionLength = 20, kRecommendedMcuVersionLength = 100, kRecommendedBleVersionLength = 100 };

int main(void) {
  char sdk_version[kRecommendedSdkVersionLength];
  const size_t sdk_version_length = cl_version(sdk_version);
  if (sdk_version_length > 0) {
    printf("[DEBUG] SDK version: %.*s\n", (int)sdk_version_length, sdk_version);
  } else {
    fprintf(stderr, "[ERROR] Could not get SDK version\n");
  }

  // Connect to chessboard device with HID.  If the device is not connected,
  // it will automatically connect when the device is plugged into the computer.
  printf("[DEBUG] Connecting to chessboard via HID ...\n");
  if (cl_connect() == 1) {
    printf("Successfully connected to chessboard\n");
  } else {
    fprintf(stderr, "[ERROR] Failed to connect to chessboard.  Exiting ...\n");
    return EXIT_FAILURE;
  }

  // MCU
  char mcu_version[kRecommendedMcuVersionLength];
  const size_t mcu_version_length = cl_get_mcu_version(mcu_version);
  if (mcu_version_length > 0) {
    printf("MCU hardware version: %.*s\n", (int)mcu_version_length, mcu_version);
  } else {
    fprintf(stderr, "[ERROR] Could not query MCU hardware version\n");
  }

  // BLE (Bluetooth Low Energy)
  char ble_version[kRecommendedBleVersionLength];

  const size_t ble_version_length = cl_get_ble_version(ble_version);
  if (ble_version_length > 0) {
    printf("BLE hardware version: %.*s\n", (int)ble_version_length, ble_version);
  } else {
    fprintf(stderr, "[ERROR] Could not query BLE hardware version\n");
  }

  // Battery
  const int battery_level = cl_get_battery();
  if (battery_level >= 0) {
    printf("Battery level: %d%%\n", battery_level);
  } else {
    fprintf(stderr, "[ERROR] Could not query the battery level\n");
  }

  // LEDs
  printf("Enabling LEDs for squares d5 and e4\n");
  const char *leds[8] = {"00000000", "00000000", "00000000", "00010000",
                         "00001000", "00000000", "00000000", "00000000"};

  if (cl_led(leds) == 0) {
    fprintf(stderr, "[ERROR] Could not enable/disable LEDs\n");
  }

  // Stored game files
  const int file_count = cl_get_file_count();
  if (file_count >= 0) {
    printf("Stored game files: %d\n", file_count);
  } else {
    fprintf(stderr, "[ERROR] Could not retrieve number of stored game files\n");
  }

  // Disconnect
  printf("[DEBUG] Disconnecting from chessboard\n");
  cl_disconnect();
  return EXIT_SUCCESS;
}
