#include "easy_link_c.h"
#include "EasyLink.h"
#include <cstring>
const string CL_VERSION = "1.0.0";

// init library
shared_ptr<ChessLink> bChessLink = nullptr;
mutex initMutex;

// realtime callback
cl_realtimeCallback rCallback = nullptr;

int cl_version(char *version) {
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

int cl_beep(unsigned short frequency, unsigned short duration) {
  if (bChessLink == nullptr) {
    return false;
  }
  return bChessLink->beep(frequency, duration);
}

int cl_led(const char *data[8]) {
  if (bChessLink == nullptr) {
    return false;
  }
  return bChessLink->setLed(string(data[0], 8), string(data[1], 8),
                            string(data[2], 8), string(data[3], 8),
                            string(data[4], 8), string(data[5], 8),
                            string(data[6], 8), string(data[7], 8));
}

int cl_get_mcu_version(char *data) {
  if (bChessLink == nullptr) {
    return 0;
  }
  auto version = bChessLink->getMcuVersion();
  if (version.length() > 0) {
    if (data != nullptr) {
      strncpy(data, version.c_str(), version.length());
    }
    return version.length();
  } else {
    return 0;
  }
  return 0;
}

int cl_get_ble_version(char *data) {
  if (bChessLink == nullptr) {
    return 0;
  }
  auto version = bChessLink->getBleVersion();
  if (version.length() > 0) {
    if (data != nullptr) {
      strncpy(data, version.c_str(), version.length());
    }
    return version.length();
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

int cl_get_file(char *data, int len) {
  if (bChessLink == nullptr) {
    return -1;
  }

  auto file = bChessLink->getFile(true);
  if (file.size() > 0) {
    string tmp = "";
    for (auto i = file.begin(); i != file.end(); i++) {
      if (tmp == "") {
        tmp += *i;
      } else {
        tmp += ";";
        tmp += *i;
      }
    }
    if (tmp.size() < len) {
      strncpy(data, tmp.c_str(), tmp.size());
      return tmp.size();
    } else {
      return -2;
    }
  } else {
    return 0;
  }
}

void testChess() {
  {

    auto ch = ChessLink::fromHidConnect();

    cout << ch->switchUploadMode() << endl;

    ch->setRealTimeCallback(
        [](string s) { cout << "real time callback call " << s << endl; });

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
