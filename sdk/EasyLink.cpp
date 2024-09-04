#include "EasyLink.h"

// device pid, vid , usage_page
constexpr unsigned short DEVICE_VID = 0x2d80;
const array<unsigned short, 7> DEVICE_PIDS = {0x8000, 0x8100, 0x8200, 0x8300,
                                              0x8400, 0x8500, 0x8600};
constexpr unsigned short DEVICE_USAGE_PAGE = 0xFF00;

// hid write time interval,millisecond
constexpr unsigned int WRITE_INTERVAL = 200;

// default chess data
constexpr unsigned char CHESS_PIECES[] = {
    '0', 'q', 'k', 'b', 'p', 'n', 'R', 'P', 'r', 'B', 'N', 'Q', 'K',
};

ChessHardConnect::ChessHardConnect() { this->connectStatus = false; }

ChessHardConnect::~ChessHardConnect() {}

bool inline ChessHardConnect::getConnectStatus(void) {
  return this->connectStatus;
}

int ChessHardConnect::write(const unsigned char *data, size_t length) {

  lock_guard<mutex> lock(this->writeMutex);
  lock_guard<mutex> lock1(this->connectMutex);

  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
               chrono::steady_clock::now() - this->writeTime)
               .count();
  if (t < WRITE_INTERVAL) {
    this_thread::sleep_for(chrono::milliseconds(WRITE_INTERVAL - t));
  }
  auto res = this->b_write(data, length);
  this->writeTime = chrono::steady_clock::now();

  // write data
  {
#ifdef _DEBUG_FLAG
    spdlog::debug("Write Length: {1}, Write Data: {0:n:X:p}",
                  spdlog::to_hex(vector<unsigned char>(data, data + length)),
                  res);
#endif
  }
  return res;
}

int ChessHardConnect::read(unsigned char *data, size_t length) {
  lock_guard<mutex> lock(this->connectMutex);
  return this->b_read(data, length);
}

bool ChessHardConnect::connect() {
  lock_guard<mutex> lock(this->connectMutex);
  return this->b_connect();
}

void ChessHardConnect::disconnect() {
  lock_guard<mutex> lock(this->connectMutex);
  lock_guard<mutex> lock1(this->writeMutex);
  this->b_disconnect();
}

ChessHidManager::ChessHidManager() { hid_init(); }
ChessHidManager::~ChessHidManager() { hid_exit(); }

unique_ptr<ChessHidManager> ChessHidConnect::HidManager =
    unique_ptr<ChessHidManager>();

ChessHidConnect::ChessHidConnect() {
  this->connectStatus = false;
  this->handle = nullptr;
}

ChessHidConnect::~ChessHidConnect() {
  if (this->connectStatus) {
    this->disconnect();
  }
  if (this->handle) {
    hid_close(this->handle);
  }
}

vector<string> ChessHidConnect::listDevice(void) {
  vector<string> res_vec;
  auto base_list = hid_enumerate(DEVICE_VID, 0);
  auto l = base_list;
  while (l != nullptr) {
    for (auto pid = DEVICE_PIDS.begin(); pid != DEVICE_PIDS.end(); pid++) {
      if ((l->product_id & 0xFF00) == *pid &&
          l->usage_page == DEVICE_USAGE_PAGE) {
        res_vec.push_back(l->path);
      }
    }
    l = l->next;
  }
  hid_free_enumeration(base_list);
  return res_vec;
}

bool ChessHidConnect::b_connect(void) {

  if (this->getConnectStatus()) {
    return this->getConnectStatus();
  }

  auto find_hid_vec = ChessHidConnect::listDevice();
  if (this->handle && this->connectStatus) {
    return this->connectStatus;
  }

  if (find_hid_vec.size() >= 1) {
    auto o_handle = hid_open_path(find_hid_vec[0].c_str());
    this->handle = o_handle ? o_handle : nullptr;
    this->connectStatus = handle ? true : false;
    return this->connectStatus;
  } else {
    return false;
  }
}

void ChessHidConnect::b_disconnect() {
  if (this->handle) {
    hid_close(this->handle);
  }
  this->connectStatus = false;
  this->handle = nullptr;
}

int ChessHidConnect::b_read(unsigned char *data, size_t length) {
  if (this->connectStatus && this->handle) {
    return hid_read_timeout(this->handle, data, length, 100);
  }
  return 0;
}

int ChessHidConnect::b_write(const unsigned char *data, size_t length) {
  if (length == 0) {
    return 0;
  }

  if (this->connectStatus && this->handle) {
    return hid_write(this->handle, data, length);
  }
  return 0;
}

ChessLink::ChessLink(ChessHardConnect *chess_connect) {

  this->reconnected = false;

  this->threadMode = true;

  this->fileTransfer = false;

  this->mode = 1;

  this->device = unique_ptr<ChessHardConnect>(chess_connect);

  this->rCallback = nullptr;

  this->ledStatus = {bitset<8>(0), bitset<8>(0), bitset<8>(0), bitset<8>(0),
                     bitset<8>(0), bitset<8>(0), bitset<8>(0), bitset<8>(0)};
}

