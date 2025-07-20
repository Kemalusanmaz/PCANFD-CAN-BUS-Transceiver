#include "../include/receive.hpp"
#include "../../driver/peak-linux-driver-8.20.0/lib/libpcanfd.h" // PEAK-System FD API header file
#include <cstdlib>
#include <cstring> // memset()
#include <fcntl.h> // open()
#include <iomanip> // std::setw, std::setfill
#include <ios>
#include <iostream>
#include <malloc.h>
#include <ostream>
#include <pcanfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h> // close()
#include <vector>

CANReceive::CANReceive(int fd) { m_fd = fd; }

// Add a message filter for IDs from idFrom to idTo
void CANReceive::addMsgFilter(std::string idFrom, std::string idTo) {
  int idF = std::stoi(idFrom, nullptr, 16);
  msgFilter.id_from = idF; // Start of accepted message ID range

  int idT = std::stoi(idTo, nullptr, 16);
  msgFilter.id_to = idT; // End of accepted message ID range

  // Flag definitions:
  // PCANFD_MSG_STD 0x00000000 - Std message (11-bit ID), default message type
  // PCANFD_MSG_RTR 0x00000001 - RTR (Remote Transmission Request) message,
  // no data, only request
  // PCANFD_MSG_EXT 0x00000002 - Extended message (29-bit ID)
  // PCANFD_MSG_SLF 0x00000004 - Self-reception message (to receive your own
  // sent message)
  // PCANFD_MSG_SNG 0x00000008 - Single direction message (send only,no receive)
  // PCANFD_MSG_ECHO 0x00000010 - Receive echo of sent message used for debug
  msgFilter.msg_flags = PCANFD_MSG_STD; // Flags that the message must have

  int ret = pcanfd_add_filter(m_fd, &msgFilter);
  if (ret < 0) { // Non-zero means filter setup failed
    perror("pcanfd_add_filter");
    close(m_fd);
  } else {
    std::cout << "Message filter is added succesfully" << std::endl;
  }
}

// Add multiple filters dynamically allocated in memory
void CANReceive::addMsgFilters() {

  const int filterCount = 2;
  // Allocate memory for filters struct and filters together
  size_t totalSize = sizeof(struct pcanfd_msg_filters) +
                     filterCount * sizeof(struct pcanfd_msg_filter);
  struct pcanfd_msg_filters *filters =
      (struct pcanfd_msg_filters *)malloc(totalSize);

  if (!filters) {
    perror("Bellek ayrımı başarısız");
  }

  // Clear allocated memory
  memset(filters, 0, totalSize);
  filters->count = filterCount;

  // Filter 1: Only accept messages with ID 0x100
  filters->list[0].id_from = 0x100;
  filters->list[0].id_to = 0x100;
  filters->list[0].msg_flags = PCANFD_MSG_STD;

  // Filter 2: Accept messages with IDs between 0x200 and 0x2FF
  filters->list[1].id_from = 0x200;
  filters->list[1].id_to = 0x2FF;
  filters->list[1].msg_flags = PCANFD_MSG_STD;

  int ret = pcanfd_add_filters(m_fd, filters);
  if (ret < 0) { // Non-zero means failed to add filters
    perror("pcanfd_add_filters");
    close(m_fd);
  } else {
    std::cout << "Message filters can added succesfully" << std::endl;
  }
}

void CANReceive::addFilter(std::string idFrom, std::string idTo,
                           std::string flags) {
  struct pcanfd_msg_filter newFilter;
  int idF = std::stoi(idFrom, nullptr, 16);
  newFilter.id_from = idF;

  int idT = std::stoi(idTo, nullptr, 16);
  newFilter.id_to = idT;

  if (flags == "STD") {
    newFilter.msg_flags = PCANFD_MSG_STD;
  } else if (flags == "RTR") {
    newFilter.msg_flags = PCANFD_MSG_RTR;
  } else if (flags == "EXT") {
    newFilter.msg_flags = PCANFD_MSG_EXT;
  } else if (flags == "SLF") {
    newFilter.msg_flags = PCANFD_MSG_SLF;
  } else if (flags == "SNG") {
    newFilter.msg_flags = PCANFD_MSG_SNG;
  } else if (flags == "ECHO") {
    newFilter.msg_flags = PCANFD_MSG_ECHO;
  } else {
  }

  m_filters.push_back(newFilter);
  std::cout << "Message Flags: " << flags << " ID: " << std::hex << idFrom
            << " to " << idTo << std::dec << " added to filter list. "
            << std::endl;
}

// Add filters from a predefined list (array)
void CANReceive::addMsgFiltersList() {

  int statusMsgFilters =
      pcanfd_add_filters_list(m_fd, m_filters.size(), m_filters.data());
  if (statusMsgFilters < 0) {
    perror("pcanfd_add_filters_list");
  } else {
    // std::cout << "Message filter list can added succesfully" << std::endl;
  }
}

