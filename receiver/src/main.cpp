#include "../../configuration/include/configuration.hpp"
#include "../include/receive.hpp"
#include <iostream>
#include <string>
#include <thread>

int main() {

  CANConfiguraton conf;
  // Initialize CAN configuration with receiver device name from JSON
  conf.initialize(conf.getJsonData()["receiverDeviceName"]);
  conf.isCanFdCapable();
  conf.getSerialNumber();
  conf.getHCDeviceNumber();
  conf.getFirmwareVersionFromDriver();
  conf.getAdapterName();
  conf.getPartNumber();
  CANReceive receive(conf.getFd());
  // Remove all existing message filters before starting
  receive.deleteAllFilters();
  int filterOption;
  int confOption;
  while (true) {

    std::cout << "---------CAN FD Transmitter---------" << std::endl;
    std::cout << "0- Continue with Default Can Initial Parameters\n"
                 "1- Configure Can Initial Parameters\n"
                 "2- Set Can Options\n"
                 "3- Get Can Options\n"
                 "4- Show Bus State\n"
                 "5- Exit"
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
      int setOption;
      std::cout << "0- Set Device Id\n"
                   "1- Set Flash LED time(ms)\n"
                   "2- Set Allowed Message Type\n"
                   "3- Set Acceptance Filter for CAN2.0\n"
                   "4- Set Acceptance Filter For CAN Ext\n"
                   "5- Set IFrame Delay Time\n"
                   "6- Set Hardware Time Stamp Mode\n"
                   "7- Set Mass Storage Mode\n"
                   "8- Set Driver Clock Reference Time(μs)\n"
                   "9- Set Linger Time(μs)\n"
                   "10- Set Self Acknowledge Mode\n"
                   "11- Set BRS Ignore Mode"
                << std::endl;

      std::cout << "Enter a valid number: ";
      std::cin >> setOption;

      switch (setOption) {
      case 0: {
        int deviceId;
        std::cout << "Device Id: ";
        std::cin >> deviceId;
        conf.setDeviceId(deviceId);
        break;
      }
      case 1: {
        int flashLed;
        std::cout << "Flash LED(ms): ";
        std::cin >> flashLed;
        conf.setFlashLed(flashLed);
        break;
      }
      case 2: {
        std::string allowedMsg;
        std::cout << "Allowed Message Options\n"
                     "0 = PCANFD_ALLOWED_MSG_CAN\n"
                     "1 = PCANFD_ALLOWED_MSG_RTR\n"
                     "2 = PCANFD_ALLOWED_MSG_EXT\n"
                     "3 = PCANFD_ALLOWED_MSG_STATUS\n"
                     "4 = PCANFD_ALLOWED_MSG_ERROR\n"
                     "Message Type can be selected single or multiple.For "
                     "multiple choose, Please use comma(,)\n"
                     "Allowed Message Type: ";
        std::cin >> allowedMsg;
        conf.setAllowedMsgs(allowedMsg);
        break;
      }
      case 3: {
        uint32_t code, mask;
        std::cout << "Acceptance Filter (CAN 2.0)\nCode: \n";
        std::cin >> code;
        std::cout << "Mask: \n";
        std::cin >> mask;
        conf.setAccFilter11B(code, mask);
        break;
      }
      case 4: {
        uint32_t code, mask;
        std::cout << "Acceptance Filter (CAN Ext)\nCode: \n";
        std::cin >> code;
        std::cout << "Mask: \n";
        std::cin >> mask;
        conf.setAccFilter29B(code, mask);
        break;
      }
      case 5: {
        int frameDelay;
        std::cout << "IFrame Delay Time(us): ";
        std::cin >> frameDelay;
        conf.setIFrameDelay(frameDelay);
        break;
      }
      case 6: {
        std::string flag;
        std::cout
            << "Hardware Timestamp  Options\n"
               "0 = PCANFD_OPT_HWTIMESTAMP_OFF - Host time only\n"
               "1 = PCANFD_OPT_HWTIMESTAMP_ON - Host time + raw HW offset\n"
               "2 = PCANFD_OPT_HWTIMESTAMP_COOKED - Host time + cooked HW "
               "offset (drift correction)\n"
               "3 = PCANFD_OPT_HWTIMESTAMP_RAW - Raw HW timestamp (not host "
               "related)\n"
               "4 = PCANFD_OPT_HWTIMESTAMP_SOF_ON - SOF + raw HW offset\n"
               "5 = PCANFD_OPT_HWTIMESTAMP_SOF_COOKED - SOF + cooked HW "
               "offset\n"
               "6 = PCANFD_OPT_HWTIMESTAMP_SOF_RAW -  SOF + raw timestamp\n"
               "Message Type can be selected only single.\n"
               "Hardware Timestamp Mode : ";
        std::cin >> flag;
        conf.setHwTimestampMode(flag);
        break;
      }
      case 7: {
        std::string massStorageMode;
        std::cout << "Mass Storage Mode (0 - Disable, 1 - Enable): ";
        std::cin >> massStorageMode;
        conf.setMassStorageMode(massStorageMode);
        break;
      }
      case 8: {
        uint32_t drvClockRef;
        std::cout << "Driver Clock Reference: ";
        std::cin >> drvClockRef;
        conf.setDrvClockRef(drvClockRef);
        break;
      }
      case 9: {
        std::string lingerValue;
        std::cout << "Linger Mode Options\n"
                     "0 = CANFD_OPT_LINGER_NOWAIT\n"
                     "1 = PCANFD_OPT_LINGER_AUTO\n"
                     "Linger Mode : ";
        std::cin >> lingerValue;
        conf.setLinger(lingerValue);
        break;
      }
      case 10: {
        std::string selfAck;
        std::cout << "Self Ack (0 - Disable, 1 - Enable): ";
        std::cin >> selfAck;
        conf.setSelfAck(selfAck);
        break;
      }
      case 11: {
        std::string brsIgnore;
        std::cout << "BRS Ignore (0 - Disable, 1 - Enable): ";
        std::cin >> brsIgnore;
        conf.setBRSIgnore(brsIgnore);
        break;
      }
      default:
        std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      }
      continue;

    } else if (confOption == 3) {
      conf.getCanClocks();
      conf.getBitTimingRanges();
      conf.getChannelFeatures();
      conf.getDeviceId();
      conf.getAcceptanceFilter();
      conf.getFrameDelayTime();
      conf.getTimeStampMode();
      conf.getDriverVersion();
      conf.getFirmwareVersion();
      conf.getIoInfo();
      continue;
    } else if (confOption == 4) {
      // Display current CAN device state and statistics
      std::cout << std::endl;
      conf.getCanState();
      std::cout << std::endl;
      continue;
    } else if (confOption == 5) {
      // Exit the program
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