ChessLink::~ChessLink() { this->disconnect(); }

bool ChessLink::setLedInternal() {
  unsigned char buf[] = {
      0x0a,
      0x08,
      static_cast<unsigned char>(this->ledStatus[0].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[1].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[2].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[3].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[4].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[5].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[6].to_ulong()),
      static_cast<unsigned char>(this->ledStatus[7].to_ulong()),
  };
  auto r = this->device->write(buf, sizeof(buf));
  return r ? true : false;
}

bool ChessLink::setLed(array<bitset<8>, 8> status) {
  {
    lock_guard<mutex> lock(this->ledMutex);
    this->ledStatus = status;
  }
  return this->setLedInternal();
}

bool ChessLink::setLed(bitset<8> b1, bitset<8> b2, bitset<8> b3, bitset<8> b4,
                       bitset<8> b5, bitset<8> b6, bitset<8> b7, bitset<8> b8) {
  {
    lock_guard<mutex> lock(this->ledMutex);
    this->ledStatus = array<bitset<8>, 8>{b1, b2, b3, b4, b5, b6, b7, b8};
  }
  return this->setLedInternal();
}

bool ChessLink::setLed(string s1, string s2, string s3, string s4, string s5,
                       string s6, string s7, string s8) {
  {
    if (s1.length() != 8 || s2.length() != 8 || s3.length() != 8 ||
        s4.length() != 8 || s5.length() != 8 || s6.length() != 8 ||
        s7.length() != 8 || s8.length() != 8) {
      return false;
    }
    lock_guard<mutex> lock(this->ledMutex);
    this->ledStatus = array<bitset<8>, 8>{
        bitset<8>(s1), bitset<8>(s2), bitset<8>(s3), bitset<8>(s4),
        bitset<8>(s5), bitset<8>(s6), bitset<8>(s7), bitset<8>(s8)};
  }
  return this->setLedInternal();
}

bool ChessLink::setLed(uint8_t x, uint8_t y, bool status) {
  {
    if (x > 7 || y > 7) {
      return false;
    }
    lock_guard<mutex> lock(this->ledMutex);

    this->ledStatus[x][y] = status;
  }
  return this->setLedInternal();
}

string ChessLink::getMcuVersion() {
  unsigned char buf[] = {
      0x27,
      0x01,
      0x01,
  };
  auto r = this->device->write(buf, sizeof(buf));
  if (r > 0) {
    auto version = this->data.read();
    if (version.size() > 0) {
      return string((char *)version.data() + 3, version.size() - 3);
    }
  }
  return "";
}

string ChessLink::getBleVersion() {
  unsigned char buf[] = {
      0x27,
      0x01,
      0x00,
  };
  auto r = this->device->write(buf, sizeof(buf));
  if (r > 0) {
    auto version = this->data.read();
    if (version.size() > 0) {
      return string((char *)version.data() + 3, version.size() - 3);
    }
  }
  return "";
}

uint32_t ChessLink::getBattery() {
  unsigned char buf[] = {
      0x29,
      0x01,
      0x00,
  };
  auto r = this->device->write(buf, sizeof(buf));
  if (r > 0) {
    auto version = this->batteryData.read();
    if (version.size() > 0) {
      return version[2];
    }
  }
  return 0;
}

uint32_t ChessLink::getFileCount() {
  unsigned char buf[] = {
      0x31,
      0x01,
      0x00,
  };
  auto r = this->device->write(buf, sizeof(buf));
  if (r > 0) {
    auto version = this->data.read();
    if (version.size() > 0) {
      return version[2];
    }
  }
  return 0;
}

vector<string> ChessLink::getFile(bool is_delete) {
  vector<string> file = {};
  if (this->getFileCount() == 0) {
    return file;
  }

  this->switchUploadMode();

  unsigned char buf1[] = {
      0x33,
      0x01,
      0x00,
  };
  auto r1 = this->device->write(buf1, sizeof(buf1));

  if (r1 > 0) {
    unsigned char buf2[] = {
        0x34,
        0x01,
        0x01,
    };
    auto r2 = this->device->write(buf2, sizeof(buf2));

    if (r2 > 0) {
      mutex_lock lock(this->fileMutex);
      if (this->fileCV.wait_for(lock, chrono::seconds(120)) ==
          cv_status::no_timeout) {
        file = this->fileContent;

        // file get success, delete it
        if (is_delete) {
          unsigned char buf3[] = {
              0x39,
              0x01,
              0x00,
          };
          this->device->write(buf3, sizeof(buf3));
        }

      } else {
        this->fileTransfer = false;
      }
    }
  }
  return file;
}

bool ChessLink::connect() {
#ifdef _DEBUG_FLAG
  spdlog::set_level(spdlog::level::debug);
#endif
  this->reconnected = true;
  return this->device->connect();
}

void ChessLink::disconnect() {
  this->reconnected = false;
  this->device->disconnect();
}

