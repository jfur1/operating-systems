John Furlong
CSCI 3753
Programming Assignment 2
Student ID: 106380738

pa2_char_driver.c
    This is the character device driver file, which can be compiled using < sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules >
    Then, install using < sudo insmod pa2_char_driver.ko >

uspace.c
    This is the userspace test application for the character device driver
    Compile using gcc

Makefile
    Initializes the object file for the device driver compilation
