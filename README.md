# idrac_fancontrol
![image](https://user-images.githubusercontent.com/35542215/163832056-8633a6e9-9c24-433e-88a8-4ebd6ab5cc54.png)

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
Monitors temperatures, fan speeds and power of DELL PowerEdge R series servers in real time, printing instant data in stdout and updating with VT100 escape codes. And plots real time graphs in a X window via SDL2, using software rendering so it works over SSH X11 forwarding. Now it can transmit UDP packets reporting the data to a remote temp_monitor. Do note a bit of latency of the fan speed graph, being shifted from the control speed.
```
  --no-graph      Do not create a window
  --no-vt100      Do not use VT100 escape sequences
  --server        Transmit monitor data over UDP multicast
  --client        Receive monitor and optional control data
```

## fan_control
Reads temperatures, calculates a temperature average of N samples, and computes an appropiate fan speed for given temperature with a fan curve, and tells iDRAC to apply it. It is also able to transmit monitor and control data to a remote temp_monitor over UDP multicast. In my case, the default fan curve shuts the fans to a very quiet level of around 9-10% (~2000rpm) , while maintaining a cool 45ºC on both sockets, with a 19ºC inlet.
```
  --no-graph      Do not create a window
  --no-vt100      Do not use VT100 escape sequences
  --server        Transmit monitor and control data over UDP multicast
```
