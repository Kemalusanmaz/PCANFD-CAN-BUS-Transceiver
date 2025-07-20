#include "../include/configuration.hpp"
#include "../../driver/peak-linux-driver-8.20.0/lib/libpcanfd.h" // PEAK-System FD API header
#include <cstdint>
#include <cstdlib>
#include <cstring> // memset()
#include <fcntl.h> // open()
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <malloc.h>
#include <ostream>
#include <pcanfd.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h> // close()

CANConfiguraton::CANConfiguraton() { jsonParser(); }
// Parses JSON config file and loads it into jsonData
void CANConfiguraton::jsonParser() {
  std::filesystem::path jsonPath = "canConfig.json";
  std::ifstream jsonFile(jsonPath);
  if (!jsonFile.is_open()) {
    std::cerr << "JSON file could not be opened!"
              << std::endl; // error if the file cannot be opened
  }
  jsonFile >> jsonData;
}

void CANConfiguraton::checkError(int ret, const char *msg) {
  if (ret < 0) {
    std::cerr << msg << " Error Code: " << strerror(-ret) << std::endl;
    exit(EXIT_FAILURE);
  }
}

// opening a PCAN device with various initialization options
void CANConfiguraton::canOpen(std::string deviceStr, uint32_t nominalBitrate,
                              uint32_t dataBitrate, uint32_t clockHz) {
  uint32_t flags = OFD_BITRATE | OFD_DBITRATE | OFD_CLOCKHZ | OFD_NONBLOCKING |
                   PCANFD_INIT_FD;
  const char *device = deviceStr.c_str();
  // Open and initialize the device with the above configuration
  int fd = pcanfd_open(device, flags,
                       nominalBitrate, // 3rd argument: nominal bitrate
                       dataBitrate,    // 4th argument: data bitrate
                       clockHz         // 5th argument: clock frequency (Hz)
  );
  checkError(fd, "Failed to open and initialize PCAN device");
  std::cout << "PCAN device successfully opened and initialized." << std::endl;
}

// Opens the CAN device file with read/write access
void CANConfiguraton::initialize(std::string deviceStr) {
  const char *device = deviceStr.c_str();

  fd = open(device,
            O_RDWR | O_NONBLOCK); // open can channel device file with both read
                                  // and write mode
  if (fd < 0) {                   // if there is an error, return -1
    perror("open");
  } else {
    std::cout << "CAN Channel File is opened successfully!" << std::endl;
  }
}

