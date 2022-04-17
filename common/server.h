#pragma once

#include "conf.h"

#include <string>
#include <stdint.h>

extern bool serverInit(std::string dstAddr, unsigned short port);
extern bool serverSend(Packet packet);
