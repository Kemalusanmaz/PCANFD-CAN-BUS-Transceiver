#ifndef __TRANSMIT_HPP__
#define __TRANSMIT_HPP__

#include <sys/time.h>
#include "../../driver/peak-linux-driver-8.20.0/driver/pcanfd.h"
#include "string"

/**
 * @class CANTransmit
 * @brief Handles CAN message transmission using PCAN FD interface.
 */
class CANTransmit {
public:
  CANTransmit(int fd);
  /**
   * @brief Sends a CAN message with specified type, ID, and data.
   * @param messageType Type of message (e.g., standard or extended).
   * @param id CAN ID in string format (hexadecimal).
   * @param data Payload data as a hexadecimal string.
   */
  void sendMessage(int messageType, std::string id, std::string data);
  /**
   * @brief Sends a CAN message with random data of given length.
   * @param messageType Type of message (e.g., standard or extended).
   * @param id CAN ID in string format (hexadecimal).
   * @param dataLength Number of random data bytes to send.
   */
  void sendRandomMessage(int messageType, std::string id, int dataLength);
  /**
   * @brief Sends a series of predefined CAN messages.
   */
  void sendMessages();
  /**
   * @brief Sends a list of two predefined CAN messages using an array.
   */
  void sendMessageList();

private:
  int m_fd; // File descriptor for the CAN device.
  struct pcanfd_msg canfdMsg {}; // Single CAN FD message structure.
  struct pcanfd_msgs *canfdMsgs; // Pointer to a list of CAN FD messages.
  struct pcanfd_msg canfdMsgList[2]; // Array to hold multiple CAN FD messages.
  /**
   * @brief Helper function to fill a CAN FD message structure with data.
   * @param pm Pointer to the CAN FD message to be filled.
   * @return 0 on success, negative value on failure.
   */
  int fill_msg(struct pcanfd_msg *pm);
};

#endif // TRANSMIT_HPP
