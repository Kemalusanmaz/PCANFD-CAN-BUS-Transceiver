#include "../../configuration/include/configuration.hpp"
#include "../include/transmit.hpp"
#include <iostream>
#include <string>

int main() {
  CANConfiguraton conf;
  // Initialize CAN configuration with transmitter device name from JSON config
  conf.initialize(conf.getJsonData()["transmitterDeviceName"]);
  CANTransmit transmit(conf.getFd());
  int sendMsgOption;
  int confOption;

  while (true) {

    std::cout << "---------CAN FD Transmitter---------" << std::endl;
    std::cout
        << "0- Continue with Default Can Initial Parameters\n1- Configure "
           "Can Initial Parameters\n2- Exit"
        << std::endl;
    std::cout << "Enter a valid number: ";
    std::cin >> confOption;
    std::cout << std::endl;

    if (confOption == 0) {
      // Use default CAN configuration parameters
      conf.setCanConfig(confOption);
      std::cout << std::endl;
      conf.getCanConfig(); // Show current CAN configuration
      std::cout << std::endl;
    } else if (confOption == 1) {
      // Let user input custom CAN configuration parameters
      int clockHz;
      std::cout << "Clock Hz: ";
      std::cin >> clockHz;

      int nominalBitrate;
      std::cout << "Nominal Bitrate: ";
      std::cin >> nominalBitrate;

      int dataBitrate;
      std::cout << "Data Bitrate: ";
      std::cin >> dataBitrate;

      conf.setCanConfig(confOption, clockHz, nominalBitrate, dataBitrate);
      std::cout << std::endl;
      conf.getCanConfig(); // Show updated CAN configuration
      std::cout << std::endl;
    } else if (confOption == 2) {
      // Exit the program
      break;
    } else {
      std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      continue;
    }

    // Message sending options loop
    while (true) {
      std::cout << "0- Show Can State\n"
                   "1- Send Single Message\n"
                   "2- Send Multiple Message\n"
                   "3- Send Single Random Message\n"
                   "4- Configure Initial Can Parameters"
                << std::endl;

      std::cout << "Enter a valid number: ";
      std::cin >> sendMsgOption;

      if (sendMsgOption == 0) {
        // Display current CAN device state and statistics
        std::cout << std::endl;
        conf.getCanState();
        std::cout << std::endl;
      } else if (sendMsgOption == 1) {
        // Send a single CAN message with user input parameters
        int msgType;
        std::cout << "Message Type (0-CAN FD, 1-CAN 2.0): ";
        std::cin >> msgType;

        std::string id;
        std::cout << "Id: ";
        std::cin >> id;

        std::cout << "Data: ";
        std::string data;
        std::cin >> data;
        std::cout << std::endl;

        transmit.sendMessage(msgType, id, data);
      } else if (sendMsgOption == 2) {
        // Send multiple messages (implementation inside sendMessages())
        transmit.sendMessages();
      } else if (sendMsgOption == 3) {
        // Send a single random message with given parameters
        int msgType;
        std::cout << "Message Type (0-CAN FD, 1-CAN 2.0): ";
        std::cin >> msgType;

        std::string id;
        std::cout << "Id: ";
        std::cin >> id;

        int msgLength;
        std::cout << "Data Length: ";
        std::cin >> msgLength;

        transmit.sendRandomMessage(msgType, id, msgLength);
      } else if (sendMsgOption == 4) {
        // Return to CAN initial configuration menu
        break;
      } else {
        std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
        continue;
      }
    }
  }

  return 0;
}