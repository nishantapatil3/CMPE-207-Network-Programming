Name:Nishant Patil

Assignment 2:
18 Oct 2015

Exercise 1:
Modify TCPdaytimed.c to make it a concurrent, connection-oriented server using a multi-process master-slave design.

Solution 1:
In this exercise we have 'TCPdaytimed.c' server program that uses 'passivesock.c' 'passivetcp.c' 'errexit.c' and compiles to get TCPdaytimed executable file.
and 'TCPdaytime.c' client program that uses 'connectsock.c' 'connectTCP.c' and compiles to get TCPdaytime executable file.

1) Server is initiated by setting up passivesock and passivetcp, then in a looping function it accepts concurrent client requests.
2) The program is modified to make it multi-process master-slave concurrent, connection-oriented server.
3) The server runs to provide service by sending current day and time values to clients.
4) For each client connection the parent process creates child process using fork().
5) Server sends day and time values to client and then child terminates.
6) After start-up, the server runs continuously to serve client requests, Parent process is concurrent and listens for next client connection.

References:
Internetworking with TCPIP Volume III ClientServer Programming
Source code from class ppt's

Execte and run:
to compile
 $ make

to clean
 $ make clean

run server:
usage: TCPdaytimed [port]

./TCPdaytimed <port>
ex: ./TCPdaytimed 8888

run client:
usage: TCPdaytime [host] [port]

./TCPdaytime <host> <port>
ex: ./TCPdaytime 127.0.0.1 8888