bool ChessLink::beep(unsigned short frequency, unsigned short duration) {
  unsigned char buf[] = {0x0b,
                         0x04,
                         static_cast<unsigned char>(frequency >> 8),
                         static_cast<unsigned char>(frequency & 0xff),
                         static_cast<unsigned char>(duration >> 8),
                         static_cast<unsigned char>(duration & 0xff)};
  auto r = this->device->write(buf, sizeof(buf));
  return r ? true : false;
}

bool ChessLink::switchMode(unsigned char mode) {
  unsigned char buf[] = {0x21, 0x01, mode};
  auto r = this->device->write(buf, sizeof(buf));
  return r ? true : false;
}

bool ChessLink::switchRealTimeMode() {
  this->mode = 0;
  if (this->device->getConnectStatus()) {
    return this->switchMode(0x00);
  } else {
    return true;
  }
}

bool ChessLink::switchUploadMode() {
  this->mode = 1;
  if (this->device->getConnectStatus()) {
    return this->switchMode(0x01);
  } else {
    return true;
  }
}

void ChessLink::setRealTimeCallback(RealTimeCallback callback) {
  if (callback) {
    this->rCallback = callback;
  } else {
    this->rCallback = nullptr;
  }
}

string ChessLink::toFen(unsigned char *data, size_t length) {
  if (length <= 32) {
    return "";
  }
  string fen = "";
  int empty = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 7; j >= 0; j--) {
      char piece =
          j % 2 == 0
              ? CHESS_PIECES[data[(i * 8 + j) / 2 + 2] & 0x0f]
              : CHESS_PIECES
                    [data[static_cast<int>(floor((i * 8 + j) / 2 + 2))] >> 4];
      if (piece == '0')
        empty++;
      else {
        if (empty > 0) {
          fen += to_string(empty);
          empty = 0;
        }
      }
      if (piece != '0')
        fen += piece;
    }
    if (empty > 0)
      fen += to_string(empty);
    if (i < 7)
      fen += "/";
    empty = 0;
  }
  return fen;
}

shared_ptr<ChessLink> ChessLink::fromHidConnect() {
  ChessHardConnect *c = new ChessHidConnect();
  shared_ptr<ChessLink> r(new ChessLink(c));

  thread readThread = thread(
      [](shared_ptr<ChessLink> chesslink) {
        unsigned char readBuf[256];
        while (chesslink->threadMode && chesslink.use_count() >= 2) {
          if (chesslink->device->connectStatus) {
            int res = chesslink->device->read(readBuf, sizeof(readBuf));
            if (res >= 1) {

              unsigned int real_size = readBuf[1] + 2;

              // get data success
              {
#ifdef _DEBUG_FLAG
                spdlog::debug("Read Length: {1}, Read Data: {0:n:X:p}",
                              spdlog::to_hex(vector<unsigned char>(
                                  readBuf, readBuf + real_size)),
                              real_size);
#endif
              }

              if (readBuf[0] == 0x37 && readBuf[1] == 0x01 &&
                  readBuf[2] == 0xbe) {
                // start get file
                lock_guard<mutex> lock(chesslink->fileMutex);
                chesslink->fileTransfer = true;
                chesslink->fileContent.clear();
              }

              if (readBuf[0] == 0x37 && readBuf[1] == 0x01 &&
                  readBuf[2] == 0xed) {
                // get file end
                chesslink->fileTransfer = false;
                chesslink->fileCV.notify_all();
              }

              // Processed separately according to the type of data received
              if (readBuf[0] == 0x01) {
                if (chesslink->fileTransfer) {
                  // if file transfer mode is true
                  lock_guard<mutex> lock(chesslink->fileMutex);
                  chesslink->fileContent.push_back(
                      ChessLink::toFen(readBuf, real_size));

                } else {
                  // chessboard piece layout data in Real Time Mode
                  if (chesslink->rCallback) {
                    chesslink->rCallback(ChessLink::toFen(readBuf, real_size));
                  }
                }

              } else {
                if (readBuf[0] == 0x2A) {
                  // battery status
                  // The new hardware will report the battery level information
                  // in real time, so the battery level information is
                  // additionally processed
                  if (readBuf[2] != 0) {
                    chesslink->batteryData.write(
                        vector<unsigned char>(readBuf, readBuf + res));
                  }

                } else {
                  // Normal response information processing
                  chesslink->data.write(
                      vector<unsigned char>(readBuf, readBuf + res));
                }
              }

            } else if (res == 0) {
              // no data, sleep for a while
              this_thread::sleep_for(chrono::milliseconds(10));
            } else if (res < 0) {
              // some thing wrong, The device may be disconnected
              chesslink->device->disconnect();
            }
          } else {
            if (chesslink->reconnected) {
              if (chesslink->connect()) {
                if (chesslink->mode == 0) {
                  chesslink->switchRealTimeMode();
                }
                if (chesslink->mode == 1) {
                  chesslink->switchUploadMode();
                }
              }
            }
            this_thread::sleep_for(chrono::milliseconds(10));
          }
        }
        return;
      },
      r);
  readThread.detach();

  return r;
}
