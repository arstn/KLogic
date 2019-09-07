// ------------------------------------------------
// RAM device implementation
//
// Andreas Rostin
// 31.07.2003
// ------------------------------------------------
#include "deviceRAM.h"

const QString RAMDevice::IO_BUS_ADDRESS[8] = {"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7"};
const QString RAMDevice::INPUT_RAM_WRITE = "WR";
const QString RAMDevice::INTERNAL_RAM_ADDRESS = "ADDRESS";
const QString RAMDevice::INTERNAL_RAM_DATA = "DATA";

