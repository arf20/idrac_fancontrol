# idrac_fancontrol
Temperature monitor and fan speed controller for DELL PowerEdge R series servers through iDRAC/IPMI. Although it could be adapted to other DELL IPMI capable servers, or even non iDRAC DELL servers like iLO HP, Supermicro or Fujitsu servers. Multiplataform support for those might be added in the future, as for now its hardcoded.

Initially, I tried using OpenIPMI, but it didn't work, so now I talk to iDRAC via this nice CLI tool named ipmitool, you will need it:
```
apt install ipmitool
```
Note: use whatever other package manager you have if you are not in a Debian based distro, these packages are fairly common.

## Build
Just a standard CMake project, simply install dependencies:
```
apt install libsdl2-dev libsdl2-ttf-dev
```
and
```
mkdir build
cd build
cmake ..
make
```

## temp_monitor
Monitors temperatures, fan speeds and power of DELL PowerEdge R series servers in real time, printing instant data in stdout and updating with VT100 escape codes. And plots real time graphs in a X window via SDL2, using software rendering so it works over SSH X11 forwarding.
