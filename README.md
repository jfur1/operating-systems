# Operating Systems

Assignments from Operating Systems (Linux-based Kernel)

## Table of Contents
### <ins>Programming Assignment 1: Create a System Call</ins>

  * **Description:**
      Write a new system call that is given two numbers, and an address to store the result. Add the new system call to the system table of the kernel as well as the system call header file. Then, recompile the kernel and write a C function to test. Output can be checked in the system log. 

  * **Build Tree:**

    - `cs3753_add.c`
        > C program which takes as input two numbers, and an address to store the result.

    - `syscall_64.tbl`
        > Kernel system table with new system call

    - `syscalls.h:`
        > Linker definitions for headers

    - `Makefile`
        > GNU makefile to build the program -- <make clean> to reset

  * **Build Instructions:**

    1. Run Makefile to compile

    2.	Execute using the following format:
	
	      `./a.out`

    3. Check system log for output:
	
	     `/var/log/syslog`

### <ins>Programming Assignment 2: Creating a Device Driver</ins>
* **Description:** Write, install, and run a Linux kernel module (LKM). Then, write a module to install a Linux character device driver. Lastly, write a user-space test program that allows user to read from, write to, or seek in the device file we created within `\dev`. 

* **Build Tree:**
    - `pa2_char_driver.c`
      > This is the character device driver file, which gets placed inside of `\dev` folder associated with the device driver.

    - `uspace.c`
      > This is the userspace test application for the character device driver.

     - `Makefile`
        > Initializes the object file for the device driver compilation (gcc).

* **Build Instructions:**
    1. Compile
    
        `sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules`
        
    2. Install
    
        `sudo insmod pa2_char_driver.ko`

### <ins>Programming Assignment 3: DNS Name Resolution Engine</ins>

* **Description:** Develop a multi-threaded application, written in C, which resolves domain names to IP addresses. The application is comprised of two subsystems: a requestor pool and a resolver pool, which communicate using a shared array. Each input file is processed by a single requestor/resolver thread. If a thread tries to read to/write from the shared array when it is full, it should block until space opens, to avoid deadlock. Once a thread is finished executing, it should write to the file `serviced.txt` the number of files it serviced. Similarly, once a resolver thread completes execution, it should write to the file `results.txt` the number of files it serviced. Lastly, the program must return the runtime using a system call.

* **Build Tree:**

    - `multi-lookup.c`
        > Program takes a set of file names as input, each with a hostname for every line. Every file gets serviced by an individual thread from the requestor pool, which is then serviced by a set of resolver threads to map each hostname to its IP address.

    - `multi-lookup.h`
        > Function prototypes for multi-lookup.c

    - `queue.h:`
        > Header file for queue.c -- standard FIFO queue struct

    - `Makefile`
        > GNU makefile to build the program -- <make clean> to reset

    - `/input` 
        > Directory of input files

    - `performance.txt`
        > Performance diagnostic for varied combinations of the number of requester / resolver threads

* **Build Instructions:**

    1. Run Make to compile all files

    2.	Execute using the following format:
	
	      `./multi-lookup < # requesters >  < # resolvers >  < serviced.txt >  < results.txt >  < names1.txt > ...\`

    3. To check for memory leaks:
	
	     `valgrind ./multi-lookup  names1.txt names2.txt ... namesN.txt results.txt`



### Programming Assignment 4: Memory Management and Paging
