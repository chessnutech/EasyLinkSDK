#include "easy_link_c.h"
#include "EasyLink.h"
#include <cstring>
const string CL_VERSION = "1.0.0";

// init library
shared_ptr<ChessLink> bChessLink = nullptr;
mutex initMutex;

// realtime callback
cl_realtimeCallback rCallback = nullptr;

size_t cl_version(char *version) {
  if (version) {
    strncpy(version, CL_VERSION.c_str(), CL_VERSION.length());
    return CL_VERSION.length();
  }
  return 0;
}
int cl_connect() {
  lock_guard<mutex> lock(initMutex);

  if (bChessLink == nullptr) {
    bChessLink = ChessLink::fromHidConnect();
  }

  return bChessLink->connect();
}

void cl_disconnect() {
  lock_guard<mutex> lock(initMutex);

  if (bChessLink != nullptr) {
    bChessLink->disconnect();
  }
}

int cl_switch_real_time_mode() {
  if (bChessLink) {
    return bChessLink->switchRealTimeMode();
  }
  return false;
}

int cl_switch_upload_mode() {
  if (bChessLink) {
    return bChessLink->switchUploadMode();
  }
  return false;
}

void cl_set_readtime_callback(cl_realtimeCallback callback) {
  if (bChessLink == nullptr) {
    return;
  }
  rCallback = callback;
  if (rCallback == nullptr) {
    bChessLink->setRealTimeCallback(nullptr);
  } else {
    bChessLink->setRealTimeCallback([](string data) {
      if (rCallback) {
        rCallback(data.data(), data.size());
      }
    });
  }
}

int cl_beep(unsigned short frequencyHz, unsigned short durationMs) {
  if (bChessLink == nullptr) {
    return false;
  }
  return bChessLink->beep(frequencyHz, durationMs);
}

int cl_led(const char *leds[8]) {
  if (bChessLink == nullptr) {
    return false;
  }
  return bChessLink->setLed(string(leds[0], 8), string(leds[1], 8), string(leds[2], 8), string(leds[3], 8),
                            string(leds[4], 8), string(leds[5], 8), string(leds[6], 8), string(leds[7], 8));
}

size_t cl_get_mcu_version(char *version) {
  if (bChessLink == nullptr) {
    return 0;
  }
  auto v = bChessLink->getMcuVersion();
  if (v.length() > 0) {
    if (version != nullptr) {
      strncpy(version, v.c_str(), v.length());
    }
    return v.length();
  } else {
    return 0;
  }
  return 0;
}

size_t cl_get_ble_version(char *version) {
  if (bChessLink == nullptr) {
    return 0;
  }
  auto v = bChessLink->getBleVersion();
  if (v.length() > 0) {
    if (version != nullptr) {
      strncpy(version, v.c_str(), v.length());
    }
    return v.length();
  } else {
    return 0;
  }
  return 0;
}

int cl_get_battery() {
  if (bChessLink == nullptr) {
    return -1;
  }
  return bChessLink->getBattery();
}

int cl_get_file_count() {
  if (bChessLink == nullptr) {
    return -1;
  }
  return bChessLink->getFileCount();
}

int cl_get_file_and_should_delete(char *game_data, size_t len, bool is_delete_file) {
  if (bChessLink == nullptr) {
    return -1;
  }

  const auto file = bChessLink->getFile(is_delete_file);
  if (!file.empty()) {
    string tmp;
    for (const auto &i : file) {
      if (tmp.empty()) {
        tmp += i;
      } else {
        tmp += ";";
        tmp += i;
      }
    }
    if (tmp.size() < len) {
      strncpy(game_data, tmp.c_str(), tmp.size());
      return static_cast<int>(tmp.size());
    } else {
      return -2;
    }
  } else {
    return 0;
  }
}

int cl_get_file(char *game_data, size_t len) { return cl_get_file_and_should_delete(game_data, len, true); }

int cl_get_file_and_delete(char *game_data, size_t len) { return cl_get_file_and_should_delete(game_data, len, true); }

int cl_get_file_and_keep(char *game_data, size_t len) { return cl_get_file_and_should_delete(game_data, len, false); }

void testChess() {
  {

    auto ch = ChessLink::fromHidConnect();

    cout << ch->switchUploadMode() << endl;

    ch->setRealTimeCallback([](string s) { cout << "real time callback call " << s << endl; });

    ch->connect();

    ch->beep();

    // cout << ch->getMcuVersion() << endl;

    // cout << ch->getBleVersion() << endl;

    // cout << ch->getBattery() << endl;

    cout << ch->getFileCount() << endl;

    while (true) {
      auto s = ch->getFile(true);

      cout << s.size() << endl;

      if (s.size() == 0) {
        return;
      }
    }

    return;

    bitset<8> ll[8] = {
        bitset<8>("10000000"), //
        bitset<8>("01000000"), //
        bitset<8>("00100000"), //
        bitset<8>("00010000"), //
        bitset<8>("00001000"), //
        bitset<8>("00000100"), //
        bitset<8>("00000010"), //
        bitset<8>("00000001"), //
    };

    int i = 0;
    while (true) {
      ch->setLed({
          ll[i % 8],
          ll[(i + 1) % 8],
          ll[(i + 2) % 8],
          ll[(i + 3) % 8],
          ll[(i + 4) % 8],
          ll[(i + 5) % 8],
          ll[(i + 6) % 8],
          ll[(i + 7) % 8],
      });
      i++;
    }

    ch->disconnect();
  }
  this_thread::sleep_for(chrono::seconds(1000));

  // unsigned char buf[] = {0x21, 0x01, 0x00};
  // auto r = ch.write(buf, sizeof(buf));

  // while (true) {
  //   unsigned char readBuf[65] = {0};
  //   auto l = ch.read(readBuf, 65);
  //   for (int i = 0; i < sizeof(readBuf); i++) {
  //     cout << hex << " " << int(readBuf[i]);
  //   }
  //   cout << endl;
  //   cout << l << endl;
  // }

  // cout << ch->switchRealTimeMode() << endl;
}
