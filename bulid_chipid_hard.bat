@echo off
"./tcc/tcc" -Istlink-static/include -Istlink-static/include/libusb-1.0 -I. -run build_chipid_hard.c 
