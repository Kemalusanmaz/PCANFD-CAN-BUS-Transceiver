#include "../include/configuration.hpp"
#include "../../driver/peak-linux-driver-8.20.0/lib/libpcanfd.h" // PEAK-System FD API header
#include <cstdlib>
#include <cstring> // memset()
#include <fcntl.h> // open()
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <ostream>
#include <pcanfd.h>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h> // close()

CANConfiguraton::CANConfiguraton() { jsonParser(); }

// Parses JSON config file and loads it into jsonData
void CANConfiguraton::jsonParser() {
  std::string jsonPath = "/home/trick/kemal/CANFDProject/canConfig.json";
  std::ifstream jsonFile(jsonPath);
  if (!jsonFile.is_open()) {
    std::cerr << "JSON file could not be opened!"
              << std::endl; // error if the file cannot be opened
  }
  jsonFile >> jsonData;
}

// Opens the CAN device file with read/write access
void CANConfiguraton::initialize(std::string deviceStr) {
  const char *device = deviceStr.c_str();

  fd = open(
      device,
      O_RDWR);  // open can channel device file with both read and write mode
  if (fd < 0) { // if there is an error, return -1
    perror("open");
  } else {
    std::cout << "CAN Channel File is opened successfully!" << std::endl;
  }
}

// Set CAN FD configuration: flags, clock, nominal bitrate, data bitrate
void CANConfiguraton::setCanConfig(int flag, int clockHz, int nominalBitrate,
                                   int dataBitrate) {

  // PCANFD_INIT_LISTEN_ONLY	Cihaz yalnızca dinleme modunda açılır.
  // PCANFD_INIT_STD_MSG_ONLY	Sadece standart CAN mesaj kimlikleri
  // gönderilir/alınır. Bu ayar yapılmazsa, her tür mesaj kimliği kullanılır.
  // PCANFD_INIT_FD	Cihaz, CAN FD ISO erişimi için açılır (cihaz
  // destekliyorsa). PCANFD_INIT_FD_NON_ISO	Cihaz, CAN FD non-ISO erişimi
  // için açılır (cihaz destekliyorsa).
  // PCANFD_INIT_TS_DEV_REL	Sürücü tarafından alınan mesajlara verilen zaman
  // damgaları, cihazın başlatıldığı ana göre ayarlanır. PCANFD_INIT_TS_HOST_REL
  // Zaman damgaları, ana bilgisayarın başlatıldığı ana göre ayarlanır.
  // PCANFD_INIT_TS_DRV_REL Zaman damgaları, sürücünün başlatıldığı ana göre
  // ayarlanır (varsayılan).
  // PCANFD_INIT_BUS_LOAD_INFO	Eğer donanım destekliyorsa, sürücü belirli
  // aralıklarla bu kanalın bağlı olduğu CAN veri yolu yükünü gösteren DURUM
  // mesajları gönderir.

  // Flags description:
  // PCANFD_INIT_LISTEN_ONLY: Open device in listen-only mode.
  // PCANFD_INIT_STD_MSG_ONLY: Only use standard CAN IDs.
  // PCANFD_INIT_FD: Open device for CAN FD ISO access.
  // Other flags control timestamp source and bus load info.
  initConfig.flags = PCANFD_INIT_FD; // Enable CAN FD mode
  while (true) {

    // clock alanı:
    // 0: Sürücü, cihazın varsayılan saatini seçer (varsayılan).
    // Diğer değerler: Cihaz donanımında seçilecek saat frekansı (Hz cinsinden).
    // Doğru bit zamanlamasını belirlemek için kullanılır.

    if (flag == 0) {
      // Use default parameters from JSON config

      // clock field:
      // 0: The driver selects the device's default clock frequency (default
      // value). Other values: The clock frequency (in Hz) to be used by the
      // device hardware. This is used to determine the correct bit timing.
      initConfig.clock_Hz = jsonData["initialConfiguration"]
                                    ["clockHz"]; // default clock rate 80kHz
      initConfig.nominal.bitrate =
          jsonData["initialConfiguration"]
                  ["nominalBitrate"]; // 500 kbps nominal bitrate
      initConfig.data.bitrate =
          jsonData["initialConfiguration"]
                  ["dataBitrate"]; // 2 Mbps data rate (for CAN FD )
      break;
    } else if (flag == 1) {
      // Use parameters passed as arguments
      initConfig.clock_Hz = clockHz;
      initConfig.nominal.bitrate = nominalBitrate;
      initConfig.data.bitrate = dataBitrate;
      break;
    } else {
      flag = 0;
      std::cout
          << "This is not a valid option. Default initial parameters are set.";
      continue;
    }
  }
  // Apply the configuration to the CAN device
  int ret = pcanfd_set_init(
      fd, &initConfig); // initialize can port that is opened with bitrate
  if (ret < 0) { // if return is not 0, there is an error and file is closed.
    perror("pcanfd_set_init");
    close(fd);
  } else {
    std::cout << "CAN FD interface is initialized succesfully!" << std::endl;
  }
}
// Get and print current CAN configuration parameters
void CANConfiguraton::getCanConfig() {
  int ret = pcanfd_get_init(
      fd, &initConfig); // get configuration of the bus that is set
  if (ret < 0) {        // if return is not 0, there is an error while initial
                        // parameters are read. So, file is closed.
    perror("pcanfd_get_init");
    close(fd);
  } else {
    std::cout << "CAN FD initial parameters are read successfully!"
              << std::endl;
  }
  std::cout << "clock_Hz: " << initConfig.clock_Hz << std::endl;
  std::cout << "flag: " << initConfig.flags << std::endl;
  std::cout << "nominal bitrate: " << initConfig.nominal.bitrate << std::endl;
  std::cout << "data bitrate: " << initConfig.data.bitrate << std::endl;
}

