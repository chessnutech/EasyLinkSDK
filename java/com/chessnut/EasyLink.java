package com.chessnut;

/**
 * Java Bindings for Chessnut electronic chess board protocol.
 * @url https://github.com/chessnutech/EasyLinkSDK
 */
public class EasyLink
{
    /**
     * @return the version of the SDK library.
     */
    public static native String version();
    /**
     * \brief Connect to the chess board with HID.
     *
     * If the board is not connected, it will automatically connect when it is plugged into the computer.
     *
     * @return 0 (false) on failure, 1 (true) on success
     */
    public static native int connect();
    /**
     * \brief Disconnect from the chess board.
     */
    public static native void disconnect();
    /**
     * \brief Set chess board mode to real-time mode.
     *
     * @return 0 (false) on failure, 1 (true) on success
     */
    public static native int switchRealTimeMode();
    /**
     * \brief Set chess board mode to file upload mode.
     *
     * @return 0 (false) on failure, 1 (true) on success
     */
    public static native int switchUploadMode();

    /**
     * \brief Type definition for real-time data callback function.
     */
    public interface IRealTimeCallback {
        /**
         * @param fen String in FEN (Forsyth-Edwards Notation) that describes the board
        position at the time of the callback.
         */
        void realTimeCallback(String fen);
    }

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
    public static native void setRealtimeCallback(IRealTimeCallback callback);

    /**
     * \brief Make a beeping sound.
     *
     * @param frequencyHz Frequency of the sound, in Hertz, from 1 to 65535.
     *                    Default is 1000.
     * @param durationMs  Duration of the sound, in milliseconds. Default is 200.
     * @return 0 (false) on failure, 1 (true) on success
     */
    public static native int beep(int frequencyHz, int durationMs);

    /**
     * \brief Control the LED states of the chess board.
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
    public static native int led(String leds);

    /**
     * @return the MCU hardware version.
     */
    public static native String getMcuVersion();

    /**
     * @return  the BLE (Bluetooth Low Energy) hardware version.
     */
    public static native String getBleVersion();

    /**
     * \brief Get the battery level of the chess board.
     *
     * @return The battery level, in a range from 0 (= 0%) to 100 (= 100%).
     *         Negative values (e.g., -1) in case of errors.  Note that the
     *         battery level is only an estimate that is not always accurate.
     */
    public static native int getBattery();

    /**
     * \brief Get the number of game files in the internal storage of the chess board.
     *
     * @return The number of stored game files. Negative values (e.g., -1) in case of errors.
     */
    public static native int getFileCount();

    /**
     * \brief CAUTION: Retrieve the next available game file from internal storage
     * and then delete (!) the file from internal storage.
     *
     * @param game_data The content of the game file will be written to this
     *                  string (char*). The content records the change of each FEN
     *                  of the game, separated by ';'. Make sure the size of the
     *                  provided pointer is sufficiently large, otherwise calling
     *                  this function will result in losing the respective game
     *                  file!
     * @return Length of the content string written to the provided data pointer.
     *         0 if the game file is empty
     *         -2 if the provided data pointer is too small to hold the content of
     *         the game file.
     */
    public static native String[] getFile(boolean andDelete);

}
