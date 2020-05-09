# DropBoxSync

This implementation is able to create multiple clients which connect to one server and integrate with him.

The service is based on three C libraries:
* [sys_socket.h](http://man7.org/linux/man-pages/man0/sys_socket.h.0p.html)  sys/socket.h — main sockets header.
* [netdb.h](http://man7.org/linux/man-pages/man0/netdb.h.0p.html) netdb.h — definitions for network database operations.
* [pthread.h](http://man7.org/linux/man-pages/man0/pthread.h.0p.html) pthread.h — threads.

## Prerequisites
In order to be able to run this implementation you will need at least one linux machine or multiple with access to the same network.