// Set CAN FD configuration: flags, clock, nominal bitrate, data bitrate
void CANConfiguraton::setCanConfig(CanChannelSettings settings) {

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

  // clock alanı:
  // 0: Sürücü, cihazın varsayılan saatini seçer (varsayılan).
  // Diğer değerler: Cihaz donanımında seçilecek saat frekansı (Hz cinsinden).
  // Doğru bit zamanlamasını belirlemek için kullanılır.

  // Use default parameters from JSON config

  // clock field:
  // 0: The driver selects the device's default clock frequency (default
  // value). Other values: The clock frequency (in Hz) to be used by the
  // device hardware. This is used to determine the correct bit timing.

  initConfig.clock_Hz = settings.getClock();
  initConfig.nominal.bitrate = settings.getNominalBitRate();
  initConfig.data.bitrate = settings.getDataBitRate();

  // Apply the configuration to the CAN device
  int ret = pcanfd_set_init(
      fd, &initConfig); // initialize can port that is opened with bitrate
  if (ret < 0) { // if return is not 0, there is an error and file is closed.
    perror("pcanfd_set_init");
    std::cout << "Error: pcanfd_set_init!" << std::endl;
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
  std::cout << "Clock (hz): " << static_cast<unsigned int>(initConfig.clock_Hz)
            << std::endl;
  std::cout << "Nominal Bitrate: "
            << static_cast<unsigned int>(initConfig.nominal.bitrate)
            << std::endl;
  std::cout << "Data Bitrate: "
            << static_cast<unsigned int>(initConfig.data.bitrate) << std::endl;
  std::cout << "Flag: " << static_cast<unsigned int>(initConfig.flags)
            << std::endl;
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
  std::cout << "Device Id: 0x" << std::hex << canState.device_id << std::dec
            << std::endl;
  std::cout << "Number of Openings: "
            << static_cast<unsigned int>(canState.open_counter) << std::endl;
  std::cout << "Number of Filters Applied: "
            << static_cast<unsigned int>(canState.filters_counter) << std::endl;
  std::cout << "Hardware Type: " << static_cast<unsigned int>(canState.hw_type)
            << std::endl;
  std::cout << "Channel Number: "
            << static_cast<unsigned int>(canState.channel_number) << std::endl;
  std::cout << "CAN Status (equal with wCANStatus): 0x" << std::hex
            << canState.can_status << std::dec << std::endl;
  if (canState.bus_load != 0xFFFF)
    std::cout << "Bus Load: " << static_cast<unsigned int>(canState.bus_load)
              << " %\n";
  else
    std::cout << "Bus Load: (Information is not available)" << std::endl;
  std::cout << "Send Queue Capacity (Number of Message): "
            << static_cast<unsigned int>(canState.tx_max_msgs) << std::endl;
  std::cout << "canfdMsgList waiting to be sent: "
            << static_cast<unsigned int>(canState.tx_pending_msgs) << std::endl;
  std::cout << "Total Number of Messages Received: "
            << canState.rx_frames_counter << std::endl;
  std::cout << "Total Send Errors: "
            << static_cast<unsigned int>(canState.tx_error_counter)
            << std::endl;
  std::cout << "Total Number of Receiving Errors: "
            << static_cast<unsigned int>(canState.rx_error_counter)
            << std::endl;
  std::cout << "Host Time (ns): "
            << static_cast<unsigned int>(canState.host_time_ns) << std::endl;
  std::cout << "Hardware Time (ns): "
            << static_cast<unsigned int>(canState.hw_time_ns) << std::endl;
  std::cout << "==========================" << std::endl;
}

// Close the CAN device file descriptor
void CANConfiguraton::terminate() {
  close(fd);
  std::cout << "Channel File Descriptor is closed succesfully!" << std::endl;
}

void CANConfiguraton::getCanClocks() {
  // Allocate enough space to store 6 clock values
  const int max_clocks = 6;
  pac = (struct pcanfd_available_clocks *)malloc(
      sizeof(*pac) + max_clocks * sizeof(struct pcanfd_available_clock));
  if (!pac) {
    std::cerr << "Memory can not be arranged.\n";
  }
  pac->count = max_clocks;
  int ret = pcanfd_get_available_clocks(fd, pac);
  if (ret) {
    std::cerr << "Clocks can not be acquired, Error Code: " << ret << "\n";
    free(pac);
  }

  // Write clocks that are acquired.
  for (int i = 0; i < static_cast<int>(pac->count); ++i) {
    std::cout << "Clock #" << i << "/" << pac->count << ": "
              << pac->list[i].clock_Hz << " Hz, "
              << "Source Code: " << pac->list[i].clock_src << "\n";
  }

  free(pac);
}

void CANConfiguraton::getBitTimingRanges() {
  // 2 zamanlama aralığı için yer ayır
  const int range_count = 2;
  pbr = (struct pcanfd_bittiming_ranges *)malloc(
      sizeof(*pbr) + range_count * sizeof(struct pcanfd_bittiming_range));

  if (!pbr) {
    std::cerr << "Memory can not be arranged.\n";
  }
  pbr->count = range_count;

  int ret = pcanfd_get_bittiming_ranges(fd, pbr);
  if (ret) {
    std::cerr << "Bit timing ranges can not be acquired, Error Code: " << ret
              << "\n";
    free(pbr);
  }

  for (unsigned int i = 0; i < pbr->count; ++i) {
    const auto &bt = pbr->list[i];
    std::cout << "Timing #" << i << ":\n";
    std::cout << "  BRP: " << bt.brp_min << " - " << bt.brp_max
              << " (Increment: " << bt.brp_inc << ")\n";
    std::cout << "  TSEG1: " << bt.tseg1_min << " - " << bt.tseg1_max << "\n";
    std::cout << "  TSEG2: " << bt.tseg2_min << " - " << bt.tseg2_max << "\n";
    std::cout << "  SJW: " << bt.sjw_min << " - " << bt.sjw_max << "\n";
  }

  free(pbr);
}

void CANConfiguraton::getChannelFeatures() {
  // Read Channel Features
  uint32_t features = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_CHANNEL_FEATURES, &features,
                              sizeof(features));
  checkError(ret, "PCANFD_OPT_CHANNEL_FEATURES can not read.");

  std::cout << "Channel Features bitmask: 0x" << std::hex << features
            << std::dec << std::endl;
  if (features & PCANFD_FEATURE_FD)
    std::cout << " - CAN FD is supported\n";
  if (features & PCANFD_FEATURE_IFRAME_DELAYUS)
    std::cout << " - Frame Delay is supported\n";
  if (features & PCANFD_FEATURE_BUSLOAD)
    std::cout << " - Bus Load Calculation is supported\n";
  if (features & PCANFD_FEATURE_HWTIMESTAMP)
    std::cout << " - Hardware Time Stamp is supported\n";
  if (features & PCANFD_FEATURE_DEVICEID)
    std::cout << " - Device ID support is available\n";
}

