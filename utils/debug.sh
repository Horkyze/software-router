#!/bin/bash

cd ..
make
cd utils
sudo gdb -q --command=debug_commands.gdb ../bin/s_router
