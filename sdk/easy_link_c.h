#ifndef EASY_LINK_SDK_HEADER_GUARD
#define EASY_LINK_SDK_HEADER_GUARD

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

#include <stddef.h>

/**
 * \brief Get the version of the SDK library.
 *
 * @param version Version data will be written to this string that you provide.
                  The parameter should have a length of at least 20.
 * @return The length of the data written to the version parameter. 0 in case of errors.
 */
EXTERN_FLAGS size_t ABI cl_version(char *version);

/**
 * \brief Connect to the chess board with HID.
 *
 * If the board is not connected, it will automatically connect when it is plugged into the computer.
 *
 * @return 0 (false) on failure, 1 (true) on success
 */
EXTERN_FLAGS int ABI cl_connect();

/**
 * \brief Disconnect from the chess board.
 */
EXTERN_FLAGS void ABI cl_disconnect();

/**
 * \brief Set chess board mode to real-time mode.
 *
 * @return 0 (false) on failure, 1 (true) on success
 */
EXTERN_FLAGS int ABI cl_switch_real_time_mode();

/**
 * \brief Set chess board mode to file upload mode.
 *
 * @return 0 (false) on failure, 1 (true) on success
 */
EXTERN_FLAGS int ABI cl_switch_upload_mode();

/**
 * \brief Type definition for real-time data callback function.
 *
 * @param fen String in FEN (Forsyth-Edwards Notation) that describes the board
              position at the time of the callback.
 * @param len Size (length) of the provided FEN string.
 */
typedef void(ABI *cl_realtimeCallback)(const char *fen, size_t len);

/**
 * \brief Register a real-time data callback function.
 *
 * The callback function receives a FEN string (char*) that describes the board
 * position at the time of the callback, as well as the length (int) of the FEN
 * string.
 *
 * @param callback Callback function. Set to `NULL` pointer to disable the
 *                 callback.
 */
EXTERN_FLAGS void ABI cl_set_readtime_callback(cl_realtimeCallback callback);

/**
 * \brief Make a beeping sound.
 *
 * @param frequencyHz Frequency of the sound, in Hertz, from 1 to 65535.
 *                    Default is 1000.
 * @param durationMs  Duration of the sound, in milliseconds. Default is 200.
 * @return 0 (false) on failure, 1 (true) on success
 */
EXTERN_FLAGS int ABI cl_beep(unsigned short frequencyHz, unsigned short durationMs);

/**
 * \brief Control the LED states of the chess board.
 *
 * Illustration:
 *
 * ```
 *    Chess board
 *    ------------
 *      abcdefgh
 *     8........8  leds[0]
 *     7........7  leds[1]
 *     6........6  leds[2]
 *     5........5  leds[3]
 *     4..x.....4  leds[4]
 *     3........3  leds[5]
 *     2........2  leds[6]
 *     1........1  leds[7]
 *      abcdefgh
 * ```
 *
 * For example, to enable the LED for square c4 only ('x' above), use:
 *
 * ```c
 * const char *leds[8] =
 *   { "00000000",
 *     "00000000",
 *     "00000000",
 *     "00000000",
 *     "00100000", // <-- here
 *     "00000000",
 *     "00000000",
 *     "00000000" };
 *
 * if (cl_led(leds) == 0) {
 *   fprintf(stderr, "[ERROR] Could not enable/disable LEDs\n");
 * }
 * ```
 *
 * @param leds An array of strings (char*) with a length of 8, with each element having 8 chars each,
 *             that represents the LED states of the 8x8 chess board.
 *
 *             The first string represents row 8, the last string is row 1 on the board.
 *             The first character in a string is colum a, the last character is column h.
 *             Char '1' in a string means enable the LED of the associated square.
 *             Char '0' means disable the LED.
 * @return 0 (false) on failure, 1 (true) on success
 */
EXTERN_FLAGS int ABI cl_led(const char *leds[8]);

/**
 * \brief Get the MCU hardware version.
 *
 * @param version Version data will be written to provided string.
 *                The string should have a length of at least 100.
 * @return The length of the data written to the version parameter. 0 in case of failure.
 */
EXTERN_FLAGS size_t ABI cl_get_mcu_version(char *version);

/**
 * \brief Get the BLE (Bluetooth Low Energy) hardware version.
 *
 * @param version Version data will be written to provided string.
 *                The string should have a length of at least 100.
 * @return The length of the data written to the version parameter. 0 in case of failure.
 */
EXTERN_FLAGS size_t ABI cl_get_ble_version(char *version);

/**
 * \brief Get the battery level of the chess board.
 *
 * @return The battery level, in a range from 0 (= 0%) to 100 (= 100%).
 *         Negative values (e.g., -1) in case of errors.  Note that the
 *         battery level is only an estimate that is not always accurate.
 */
EXTERN_FLAGS int ABI cl_get_battery();

/**
 * \brief Get the number of game files in the internal storage of the chess board.
 *
 * @return The number of stored game files. Negative values (e.g., -1) in case of errors.
 */
EXTERN_FLAGS int ABI cl_get_file_count();