void CANConfiguraton::getDeviceId() {
  // Read Device Id
  uint32_t deviceId = 0;
  int ret =
      pcanfd_get_option(fd, PCANFD_OPT_DEVICE_ID, &deviceId, sizeof(deviceId));
  checkError(ret, "PCANFD_OPT_DEVICE_ID can not read.");
  std::cout << "Device Id: " << deviceId << std::endl;
}

void CANConfiguraton::getAcceptanceFilter() {
  // Read Accept filter (11-bit)
  uint64_t accFilter = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_ACC_FILTER_11B, &accFilter,
                              sizeof(accFilter));
  checkError(ret, "PCANFD_OPT_ACC_FILTER_11B can not read.");
  uint32_t code = static_cast<uint32_t>(accFilter >> 32);
  uint32_t mask = static_cast<uint32_t>(accFilter & 0xFFFFFFFF);
  std::cout << "11-bit Accept Filter Code: 0x" << std::hex << code << std::dec
            << std::endl;
  std::cout << "11-bit Accept Filter Mask: 0x" << std::hex << mask << std::dec
            << std::endl;
}

void CANConfiguraton::getFrameDelayTime() {
  // Read Delay Time between each frame (microsecond)
  uint32_t delayUs = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_IFRAME_DELAYUS, &delayUs,
                              sizeof(delayUs));
  checkError(ret, "PCANFD_OPT_IFRAME_DELAYUS can not read.");
  std::cout << "Delay Time between frames (μs): " << delayUs << std::endl;
}

void CANConfiguraton::getTimeStampMode() {
  // Read Time Stamp Mode
  uint32_t hwTimestampMode = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_HWTIMESTAMP_MODE, &hwTimestampMode,
                              sizeof(hwTimestampMode));
  checkError(ret, "PCANFD_OPT_HWTIMESTAMP_MODE can not read.");
  std::cout << "Time Stamp Mode: ";
  switch (hwTimestampMode) {
  case PCANFD_OPT_HWTIMESTAMP_OFF:
    std::cout << "Based On Host Time (Hardware is off)\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_ON:
    std::cout << "Host Time + Base Hardware Offset\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_COOKED:
    std::cout << "Host Time + Fixed Hardware Offset\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_RAW:
    std::cout << "Base Hardware Time Stamp\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_SOF_ON:
    std::cout << "Base Time Stamp (Start of Frame)\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_SOF_COOKED:
    std::cout << "Fixed Time Stamp (Start of Frame)\n";
    break;
  case PCANFD_OPT_HWTIMESTAMP_SOF_RAW:
    std::cout << "Base Time Stamp (Start of Frame Raw)\n";
    break;
  default:
    std::cout << "Unknown Mode\n";
    break;
  }
}

void CANConfiguraton::getDriverVersion() {
  // Read Driver Version
  uint32_t drvVersion = 0, fwVersion = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_DRV_VERSION, &drvVersion,
                              sizeof(drvVersion));
  checkError(ret, "Driver Version can not read.");
  std::cout << "Driver Version: " << ((drvVersion >> 24) & 0xFF) << "."
            << ((drvVersion >> 16) & 0xFF) << "." << ((drvVersion >> 8) & 0xFF)
            << std::endl;
}