// Get and print detailed CAN device and bus state info
void CANConfiguraton::getCanState() {
  int state = pcanfd_get_state(
      fd, &canState); // get bus and device state wtih specific informations

  std::cout << "=== PCAN Device State ===" << std::endl;
  std::cout << "PCAN Driver Version: " << canState.ver_major << "."
            << canState.ver_minor << "." << canState.ver_subminor << std::endl;
  std::cout << "CAN Bus State: ";
  switch (canState.bus_state) {
  case PCANFD_UNKNOWN:
    std::cout << "Unknown";
    break;
  case PCANFD_ERROR_ACTIVE:
    std::cout << "Active Error ";
    break;
  case PCANFD_ERROR_PASSIVE:
    std::cout << "Passive Error ";
    break;
  case PCANFD_ERROR_BUSOFF:
    std::cout << "Bus Off";
    break;
  default:
    std::cout << "Unknown State (" << static_cast<int>(canState.bus_state)
              << ")";
  }
  std::cout << std::endl;
  std::cout << "Device Id: 0x" << std::hex << canState.device_id << std::endl;
  std::cout << "Number of Openings: " << canState.open_counter << std::endl;
  std::cout << "Number of Filters Applied: " << canState.filters_counter
            << std::endl;
  std::cout << "Hardware Type: " << canState.hw_type << std::endl;
  std::cout << "Channel Number: " << canState.channel_number << std::endl;
  std::cout << "CAN Status (equal with wCANStatus): 0x" << std::hex
            << canState.can_status << std::endl;
  if (canState.bus_load != 0xFFFF)
    std::cout << "Bus Load: " << canState.bus_load << " %\n";
  else
    std::cout << "Bus Load: (Information is not available)" << std::endl;
  std::cout << "Send Queue Capacity (Number of Message): "
            << canState.tx_max_msgs << std::endl;
  std::cout << "canfdMsgList waiting to be sent: " << canState.tx_pending_msgs
            << std::endl;
  std::cout << "Receive Queue Capacity (Number of Message): "
            << canState.rx_max_msgs << std::endl;
  std::cout << "Number of pending receiving messages: "
            << canState.rx_pending_msgs << std::endl;
  std::cout << "Total Number of Messages Sent: " << canState.tx_frames_counter
            << std::endl;
  std::cout << "Total Number of Messages Received: "
            << canState.rx_frames_counter << std::endl;
  std::cout << "Total Send Errors: " << canState.tx_error_counter << std::endl;
  std::cout << "Total Number of Receiving Errors: " << canState.rx_error_counter
            << std::endl;
  std::cout << "Host Time (ns): " << canState.host_time_ns << std::endl;
  std::cout << "Hardware Time (ns): " << canState.hw_time_ns << std::endl;
  std::cout << "==========================" << std::endl;
}

// Close the CAN device file descriptor
void CANConfiguraton::terminate() { close(fd); }
