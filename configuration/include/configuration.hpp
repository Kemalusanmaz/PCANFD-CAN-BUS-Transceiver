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
  /**
   * @brief Opens the CAN device with specified nominal/data bitrate and clock.
   * @param deviceStr Path to the CAN device (e.g., "/dev/pcanusbprofd0").
   * @param nominalBitrate Bitrate for arbitration phase (in bits/s).
   * @param dataBitrate Bitrate for data phase (in bits/s).
   * @param clockHz Clock frequency used by the CAN controller (in Hz).
   */
  void canOpen(std::string deviceStr, uint32_t nominalBitrate,
               uint32_t dataBitrate, uint32_t clockHz);

  /**
   * @brief Retrieves and prints the available CAN controller clocks.
   */
  void getCanClocks();
  /**
   * @brief Retrieves and prints the supported bit timing ranges.
   */
  void getBitTimingRanges();
  /**
   * @brief Retrieves and prints the features supported by the CAN channel.
   */
  void getChannelFeatures();
  /**
   * @brief Retrieves and prints the current device ID.
   */
  void getDeviceId();
  /**
   * @brief Retrieves and prints the current acceptance filter settings.
   */
  void getAcceptanceFilter();
  /**
   * @brief Retrieves and prints the current inter-frame delay time.
   */
  void getFrameDelayTime();
  /**
   * @brief Retrieves and prints the current hardware timestamp mode.
   */
  void getTimeStampMode();
  /**
   * @brief Retrieves and prints the driver version.
   */
  void getDriverVersion();
  /**
   * @brief Retrieves and prints the firmware version of the CAN device.
   */
  void getFirmwareVersion();
  /**
   * @brief Retrieves and prints miscellaneous IO information.
   */
  void getIoInfo();
  /**
   * @brief Sets a user-defined device ID for the CAN device.
   * @param deviceId New device ID to be set.
   */
  void setDeviceId(uint32_t deviceId);
  /**
   * @brief Sets the duration for the device's flash LED in milliseconds.
   * @param durationMs Duration in milliseconds.
   */
  void setFlashLed(uint32_t durationMs);
  /**
   * @brief Sets the allowed message types for transmission/reception.
   * @param flag Comma-separated string of allowed message flags.
   */
  void setAllowedMsgs(std::string flag);
  /**
   * @brief Sets the acceptance filter for 11-bit (standard) CAN IDs.
   * @param code Acceptance code.
   * @param mask Acceptance mask.
   */
  void setAccFilter11B(uint32_t code, uint32_t mask);
  /**
   * @brief Sets the acceptance filter for 29-bit (extended) CAN IDs.
   * @param code Acceptance code.
   * @param mask Acceptance mask.
   */
  void setAccFilter29B(uint32_t code, uint32_t mask);
  /**
   * @brief Sets the inter-frame delay time in microseconds.
   * @param delayUs Delay time in microseconds.
   */
  void setIFrameDelay(uint32_t delayUs);
  /**
   * @brief Sets the hardware timestamp mode.
   * @param flag String representing the selected timestamp mode.
   */
  void setHwTimestampMode(std::string flag);
  /**
   * @brief Enables or disables mass storage mode.
   * @param flag "0" to disable, "1" to enable.
   */
  void setMassStorageMode(std::string flag);
  /**
   * @brief Sets the driver's clock reference time in microseconds.
   * @param clkRef Clock reference time in microseconds.
   */
  void setDrvClockRef(uint32_t clkRef);
  /**
   * @brief Sets the linger mode (wait behavior for closing).
   * @param lingerValue Linger option as a string ("0" or "1").
   */
  void setLinger(std::string lingerValue);
  /**
   * @brief Enables or disables self-acknowledgement mode.
   * @param enable "0" to disable, "1" to enable.
   */
  void setSelfAck(std::string enable);
  /**
   * @brief Enables or disables ignoring BRS (bit rate switch) errors.
   * @param enable "0" to disable, "1" to enable.
   */
  void setBRSIgnore(std::string enable);
  /**
   * @brief Checks if the device is CAN-FD capable and prints result.
   */
  void isCanFdCapable();
  /**
   * @brief Retrieves and prints the device's serial number.
   */
  void getSerialNumber();
  /**
   * @brief Retrieves and prints the hardware-configurable device number.
   */
  void getHCDeviceNumber();
  /**
   * @brief Sets the hardware-configurable device number in flash memory.
   * @param devN The new device number.
   */
  void setHCDeviceNumber(uint8_t devN);
  /**
   * @brief Retrieves and prints the firmware version from driver.
   */
  void getFirmwareVersionFromDriver();
  /**
   * @brief Retrieves and prints the adapter name of the CAN device.
   */
  void getAdapterName();
  /**
   * @brief Retrieves and prints the part number of the CAN device.
   */
  void getPartNumber();

private:
  /**
   * @brief Parses the CAN configuration from a JSON file or source.
   */
  void jsonParser();
  /**
   * @brief Utility function to check the return code from driver calls.
   * @param ret Return code.
   * @param msg Descriptive error message to print if an error occurred.
   */
  void checkError(int ret, const char *msg);
  /**
   * @brief Internal helper to fetch and print I/O information.
   * @param name Information code defined by driver.
   * @param msg Description string to identify the printed info.
   */
  void getIoInfoProcess(int name, std::string msg);
  int fd{};                         // File descriptor for the CAN device.
  struct pcanfd_init initConfig {}; // PCAN FD initialization configuration.
  struct pcanfd_state canState {};  // CAN controller state structure.
  nlohmann::json jsonData;          // Parsed JSON configuration data.
  struct pcan_extra_params
      param; // Structure for accessing extended parameters using ioctl.
  struct pcanfd_available_clocks
      *pac; // Pointer to structure holding available CAN controller clocks.
  struct pcanfd_bittiming_ranges
      *pbr; // Pointer to structure holding supported bit timing ranges.
};

#endif // CONFIGURATION_HPP