void CANConfiguraton::getFirmwareVersion() {
  // Read Fİrmware Version
  uint32_t fwVersion = 0;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_FW_VERSION, &fwVersion,
                              sizeof(fwVersion));
  checkError(ret, "Firmware Version can not read.");
  std::cout << "Firmware Version: " << ((fwVersion >> 24) & 0xFF) << "."
            << ((fwVersion >> 16) & 0xFF) << "." << ((fwVersion >> 8) & 0xFF)
            << std::endl;
}

void CANConfiguraton::getDriverClockReference() {
  uint32_t canIo = 0;
  int ret =
      pcanfd_get_option(fd, PCANFD_OPT_DRV_CLK_REF, &canIo, sizeof(canIo));
  checkError(ret, "Driver Clock Reference can not be read");
  std::cout << "Driver Clock Reference" << ": " << canIo << std::endl;
}

void CANConfiguraton::getLinger() {
  uint32_t canIo = 0;
  std::string state;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_LINGER, &canIo, sizeof(canIo));
  checkError(ret, "Linger Mode can not be read");
  if (canIo == 0) {
    state = "No Wait";
    std::cout
        << "Linger Mode (Maximum time (ms) the driver waits for frames to be "
           "written before shutting down the device): "
        << state << std::endl;
  } else {
    std::cout
        << "Linger Mode (Maximum time (ms) the driver waits for frames to be "
           "written before shutting down the device): "
        << canIo << std::endl;
  }
}

void CANConfiguraton::getSelfAck() {
  uint32_t canIo = 0;
  std::string state;

  int ret = pcanfd_get_option(fd, PCANFD_OPT_SELF_ACK, &canIo, sizeof(canIo));
  checkError(ret, "Self Ack can not be read");
  if (canIo == 0) {
    state = "Disable";
  } else {
    state = "Enable";
  }
  std::cout << "Self Ack Mode: " << state << std::endl;
}

void CANConfiguraton::getBRSIgnore() {
  uint32_t canIo = 0;
  std::string state;
  int ret = pcanfd_get_option(fd, PCANFD_OPT_BRS_IGNORE, &canIo, sizeof(canIo));
  checkError(ret, "BRS Ignore Mode can not be read");
  if (canIo == 0) {
    state = "Disable";
  } else {
    state = "Enable";
  }
  std::cout << "BRS Ignore Mode" << ": " << state << std::endl;
}

// Assign a custom user-defined numeric ID to a CAN channel (if supported)
void CANConfiguraton::setDeviceId(uint32_t deviceId) {
  int ret =
      pcanfd_set_option(fd, PCANFD_OPT_DEVICE_ID, &deviceId, sizeof(deviceId));
  checkError(ret, "Failed to set device ID");
  std::cout << "Device Id set to: " << deviceId << std::endl;
}

// Set allowed message types
void CANConfiguraton::setAllowedMsgs(std::string flag) {
  // PCANFD_ALLOWED_MSG_- CAN Standard CAN messages only
  // PCANFD_ALLOWED_MSG_RTR - Include RTR (remote request)
  // PCANFD_ALLOWED_MSG_EXT - Include Extended IDs
  // PCANFD_ALLOWED_MSG_STATUS - Include Status messages
  // PCANFD_ALLOWED_MSG_ERROR - Include Error messages

  const uint32_t options[] = {PCANFD_ALLOWED_MSG_CAN, PCANFD_ALLOWED_MSG_RTR,
                              PCANFD_ALLOWED_MSG_EXT, PCANFD_ALLOWED_MSG_STATUS,
                              PCANFD_ALLOWED_MSG_ERROR};
  uint32_t flags = 0;

  std::stringstream ss(flag);
  std::string token;
  while (std::getline(ss, token, ',')) {
    int index = std::stoi(token);
    if (index >= 0 &&
        index < static_cast<int>(sizeof(options) / sizeof(options[0]))) {
      flags |= options[index];
    } else {
      std::cerr << "Invalid option index: " << index << std::endl;
    }
  }

  int ret =
      pcanfd_set_option(fd, PCANFD_OPT_ALLOWED_MSGS, &flags, sizeof(flags));
  checkError(ret, "Failed to set allowed message types");
  std::cout << "Allowed message types set (bitmask = 0x" << std::hex << flags
            << ")" << std::dec << std::endl;
}