// Delete all filters on the CAN device
void CANReceive::deleteAllFilters() {
  int ret = pcanfd_del_filters(m_fd);
  if (ret < 0) {
    perror("pcanfd_del_filters");
  } else {
    m_filters.clear();
    std::cout << "All filters are deleted successfully" << std::endl;
  }
}

void CANReceive::getFilterList() {
  int i = 0;
  std::string msgFltr;
  if (m_filters.size() != 0) {

    for (auto &el : m_filters) {
      if (el.msg_flags == PCANFD_MSG_STD) {
        msgFltr = "STD";
      } else if (el.msg_flags == PCANFD_MSG_RTR) {
        msgFltr = "RTR";
      } else if (el.msg_flags == PCANFD_MSG_EXT) {
        msgFltr = "EXT";
      } else if (el.msg_flags == PCANFD_MSG_SLF) {
        msgFltr = "SLF";
      } else if (el.msg_flags == PCANFD_MSG_SNG) {
        msgFltr = "SNG";
      } else if (el.msg_flags == PCANFD_MSG_ECHO) {
        msgFltr = "ECHO";
      } else {
      }

      std::cout << "Filter " << i + 1 << ": " << "Message Flag: " << msgFltr
                << " ID: " << std::hex << el.id_from << " to " << el.id_to
                << std::endl;
    }
  } else {
    std::cout << "There is no message filter added!";
  }
}

// Receive a message from the CAN device and print its details
void CANReceive::receiveMessage() {
  int ret = pcanfd_recv_msg(m_fd, &receivedMsg);
  if (ret < 0) {
    // perror("Message can not be read");
  } else {
    // std::cout << "Message is received!" << std::endl;
    if (receivedMsg.type == PCANFD_TYPE_CAN20_MSG) {
      std::cout << "Message Type: CAN 2.0" << std::endl;
    } else if (receivedMsg.type == PCANFD_TYPE_CANFD_MSG) {
      std::cout << "Message Type: CAN FD" << std::endl;
    } else {
      std::cout << "Unknown Message Type!" << std::endl;
    }
    std::cout << "ID: 0x" << std::hex << receivedMsg.id << std::dec
              << std::endl;
    std::cout << "Data Length: " << receivedMsg.data_len << std::endl;
    std::cout << "Data: ";
    for (int i = 0; i < receivedMsg.data_len; ++i) {
      std::cout << "0x" << std::hex // Hexadecimal moda geç
                << std::setw(2)     // Genişliği 2 karaktere ayarla
                << std::setfill('0') // Boşlukları '0' ile doldur (örn: 5 -> 05)
                << static_cast<int>(
                       receivedMsg.data[i]) // uint8_t'yi int'e çevirerek yazdır
                << " ";
    }
    std::cout << std::dec << std::endl;
    std::cout << "Flags: 0x" << std::hex << receivedMsg.flags << std::dec
              << std::endl;
    std::cout << std::endl;
  }
}

void CANReceive::receiveMessageList() {
  const int max_msgs = 5;

  // reserved for received messages.
  struct pcanfd_msg msgs[max_msgs];

  // Read messages
  int received = pcanfd_recv_msgs_list(m_fd, max_msgs, msgs);

  if (received < 0) {
    std::cerr << "Message can not received, Error Code:: " << received << "\n";
  }

  // process received messages.
  for (int i = 0; i < received; ++i) {
    processMsg(&msgs[i]);
  }
}

void CANReceive::receiveMessages() {
  // reserved for received messages
  struct pcanfd_msgs *pml = (struct pcanfd_msgs *)malloc(
      sizeof(*pml) + 5 * sizeof(struct pcanfd_msg));
  if (!pml) {
    std::cerr << "Buffer can not arranged\n";
  }

  pml->count = 5;

  // Read messages
  int err = pcanfd_recv_msgs(m_fd, pml);
  if (err) {
    std::cerr << "Message can not received, Error Code: " << err << "\n";
    free(pml);
  }

  // process received messages.
  for (int i = 0; i < static_cast<int>(pml->count); ++i) {
    processMsg(&(pml->list[i]));
  }

  free(pml);
}

int CANReceive::processMsg(struct pcanfd_msg *pm) {
  switch (pm->type) {
  case PCANFD_TYPE_CAN20_MSG:
    std::cout << "CAN 2.0 message is received\n";
    return 0;
  case PCANFD_TYPE_CANFD_MSG:
    std::cout << "CAN FD message is received\n";
    return 0;
  case PCANFD_TYPE_STATUS:
    std::cout << "Status message is received\n";
    return 0;
  case PCANFD_TYPE_ERROR_MSG:
    std::cout << "Error message is received\n";
    return 0;
  }
  return -EINVAL;
}
