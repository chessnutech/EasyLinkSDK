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

/**
 * Get the version of the SDK library.
 *
 * @param version Version data will be written to this string that you provide.
                  The parameter should have a length of at least 20.
 * @return The length of the data written to the version parameter. 0 in case of errors.
 */
EXTERN_FLAGS int ABI cl_version(char *version);

/**
 * Connect to the chess board with HID.
 *
 * If the board is not connected, it will automatically connect when it is plugged into the computer.
 *
 * @return 0 (false) on failure, non-zero (true) on success
 */
EXTERN_FLAGS int ABI cl_connect();

/**
 * Disconnect from the chess board.
 *
 * Unfortunately, the function does not return success or failure information.
 */
EXTERN_FLAGS void ABI cl_disconnect();

/**
 * Set chess board mode to real-time mode.
 *
 * @return 0 (false) on failure, non-zero (true) on success
 */
EXTERN_FLAGS int ABI cl_switch_real_time_mode();

/**
 * Set chess board mode to file upload mode.
 *
 * @return 0 (false) on failure, non-zero (true) on success
 */
EXTERN_FLAGS int ABI cl_switch_upload_mode();

/**
 * Type definition for real-time data callback function.
 *
 * @param fen String in FEN (Forsyth–Edwards Notation) that describes the board
              position at the time of the callback.
 * @param len Size (length) of the provided FEN string.
 */
typedef void(ABI *cl_realtimeCallback)(const char *fen, int len);

/**
 * Register a real-time data callback function.
 *
 * The callback function receives a FEN string (char*) that describes the board
 * position at the time of the callback, as well as the length (int) of the FEN
 * string.
 *
 * @param callback Callback function. Set to `nullptr` to disable the callback.
 */
EXTERN_FLAGS void ABI cl_set_readtime_callback(cl_realtimeCallback callback);

/**
 * Make a beeping sound.
 *
 * @param frequencyHz Frequency of the sound, in Hertz. Default is 1000.
 * @param durationMs  Duration of the sound, in milliseconds. Default is 200.
 * @return 0 (false) on failure, non-zero (true) on success
 */
EXTERN_FLAGS int ABI cl_beep(unsigned short frequencyHz, unsigned short durationMs);

/**
 * Control the LED states of the chess board.
 *
 * Illustration:
 *
 * <code>
 *    abcdefgh
 * 8........8  led[0]
 * 7........7  led[1]
 * 6........6  led[2]
 * 5........5  led[3]
 * 4..x.....4  led[4]
 * 3........3  led[5]
 * 2........2  led[6]
 * 1........1  led[7]
 *  abcdefgh
 * </code>
 *
 * For example, to enable the LED for square c4 only ('x' above), use:
 *
 * <code>
 * { "00000000",
 *   "00000000",
 *   "00000000",
 *   "00000000",
 *   "00100000", // <-- here
 *   "00000000",
 *   "00000000",
 *   "00000000" };
 * </code>
 *
 * @param data An array of strings (char*) with a length of 8, with each element having 8 chars each,
 *             that represents the LED states of the 8x8 chess board.
 *
 *             The first string represents row 8, the last string is row 1 on the board.
 *             The first character in a string is colum a, the last character is column h.
 *             Char '1' in a string means enable the LED of the associated square.
 *             Char '0' means disable the LED.
 * @return 0 (false) on failure, non-zero (true) on success
 */
EXTERN_FLAGS int ABI cl_led(const char *data[8]);

/**
 * Get the MCU hardware version.
 *
 * @param version Version data will be written to provided string.
 *                The string should have a length of at least 100.
 * @return The length of the data written to the version parameter. 0 in case of failure.
 */
EXTERN_FLAGS int ABI cl_get_mcu_version(char *version);

/**
 * Get the BLE (Bluetooth Low Energy) hardware version.
 *
 * @param version Version data will be written to provided string.
 *                The string should have a length of at least 100.
 * @return The length of the data written to the version parameter. 0 in case of failure.
 */
EXTERN_FLAGS int ABI cl_get_ble_version(char *version);

/**
 * Get the battery level of the chess board.
 *
 * @return The battery level, in a range from 0 (= 0%) to 100 (= 100%). Negative values (e.g., -1) in case of errors.
 */
EXTERN_FLAGS int ABI cl_get_battery();

/**
 * Get the number of game files in the internal storage of the chess board.
 *
 * @return The number of stored game files. Negative values (e.g., -1) in case of errors.
 */
EXTERN_FLAGS int ABI cl_get_file_count();

/**
 * Get a game file from internal storage and delete the file at the same time.
 * Calling this function will set the board's mode to file upload mode.
 *
 * @param game_data The content of the game file will be written to this
 *                  string (char*). The content records the change of each FEN
 *                  of the game, separated by ';'. Make sure the size of the
 *                  provided pointer is sufficiently large.
 * @param len Size (length) of the provided game_data parameter.
 * @return Length of the content string written to the provided data pointer.
 *         0 if no game file is available.
 *         -2 if the provided data pointer is too small to hold the content of
 *         the game file.
 */
EXTERN_FLAGS int ABI cl_get_file(char *game_data, int len);

#ifdef __cplusplus
}
#endif