/**
 * \brief CAUTION: Retrieve the next available game file from internal storage
 * and then delete (!) the file from internal storage.
 *
 * - **DANGER: If the size of `char *game_data` is too small to fully store
 *   the game file, then the game file is still irrevocably deleted from the
 *   chessboard's internal storage!**
 * - It is recommended to query the number of available game files first via
 *   `cl_get_file_count()` before attempting to retrieve any files.
 * - Use the safer variant `cl_get_file_and_keep()` to perform a dry-run of
 *   file retrieval first (e.g., to verify that the provided `char *game_data`
 *   is sufficiently large), and only if this succeeds do you call
 *   `cl_get_file_and_delete()`.
 * - Unfortunately, the SDK does not allow you to retrieve specific game files
 *   or iterate through them without deletion. The only way to "advance the
 *   cursor" and iterate through available game files is by calling
 *   `cl_get_file_and_delete()` or its alias `cl_get_file()`.
 *
 * Calling this function will set automatically the board's mode to file
 * upload mode.
 *
 * Note: This function and its behavior is kept as-is so that the API
 * is compatible with prior versions of this SDK.
 *
 * Example return value (content of game file):
 * ```
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR;rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR;rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR
 * ```
 * @param game_data The content of the game file will be written to this
 *                  string (char*). The content records the change of each FEN
 *                  of the game, separated by ';'. Make sure the size of the
 *                  provided pointer is sufficiently large, otherwise calling
 *                  this function will result in losing the respective game
 *                  file!
 * @param len Size (length) of the provided game_data parameter.
 * @return Length of the content string written to the provided data pointer.
 *         0 if the game file is empty
 *         -2 if the provided data pointer is too small to hold the content of
 *         the game file.
 */
EXTERN_FLAGS int ABI cl_get_file(char *game_data, size_t len);

/**
 * \brief CAUTION: Retrieve the next available game file from internal storage
 * and then delete (!) the file from internal storage. Alias for
 * `cl_get_file()`.
 *
 * - **DANGER: If the size of `char *game_data` is too small to fully store
 *   the game file, then the game file is still irrevocably deleted from the
 *   chessboard's internal storage!**
 * - It is recommended to query the number of available game files first via
 *   `cl_get_file_count()` before attempting to retrieve any files.
 * - Use the safer variant `cl_get_file_and_keep()` to perform a dry-run of
 *   file retrieval first (e.g., to verify that the provided `char *game_data`
 *   is sufficiently large), and only if this succeeds do you call
 *   `cl_get_file_and_delete()`.
 * - Unfortunately, the SDK does not allow you to retrieve specific game files
 *   or iterate through them without deletion. The only way to "advance the
 *   cursor" and iterate through available game files is by calling
 *   `cl_get_file_and_delete()` or its alias `cl_get_file()`.
 *
 * Calling this function will set automatically the board's mode to file
 * upload mode.
 *
 * Example return value (content of game file):
 * ```
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR;rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR;rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR
 * ```
 * @param game_data The content of the game file will be written to this
 *                  string (char*). The content records the change of each FEN
 *                  of the game, separated by ';'. Make sure the size of the
 *                  provided pointer is sufficiently large, otherwise calling
 *                  this function will result in losing the respective game
 *                  file!
 * @param len Size (length) of the provided game_data parameter.
 * @return Length of the content string written to the provided data pointer.
 *         0 if the game file is empty
 *         -2 if the provided data pointer is too small to hold the content of
 *         the game file.
 */
EXTERN_FLAGS int ABI cl_get_file_and_delete(char *game_data, size_t len);

/**
 * \brief Retrieve the next available game file from internal storage
 * without deleting the file from internal storage. Calling this function
 * repeatedly will always retrieve the same game file.
 *
 * This is a safer variant of `cl_get_file()` and `cl_get_file_and_keep()`
 * because the retrieved game file will not be removed from internal storage.
 * However, only deleting the game file will allow you to retrieve the next
 * game file (think: only deletion allows for iterating through game files).
 * The primary use of this function is therefore to verify that the provided
 * `char* game_data` can hold the full contents of the game file before
 * then calling `cl_get_file_and_delete()`.
 *
 * It is recommended to query the number of available game files first via
 * `cl_get_file_count()` before attempting to retrieve any files.
 *
 * Calling this function will set automatically the board's mode to file
 * upload mode.
 *
 * Example return value (content of game file):
 * ```
 * rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR;rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR;rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR
 * ```
 * @param game_data The content of the game file will be written to this
 *                  string (char*). The content records the change of each FEN
 *                  of the game, separated by ';'. Make sure the size of the
 *                  provided pointer is sufficiently large.
 * @param len Size (length) of the provided game_data parameter.
 * @return Length of the content string written to the provided data pointer.
 *         0 if the game file is empty
 *         -2 if the provided data pointer is too small to hold the content of
 *         the game file.
 */
EXTERN_FLAGS int ABI cl_get_file_and_keep(char *game_data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // EASY_LINK_SDK_HEADER_GUARD
