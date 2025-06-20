#ifndef __CONFIGURATION_HPP__
#define __CONFIGURATION_HPP__

#include <sys/time.h>
#include "../../driver/peak-linux-driver-8.20.0/driver/pcanfd.h"
#include <nlohmann/json.hpp>

/**
 * @class CANConfiguraton
 * @brief Handles initialization, configuration, and termination of a PCAN FD
 * CAN device.
 */
class CANConfiguraton {
public:
  CANConfiguraton();
  /**
   * @brief Initializes the CAN device using the specified device path.
   * @param deviceStr Device path string (e.g., "/dev/pcanpcifd0").
   */
  void initialize(std::string deviceStr);
  /**
   * @brief Sets CAN interface configuration with custom parameters.
   * @param flag Configuration flags (e.g., PCANFD_INIT_xxx).
   * @param clockHz Optional clock frequency (Hz).
   * @param nominalBitrate Optional nominal bitrate (bit/s).
   * @param dataBitrate Optional data bitrate for CAN FD (bit/s).
   */
  void setCanConfig(int flag, int clockHz = 0, int nominalBitrate = 0,
                    int dataBitrate = 0);
  /**
   * @brief Gets and prints the current CAN configuration.
   */
  void getCanConfig();
  /**
   * @brief Gets and prints the current CAN controller state.
   */
  void getCanState();
  /**
   * @brief Closes the CAN interface and releases resources.
   */
  void terminate();
  /**
   * @brief Returns the file descriptor of the CAN device.
   * @return Integer file descriptor.
   */
  int getFd() const { return fd; }
  /**
   * @brief Returns the parsed JSON configuration data.
   * @return JSON object.
   */
  nlohmann::json getJsonData() const { return jsonData; }

  void getCanClocks();

  void getBitTimingRanges();

  void getChannelFeatures();

  void getDeviceId();

  void getAcceptFilter();

  void getFrameDelayTime();

  void getTimeStampMode();

  void getDriverVersion();

  void getFirmwareVersion();

  void getIoInfo();

  void setDeviceId(uint32_t deviceId);

  void setFlashLed(uint32_t durationMs);

  void setAllowedMsgs(std::string flag);

  void setAccFilter11B(uint32_t code, uint32_t mask);

  void setAccFilter29B(uint32_t code, uint32_t mask);

  void setIFrameDelay(uint32_t delayUs);

  void setHwTimestampMode(std::string flag);

  void setMassStorageMode(std::string flag);

  void setDrvClockRef(uint32_t clkRef);

  void setLinger(std::string lingerValue);

  void setSelfAck(std::string enable);

  void setBRSIgnore(std::string enable);

private:
  int fd{};                         // File descriptor for the CAN device.
  struct pcanfd_init initConfig {}; // PCAN FD initialization configuration.
  struct pcanfd_state canState {};  // CAN controller state structure.
  nlohmann::json jsonData;          // Parsed JSON configuration data.
  /**
   * @brief Parses the CAN configuration from a JSON file or source.
   */
  void jsonParser();
  struct pcanfd_available_clocks *pac;
  struct pcanfd_bittiming_ranges *pbr;
  void checkError(int ret, const char *msg);

  void getIoInfoProcess(int name, std::string msg);
};

#endif // CONFIGURATION_HPP
