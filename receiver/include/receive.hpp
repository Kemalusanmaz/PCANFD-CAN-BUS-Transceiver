#ifndef __RECEIVE_HPP__
#define __RECEIVE_HPP__

#include <sys/time.h>
#include "../../driver/peak-linux-driver-8.20.0/driver/pcanfd.h"
#include <string>

/**
 * @class CANReceive
 * @brief Handles CAN message reception and filtering using PCAN FD interface.
 */
class CANReceive {
public:
  CANReceive(int fd);
  /**
   * @brief Adds a single message filter for a specified ID range.
   * @param idFrom Starting CAN ID in string format (hexadecimal).
   * @param idTo Ending CAN ID in string format (hexadecimal).
   */
  void addMsgFilter(std::string idFrom, std::string idTo);
  /**
   * @brief Adds predefined message filters.
   */
  void addMsgFilters();
  /**
   * @brief Adds a list of message filters using a predefined array.
   */
  void addMsgFiltersList();
  /**
   * @brief Deletes all existing CAN message filters.
   */
  void deleteAllFilters();
  /**
   * @brief Receives a single CAN message and stores it.
   */
  void receiveMessage();

private:
  int m_fd; // File descriptor for the CAN device.
  struct pcanfd_msg_filter msgFilter {}; // Structure for a single CAN message filter.
  struct pcanfd_msg_filter filters[2]; // Array of message filters.
  struct pcanfd_msg receivedMsg {}; // Structure to store the received CAN message.
};

#endif // RECEIVE_HPP
