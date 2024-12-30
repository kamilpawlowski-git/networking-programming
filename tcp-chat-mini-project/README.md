# TCP chat in C
Tcp server-client chat on Linux OS.

Thanks to that project you can get better understanding of:
* tcp protocol
* how to use threads
* signal library
* linux kernel (from networking perspective)

To test it out Docker (or any virtual machines) are recommended.

**How to use it?**
First machine:
* compile code: gcc -o server chat_server.c
* turn server on: ./server

Second machine:
* compile code: gcc -o client chat_client.c
* turn client on and input proper server ip address: ./client *your server ipv6 address*

Make sure you have machines in the same ipv6 network!
