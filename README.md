# idrac_fancontrol
Temperature monitor and fan speed controller for DELL PowerEdge servers through iDRAC7/IPMI

## Build
Just a standard CMake project, simply:
```
mkdir build
cd build
cmake ..
make
```

## temp_monitor
Monitors temperatures, fan speeds and power of DELL PowerEdge R series servers in real time, printing instant data in stdout and updating with VT100 escape codes. And plots real time graphs in a X window via SDL2, using software rendering so it works over SSH X11 forwarding.
