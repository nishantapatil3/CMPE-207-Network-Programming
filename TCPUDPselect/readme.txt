Name:Nishant Patil
SJSU ID:010720918

Assignment 2:
18 Oct 2015

Exercise 2:
Re-implement the TCP and UDP servers of Homework #1 in a single program. The program should also satisfy the following requirements:
-> Use select() in the server to support both TCP and UDP clients.
-> The server should be multithreaded as well: It should create a new thread to service each new client connection (TCP) or request (UDP).
-> The server should keep track of the total number of client connections and the total number of DVDs being purchased successfully since the server starts and log these stat counters to the console or a log file after each client connection. Make sure that you use proper locking to protect the access to shared resources.

Solution 2:
1) Create a file descriptor and set its values to zero usin fd_zero();.
2) Create a pthread_t and set its attributes to required execution type.
3) Create a TCP socket and a UDP socket to listen to incoming tcp or udp requests.
4) In a looping statement set fd values to zero so that in each itteration the fd is reset.
5) Read current socket status by using fd_set();.
6) start a select() function and keep checking for any input commands.
7) If a socket connection is initiated then its value is set to &readfds then using if statements tcp or udp connection is differentiated.
8) If a TCP connection is made then TCPdvd() function is called, listen and accept connection is used to connect and serve the client request, then disconnected, it works on the same logic as explained at the end of this readme file**.
9) If a UDP connection is made then UDPdvd() function is called, sendto() and recvfrom() are used to serve the client request, then disconnected, it works on the same logic as explained at the end of this readme file**.
10) A structure is defined to count current tcp and current udp connections, the variables are locked in every connection so that do not experience race conditions.
11) After start-up, the server runs continuously to serve client requests, the server can handle multiple clients and multiple connections are handled by multithreads.
12) Interval and timeout are set to one second which can be changed as required.

References:
Internetworking with TCPIP Volume III ClientServer Programming
Source code from class ppt's

Execte and run:
to compile
 $ make

to clean
 $ make clean

run server:
1) ./tcpudpselect <ip> <port>
ex: ./tcpudpselect 127.0.0.1 8888

run client:
2.a) ./tcpclient <ip> <port>
     ./tcpclient <ip> <port> <order> <itemnumber> <quantity>
ex:  ./tcpclient 127.0.0.1 8888 list					//to display dvd list
ex:  ./tcpclient 127.0.0.1 8888 order 1003 3				//to order dvd

usage: tcp_client [ip] [port]

2.b) ./udpclient <ip> <port> <list>
     ./udpclient <ip> <port> <order> <itemnumber> <quantity>
ex:  ./udpclient 127.0.0.1 8888 list					//to display dvd list
ex:  ./udpclient 127.0.0.1 8888 order 1001 4				//to order dvd

**
working of dvd store
1)Client sends 'list' or 'order' string commands.
2)Using string comparision on server to compare and execute the commands sent by client.
3)A database.txt file is used to read from and write to the database.
4)In 'list' logic, the database is copied into a string and sent to client from server, that displays the database of dvdstore.
5)In 'order' logic, the server recieves itemnumber to be ordered from client, itteratively checks on database in a loop statement, if found, raise a flag 'found' and break loop, then server recieves number of items to be ordered and update the dvd itemnumber with ordered items deducted from database.
6)A 'temp.txt' file is used to temporarily store database and update the database.
7)After start-up, the server runs continuously to serve client requests. The server processes one client request at a time. It also logs (to the server console or a log file) each client request, including the client IP and port, the server IP and port for the client connection, the request type (LIST or ORDER), and the request details (e.g. item number and quantity of a DVD ORDER request).
8)The client reads a request from its command line arguments, connects to the specified server, sends the request, reads the reply and then disconnects and exits.
