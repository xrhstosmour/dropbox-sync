# dropbox-sync

This implementation is able to create multiple clients which connect to one server and integrate with him. 
1. When a client connects to the server, sends the LOG_ON <IP address, portNum> message to him, so as to inform for its existence.
2. The the server sends to all the connected clients the needed details of the new client. Both the server and the already connected clients keep the details of the new client to a list.
3. A client can request a list of all the connected clients to the server via the GET_CLIENTS command. When the server receives this commands, it forwards the list to all the connected clients. 
4. When a client disconnects it sends the LOG_OFF <IP, port> message to the server. The server updates its clients list and then inform the connected clients in order to update their list too.

The service is based on three C libraries:
* [sys_socket.h](http://man7.org/linux/man-pages/man0/sys_socket.h.0p.html)  sys/socket.h — main sockets header.
* [netdb.h](http://man7.org/linux/man-pages/man0/netdb.h.0p.html) netdb.h — definitions for network database operations.
* [pthread.h](http://man7.org/linux/man-pages/man0/pthread.h.0p.html) pthread.h — threads.

## Prerequisites
In order to be able to run this implementation you will need at least one linux machine or multiple with access to the same network. 

## Configuration:
1. Change [clientPort, workersNumber, bufferSize, serverPort, serverIP](https://github.com/xrhstosmour/DropBoxSync/blob/master/dropbox_client/Makefile#L27) with the details for your clients.
2. Change [serverPort](https://github.com/xrhstosmour/DropBoxSync/blob/master/dropbox_server/Makefile#L27) with the details for your server.

## Execution:
1. After finishing your [configurations](#configuration)
2. Navigate to the [server's folder](https://github.com/xrhstosmour/DropBoxSync/blob/master/dropbox_server/) and run the following command: ``` make clean valgrind ```
3. Then navigate to the [client's folder](https://github.com/xrhstosmour/DropBoxSync/blob/master/dropbox_clientr/) and run the following command: ``` make clean valgrind ```. If you wish more than one clients you should reconfigure the client's Makefile before each run.
