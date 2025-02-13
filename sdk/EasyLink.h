#ifdef _DEBUG_FLAG
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/spdlog.h"
#endif
#include "../thirdparty/hidapi/hidapi/hidapi.h"
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <ostream>
#include <stdint.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

using mutex_lock = unique_lock<mutex>;

using RealTimeCallback = void (*)(const string);

class ChessHardConnect {
private:
  // connect mutex
  mutex connectMutex;

  // write mutex
  mutex writeMutex;

  // write time pint;
  std::chrono::steady_clock::time_point writeTime;

public:
  ChessHardConnect();
  virtual ~ChessHardConnect();

  // The connection state of the physical chessboard, default is false
  atomic_bool connectStatus;

  // base write data
  virtual int b_write(const unsigned char *data, size_t length) = 0;

  // base read data
  virtual int b_read(unsigned char *data, size_t length) = 0;

  // write data, based on b_write;
  int write(const unsigned char *data, size_t length);

  // read data, based on b_read;
  int read(unsigned char *data, size_t length);

  /**
  connect to physics chess
  Returns true if the connection is successful; otherwise returns false
  */
  virtual bool b_connect(void) = 0;

  /**
  connect to physics chess, based on b_connect;
   */
  bool connect(void);

  /**
  disconnect to physics chess
 */
  virtual void b_disconnect(void) = 0;

  /**
  disconnect, based on b_disconnect;
  */
  void disconnect(void);

  /**
  returns connectStatus
  Returns true if connected, false otherwise
  */
  bool getConnectStatus(void);
};

// init and clear hidapi library
class ChessHidManager {
public:
  ChessHidManager();
  ~ChessHidManager();
};

class ChessHidConnect : public ChessHardConnect {
private:
  static unique_ptr<ChessHidManager> HidManager;

  // hid device handle
  hid_device *handle;

public:
  ChessHidConnect();
  ~ChessHidConnect();

  // overload
  bool b_connect(void);

  // overload
  void b_disconnect(void);

  // overload
  int b_read(unsigned char *data, size_t length);

  // overload
  int b_write(const unsigned char *data, size_t length);

  /**
  list all device
  */
  static vector<string> listDevice(void);
};

template <class T> class channel {
  T buffer;
  mutex buffer_mutex;
  condition_variable read_cond;

public:
  void write(T data) {
    mutex_lock lock(buffer_mutex);
    buffer = data;
    read_cond.notify_all();
  }

  T read() {
    mutex_lock lock(buffer_mutex);
    read_cond.wait_for(lock, chrono::milliseconds(1000));
    T item = buffer;
    return item;
  }
};

class ChessLink {
private:
  ChessLink(ChessHardConnect *chess_connect);

  // chess board mode
  // 0 is Real Time Mode
  // 1 is Upload Mode
  // default is Upload Mode
  atomic_int mode;

  // read thread status
  // true is active
  // false is closed
  atomic_bool threadMode;

  // Control whether to disconnect and reconnect
  // true is reconnect
  // false never reconnect
  atomic_bool reconnected;

  // the status of file transfer mode
  atomic_bool fileTransfer;

  // the file content
  vector<string> fileContent;

  // file transfer mutex
  mutex fileMutex;

  // file transfer condition variable
  condition_variable fileCV;

  /**
  switch mode
  0x00 is Real Time Mode, In this mode you can get the chess piece layout,
  Need to receive data in callback function

  0x01 is Upload Mode

  Returns true if success, false otherwise
  */
  bool switchMode(unsigned char mode = 0x00);

  // The callback function for receiving data in the Real Time Mode
  RealTimeCallback rCallback;

  // change real data to fen
  static string toFen(unsigned char *, size_t length);

  // led status
  array<bitset<8>, 8> ledStatus;

  // led set mutex
  mutex ledMutex;

  // set led status internal
  bool setLedInternal();

  // data channl
  channel<vector<unsigned char>> data;

  // battery data channl
  channel<vector<unsigned char>> batteryData;

public:
  static const string CL_VERSION;

  ~ChessLink();

  unique_ptr<ChessHardConnect> device;

  /**
  connect to chess board
  */
  bool connect(void);

  /**
  disconnect to chess board
  */
  void disconnect(void);

  /**
  switch to Real Time Mode
  Returns true if success, false otherwise
  */
  bool switchRealTimeMode(void);

  /**
  switch to upload mode
  Returns true if success, false otherwise
  */
  bool switchUploadMode(void);

  /**
  set callback for receiving data in the Real Time Mode
  */
  void setRealTimeCallback(RealTimeCallback callback);

  /**
  Control the buzzer to sound
  frequency is sound frequency, 1-65535
  duration is duration of sound, millisecond
  Returns true if success, false otherwise
  */
  bool beep(const uint16_t frequency = 1000, const uint16_t duration = 200);

  /**
  Control led light
  example:
  setLed({
        bitset<8>("10000000"), //
        bitset<8>("01000000"), //
        bitset<8>("00100000"), //
        bitset<8>("00010000"), //
        bitset<8>("00001000"), //
        bitset<8>("00000100"), //
        bitset<8>("00000010"), //
        bitset<8>("00000001") //
    });
  */
  bool setLed(const array<bitset<8>, 8>);

  /**
  Control led light
  example:
  setLed(
    bitset<8>("10000000"), //
    bitset<8>("01000000"), //
    bitset<8>("00100000"), //
    bitset<8>("00010000"), //
    bitset<8>("00001000"), //
    bitset<8>("00000100"), //
    bitset<8>("00000010"), //
    bitset<8>("00000001")  //
    );
  */
  bool setLed(const bitset<8>, const bitset<8>, const bitset<8>,
              const bitset<8>, const bitset<8>, const bitset<8>,
              const bitset<8>, const bitset<8>);

  /**
  Control led light, A string of length 8 that can only contain character 0 or
  character 1 example: setLed(
    "10000000", //
    "01000000", //
    "00100000", //
    "00010000", //
    "00001000", //
    "00000100", //
    "00000010", //
    "00000001"  //
    );
  */
  bool setLed(const string, const string, const string, const string,
              const string, const string, const string, const string);

  bool setLed(const char* strings);

  /**
  Control led light,But only control the state of the specified location, x and
  y range from 0-7
   */
  bool setLed(uint8_t x, uint8_t y, bool status);

  /**
  query ble version
  */
  string getBleVersion();

  /**
  query mcu version
  */
  string getMcuVersion();

  /**
  query battery state
  */
  uint32_t getBattery();

  /**
  query saved file count
  */
  uint32_t getFileCount();

  /**
  start to get saved file, return list of fen
  this method will call switchUploadMode, chess board mode will change to upload
  mode you can set is_delete to false, it will not to delete files, but at this
  time, multiple calls can only get the same chess game if you want to get all
  the chess games, you must set is_delete true and call the secondary function
  in a loop at the same time if the returned length is 0, it means that there
  are no more games in the store
  */
  vector<string> getFile(bool is_delete = true);

  static shared_ptr<ChessLink> instance()
  {
    static shared_ptr<ChessLink> chessLink = fromHidConnect();
    return chessLink;
  }

private:
  /**
Create ChessLink from HID connect mode
*/
  static shared_ptr<ChessLink> fromHidConnect(void);

};