//  Set acceptance code and mask for 11-bit standard CAN identifiers
void CANConfiguraton::setAccFilter11B(uint32_t code, uint32_t mask) {
  uint64_t filter = (static_cast<uint64_t>(code) << 32) | mask;
  int ret =
      pcanfd_set_option(fd, PCANFD_OPT_ACC_FILTER_11B, &filter, sizeof(filter));
  checkError(ret, "Failed to set 11-bit acceptance filter");
  std::cout << "11-bit acceptance filter set to: " << filter << std::endl;
}

// Set acceptance code and mask for 29-bit extended CAN identifiers
void CANConfiguraton::setAccFilter29B(uint32_t code, uint32_t mask) {
  uint64_t filter = (static_cast<uint64_t>(code) << 32) | mask;
  int ret =
      pcanfd_set_option(fd, PCANFD_OPT_ACC_FILTER_29B, &filter, sizeof(filter));
  checkError(ret, "Failed to set 29-bit acceptance filter");
  std::cout << "29-bit acceptance filter set to: " << filter << std::endl;
}

// Define the delay in microseconds between sent frames (if controller supports
// it)
void CANConfiguraton::setFrameDelayTime(uint32_t delayUs) {
  int ret = pcanfd_set_option(fd, PCANFD_OPT_IFRAME_DELAYUS, &delayUs,
                              sizeof(delayUs));
  checkError(ret, "Failed to set inter-frame delay");
  std::cout << "Frame Delay Time set to: " << delayUs << " μs" << std::endl;
}

//  Set timestamp mode
void CANConfiguraton::setHwTimestampMode(std::string flag) {
  // 0 = PCANFD_OPT_HWTIMESTAMP_OFF - Host time only
  // 1 = PCANFD_OPT_HWTIMESTAMP_ON - Host time + raw HW offset
  // 2 = PCANFD_OPT_HWTIMESTAMP_COOKED - Host time + cooked HW offset (drift
  // correction)
  // 3 = PCANFD_OPT_HWTIMESTAMP_RAW - Raw HW timestamp (not host related)
  // 4 = PCANFD_OPT_HWTIMESTAMP_SOF_ON - SOF + raw HW offset
  // 5 = PCANFD_OPT_HWTIMESTAMP_SOF_COOKED - SOF + cooked HW offset
  // 6 = PCANFD_OPT_HWTIMESTAMP_SOF_RAW -  SOF + raw timestamp

  // if (flag == "OFF") {
  //   newFilter.msg_flag = PCANFD_MSG_STD;
  // } else if (flag == "RTR") {
  //   newFilter.msg_flags = PCANFD_MSG_RTR;
  // } else if (flag == "EXT") {
  //   newFilter.msg_flags = PCANFD_MSG_EXT;
  // } else if (flag == "SLF") {
  //   newFilter.msg_flags = PCANFD_MSG_SLF;
  // } else if (flag == "SNG") {
  //   newFilter.msg_flags = PCANFD_MSG_SNG;
  // } else if (flag == "ECHO") {
  //   newFilter.msg_flags = PCANFD_MSG_ECHO;
  // } else {
  // }

  const uint32_t modes[] = {
      PCANFD_OPT_HWTIMESTAMP_OFF,    PCANFD_OPT_HWTIMESTAMP_ON,
      PCANFD_OPT_HWTIMESTAMP_COOKED, PCANFD_OPT_HWTIMESTAMP_RAW,
      PCANFD_OPT_HWTIMESTAMP_SOF_ON, PCANFD_OPT_HWTIMESTAMP_SOF_COOKED,
      PCANFD_OPT_HWTIMESTAMP_SOF_RAW};

  int index = std::stoi(flag);
  if (index >= 0 &&
      index < static_cast<int>(sizeof(modes) / sizeof(modes[0]))) {
    uint32_t mode = modes[index];
    int ret =
        pcanfd_set_option(fd, PCANFD_OPT_HWTIMESTAMP_MODE, &mode, sizeof(mode));
    checkError(ret, "Failed to set hardware timestamp mode");
    std::cout << "Hardware timestamp mode set to index " << index << std::endl;
  } else {
    std::cerr << "Invalid timestamp mode index: " << index << std::endl;
  }
}

