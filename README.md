A software router with 2 ports.

FEATURES

arp, ping, dhcp, ripv2

DEPENDENCIES

libpcap, C comliper (clang is default), make
computer with multiple NICs
administrative rights

sudo apt-get install libpcap-dev clang make vim gdb git

Run
$ make
to create a binary inside bin folder.

Optionaly run
$ make debug
to open gdb


EXAMPLE

sudo ./bin/s_router -1 eth0 -2 wlan0
