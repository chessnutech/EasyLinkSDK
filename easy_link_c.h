#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define EXTERN_FLAGS __declspec(dllexport)
#define ABI __cdecl
#else
#define EXTERN_FLAGS
#define ABI
#endif
// set the version number of the library to the passed in string pointer
// string length set to 20 is a suitable value
EXTERN_FLAGS int ABI cl_version(char *version);

// connect to chess board device with hid
// even if the device is not connected, it will automatically connect when the
// device is plugged into the computer
EXTERN_FLAGS int ABI cl_connect();

// disconnect to chess board
EXTERN_FLAGS void ABI cl_disconnect();

// set chess board mode to read time mode
EXTERN_FLAGS int ABI cl_switch_real_time_mode();

// set chess board mode to upload file mode
EXTERN_FLAGS int ABI cl_switch_upload_mode();

// real time data callback
typedef void(ABI *cl_realtimeCallback)(const char *fen, int len);

// set real time data callback
// set nullptr to disable callback
EXTERN_FLAGS void ABI cl_set_readtime_callback(cl_realtimeCallback callback);

// The parameters set the frequency and duration of the buzzer respectively
// default frequency is 1000, default duration is 200
EXTERN_FLAGS int ABI cl_beep(unsigned short frequency, unsigned short duration);

// the argument is a C string of length 8, character '1' means display,
// character '0' means close
// the length of the array is 8, which means there are 8 lines in total, and 8
// characters in each line respectively represent the status of the 8 lights in
// the line.
// example:
//   const char *data[8] = {"11111111", "00000000", "11111111",
//   "00000000","11111111", "00000000", "11111111", "00000000"};
EXTERN_FLAGS int ABI cl_led(const char *data[8]);

// this method gets the version of the mcu, the version string will be written
// to the parameter data, and return the length of the version string
// string length set to 100 is a suitable value
EXTERN_FLAGS int ABI cl_get_mcu_version(char *data);

// this method gets the version of the ble, the version string will be written
// to the parameter data, and return the length of the version string
// string length set to 100 is a suitable value
EXTERN_FLAGS int ABI cl_get_ble_version(char *data);

// this method returns the battery level
EXTERN_FLAGS int ABI cl_get_battery();

// get the number of game files in internal storage
EXTERN_FLAGS int ABI cl_get_file_count();

// this function will set mode to upload file mode
// get a file from internal storage and delete the file at the same time
// The content of the file records the change of each fen of a game,
// split with ';'
// if the length of the string is less than the length of the passed pointer,
// the string will be written into the passed pointer make sure the length of
// the pointer passed in is large enough if -2 is returned, it means that the
// length is not enough to write the string if 0 is returned, it means no file
// get if any number greater than 0 is returned, it means the length of the
// string written
EXTERN_FLAGS int ABI cl_get_file(char *data, int len);

#ifdef __cplusplus
}
#endif