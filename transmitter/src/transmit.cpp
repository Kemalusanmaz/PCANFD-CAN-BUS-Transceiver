#include "../include/transmit.hpp"
#include "../../driver/peak-linux-driver-8.20.0/lib/libpcanfd.h" // PEAK-System FD API header
#include <cstdlib>
#include <cstring> // memset()
#include <fcntl.h> // open()
#include <iostream>
#include <malloc.h>
#include <ostream>
#include <pcanfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h> // close()

// Initializes CANTransmit object with given file descriptor.
// Seeds the random number generator.
CANTransmit::CANTransmit(int fd) {
  m_fd = fd;
  srand(time(0)); // Should be called only once
}

// Sends a CAN message using messageType (0 = CAN FD, 1 = CAN 2.0),
// CAN ID (hex string), and data (hex string).
void CANTransmit::sendMessage(int messageType, std::string id,
                              std::string data) {
  // Select message type
  if (messageType == 0) {
    canfdMsg.type = PCANFD_TYPE_CANFD_MSG;
  } else if (messageType == 1) {
    canfdMsg.type = PCANFD_TYPE_CAN20_MSG;
  }

  int i = std::stoi(id, nullptr, 16); // Convert hex string ID to integer
  canfdMsg.id = i;

  canfdMsg.data_len = data.length() / 2; // Calculate number of bytes in data

  // Convert data hex string to byte array
  for (size_t i = 0; i < data.length(); i += 2) {
    std::string byteString = data.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
    canfdMsg.data[i / 2] = byte;
  }
  canfdMsg.flags =
      PCANFD_MSG_STD; // Defines whether the message is remote transmission
                      // (RTR), uses a 29-bit ID (EXT), or requests the data
                      // section to be sent quickly (BRS) Default Standard
                      // message (11-bit ID)
  int ret = pcanfd_send_msg(m_fd, &canfdMsg);
  // Send the message
  if (ret < 0) {
    perror("Message cannot sent!");
  } else {
    std::cout << "Message is sent successfully." << std::endl;
  }
}

// Sends a CAN message with randomly generated data of given length.
// messageType: 0 = CAN FD, 1 = CAN 2.0
void CANTransmit::sendRandomMessage(int messageType, std::string id,
                                    int dataLength) {
  // Select message type
  if (messageType == 0) { // Defines whether CAN 2.0 or CAN FD will be sent.
    canfdMsg.type = PCANFD_TYPE_CANFD_MSG;
  } else if (messageType == 1) {
    canfdMsg.type = PCANFD_TYPE_CAN20_MSG;
  }
  // Convert ID from hex string
  int i = std::stoi(id, nullptr, 16);
  canfdMsg.id = i;                // ID
  canfdMsg.data_len = dataLength; // Data length

  // Generate random hex string
  std::string hexString;
  const char hex[] = "0123456789ABCDEF";
  for (int i = 0; i < dataLength * 2; ++i) {
    hexString += hex[rand() % 16];
  }

  // convert 1 byte from 2 char
  for (int i = 0; i < dataLength - 1; ++i) {
    std::string byteStr = hexString.substr(i * 2, 2);
    canfdMsg.data[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
  }
  canfdMsg.flags =
      PCANFD_MSG_STD; // Defines whether the message is remote transmission
                      // (RTR), uses a 29-bit ID (EXT), or requests the data
                      // section to be sent quickly (BRS) Default Standard
                      // message (11-bit ID)

  // Send message
  int ret = pcanfd_send_msg(m_fd, &canfdMsg);
  if (ret < 0) {
    perror("Message cannot sent!");
  } else {
    std::cout << "Message is sent successfully." << std::endl;
  }
}

// Sends two predefined CAN messages using a message list.
void CANTransmit::sendMessageList() {

  const int numberOfMessages = 2; // Number of messages to be sent.

  // First message: CAN 2.0 standard message
  canfdMsgList[0].type = PCANFD_TYPE_CAN20_MSG;
  canfdMsgList[0].id = 0x123;
  canfdMsgList[0].flags = PCANFD_MSG_STD;
  canfdMsgList[0].data_len = 3;
  canfdMsgList[0].data[0] = 0x11;
  canfdMsgList[0].data[1] = 0x22;
  canfdMsgList[0].data[2] = 0x33;

  // Second message: CAN FD extended + BRS (bit rate switching)
  canfdMsgList[1].type = PCANFD_TYPE_CANFD_MSG;
  canfdMsgList[1].id = 0x456;
  canfdMsgList[1].flags = PCANFD_MSG_EXT | PCANFD_MSG_BRS;
  canfdMsgList[1].data_len = 6;
  canfdMsgList[1].data[0] = 0xAA;
  canfdMsgList[1].data[1] = 0xBB;
  canfdMsgList[1].data[2] = 0xCC;
  canfdMsgList[1].data[3] = 0xDD;
  canfdMsgList[1].data[4] = 0xEE;
  canfdMsgList[1].data[5] = 0xFF;

  // Send both messages together
  int ret = pcanfd_send_msgs_list(m_fd, numberOfMessages, canfdMsgList);
  if (ret < 0) {
    perror("Messages cannot be sent");
  } else {
    printf("%d message is sent successfully.\n", numberOfMessages);
  }
}

// Sends 5 messages using dynamically allocated message list
void CANTransmit::sendMessages() {
  // Allocate memory for 5 messages
  canfdMsgs = (struct pcanfd_msgs *)malloc(sizeof(*canfdMsgs) +
                                           5 * sizeof(struct pcanfd_msg));

  if (!canfdMsgs) {
    perror("malloc");
  }

  canfdMsgs->count = 5;

  // Fill each message with predefined data
  for (canfdMsgs->count = 0; canfdMsgs->count < 5; canfdMsgs->count++) {
    fill_msg(canfdMsgs->list + canfdMsgs->count);
  }

  // Send all messages at once
  int err = pcanfd_send_msgs(m_fd, canfdMsgs);

  if (err) {
    // If there is an error, report how many messages were sent
    printf(" Only %u/5 message can be sent, Error Code: %d\n", canfdMsgs->count,
           err);
  } else {
    printf(" %u message is sent successfully!\n", canfdMsgs->count);
  }

  free(canfdMsgs);
}

// Fills a single message with sample CAN 2.0 standard frame
int CANTransmit::fill_msg(struct pcanfd_msg *pm) {
  pm->type = PCANFD_TYPE_CAN20_MSG; // CAN 2.0 message
  pm->id = 0x100;                   // ID
  pm->flags = PCANFD_MSG_STD;       // standard frame
  pm->data_len = 3;                 // data length
  pm->data[0] = 0x11;
  pm->data[1] = 0x22;
  pm->data[2] = 0x33;
  return 0;
}
