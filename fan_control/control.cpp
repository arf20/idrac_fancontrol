#include "../common/conf.h"
#include "../common/exec.h"

#include <iomanip>
#include <sstream>

// percent  iDRAC value is just percent in hex
// 100%     0x64
// 25&      0x19

bool controlAcquired = false;

void acquireControl() {
    exec(ipmiAcquireControlCommand);
    controlAcquired = true;
}

void returnControl() {
    exec(ipmiReturnControlCommand);
    controlAcquired = false;
}

void setSpeed(int percent) {
    if (!controlAcquired) return;
    std::stringstream ss;
    ss << std::setfill ('0') << std::setw(2) << std::hex << percent;
    exec(ipmiSetSpeedCommand + ss.str());
}