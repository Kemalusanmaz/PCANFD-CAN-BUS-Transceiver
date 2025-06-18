#include "../../configuration/include/configuration.hpp"
#include "../include/receive.hpp"
#include <iostream>
#include <string>
#include <thread>

int main() {

  CANConfiguraton conf;
  // Initialize CAN configuration with receiver device name from JSON
  conf.initialize(conf.getJsonData()["receiverDeviceName"]);
  CANReceive receive(conf.getFd());
  // Remove all existing message filters before starting
  receive.deleteAllFilters();
  int filterOption;
  int confOption;
  while (true) {

    std::cout << "---------CAN FD Receiver---------" << std::endl;
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
      conf.getCanConfig();
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
      conf.getCanConfig();
      std::cout << std::endl;
    } else if (confOption == 2) {
      // Exit the program loop
      break;
    } else {
      std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      continue;
    }

    while (true) {

      std::cout << "0- Show Can State\n1- Add Single Filter\n2- Add Filter "
                   "List\n3- Delete "
                   "Filter\n4- Continue Without Any Filter "
                << std::endl;
      std::cout << "Enter a valid number: ";
      std::cin >> filterOption;
      if (filterOption == 0) {
        // Display the current CAN device state and statistics
        std::cout << std::endl;
        conf.getCanState();
        std::cout << std::endl;
        continue;
        ;
      } else if (filterOption == 1) {
        // Add a single message filter by specifying ID range
        std::string idFrom;
        std::cout << "Id From: ";
        std::cin >> idFrom;

        std::string idTo;
        std::cout << "Id To: ";
        std::cin >> idTo;

        receive.addMsgFilter(idFrom, idTo);
        break;
      } else if (filterOption == 2) {
        // Add a list of message filters (TODO)
        receive.addMsgFiltersList();
        break;
      } else if (filterOption == 3) {
        // Delete all existing message filters
        receive.deleteAllFilters();
        break;
      } else if (filterOption == 4) {
        // Continue without applying any filters
        break;
      } else {
        std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
        continue;
      }
      std::cout << std::endl;
    }
    while (true) {
      // Receive messages continuously with 100 ms delay
      receive.receiveMessage();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // close the CAN device before exiting
    conf.terminate();
  }
  return 0;
}