// Set different linger modes controlling how long the driver waits before
// closing
void CANConfiguraton::setLinger(std::string lingerValue) {
  uint32_t mode;

  if (lingerValue == "No Wait") {
    mode = PCANFD_OPT_LINGER_NOWAIT;
  } else if (lingerValue == "Auto") {
    mode = PCANFD_OPT_LINGER_AUTO;
  }

  int ret = pcanfd_set_option(fd, PCANFD_OPT_LINGER, &mode, sizeof(mode));

  checkError(ret, "Failed to set linger option");
  std::cout << "Set linger to value: " << lingerValue << std::endl;
}

// Enable or disable whether the controller acknowledges its own messages on the
// bus
void CANConfiguraton::setSelfAck(std::string enable) {
  // 0 = Disable
  // 1 = Enable
  int index;
  if (enable == "Disable") {
    index = 0;
  } else if (enable == "Enable") {
    index = 1;
  }

  int ret = pcanfd_set_option(fd, PCANFD_OPT_SELF_ACK, &index, sizeof(index));
  checkError(ret, "Failed to toggle self ACK");

  std::cout << "Self ACK set to: " << enable << std::endl;
}

// Configure whether to ignore CAN FD frames with BRS (Bit Rate Switch) flag
void CANConfiguraton::setBRSIgnore(std::string enable) {
  // 0 = Disable
  // 1 = Enable
  int index;
  if (enable == "Disable") {
    index = 0;
  } else if (enable == "Enable") {
    index = 1;
  }

  int ret = pcanfd_set_option(fd, PCANFD_OPT_BRS_IGNORE, &index, sizeof(index));
  checkError(ret, "Failed to toggle BRS ignore");

  std::cout << "BRS Ignore set to: " << enable << std::endl;
}

void CANConfiguraton::isCanFdCapable() {
  int result = pcanfd_is_canfd_capable(fd);
  if (result == 0) {
    std::cout << "Device does NOT support CAN-FD." << std::endl;
  } else {
    std::cout << "Device supports CAN-FD." << std::endl;
  }
}
// Get serial number from the device
void CANConfiguraton::getSerialNumber() {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_GET_SERIALNUMBER;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to get serial number");
  std::cout << "Serial Number: " << param.func.dwSerialNumber << std::endl;
}
// Get user-defined hardware device number
void CANConfiguraton::getHCDeviceNumber() {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_GET_HCDEVICENO;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to get hardware device number");
  std::cout << "Hardware Device Number: "
            << static_cast<int>(param.func.ucHCDeviceNo) << std::endl;
}
// Set user-defined hardware device number
void CANConfiguraton::setHCDeviceNumber(uint8_t devNo) {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_SET_HCDEVICENO;
  param.func.ucHCDeviceNo = devNo;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to set hardware device number");
  std::cout << "Hardware Device Number set to: " << static_cast<int>(devNo)
            << std::endl;
}
// Get firmware version of the device
void CANConfiguraton::getFirmwareVersionFromDriver() {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_GET_FWVERSION;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to get firmware version");
  std::cout << "Firmware Version: " << param.func.dwSerialNumber << std::endl;
}
// Get adapter name of the device
void CANConfiguraton::getAdapterName() {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_GET_ADAPTERNAME;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to get adapter name");
  std::cout << "Adapter Name: "
            << reinterpret_cast<char *>(param.func.ucDevData) << std::endl;
}
// Get part number of the device
void CANConfiguraton::getPartNumber() {
  memset(&param, 0, sizeof(param));
  param.nSubFunction = SF_GET_PARTNUM;

  int ret = pcan_set_extra_params(fd, &param);
  checkError(ret, "Failed to get part number");
  std::cout << "Part Number: " << reinterpret_cast<char *>(param.func.ucDevData)
            << std::endl;
}
