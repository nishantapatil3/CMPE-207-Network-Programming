# CMPE-207-Network-Programming

Solution to:
1. Implement a pair of IPv6 TCP client and server for a simple DVD store ordering system (as described in Homework #1) that support both IPv4 and IPv6 peers.
 Implement an IPv6 server for this DVD store system. This server should accept connections from both IPv6 clients and pure IPv4 clients (e.g. client4 from hw3-ipv4).
 Implement an IP6 client for this DVD store system that can connect to an IPv6 server at the server’s IPv6 address as well as a pure IPv4 server (e.g. server4 fromhw3-ipv4)attheserver’sIPv4address. Youshouldusegetaddrinfoinyour implementation to request only IPv6 addresses for the address lookup.
NOTES:
 All IP addresses used and logged by the IPv6 client and the IPv6 server should be IPv6 addresses. IPv4 addresses can only be used in the command line arguments. In the IPv6 client/server programs, IPv4 addresses should be used and logged in IPv4-mapped IPv6 addresses.
 The node (aka hostname) argument of getaddrinfo can be either an actual host DNS name or just a host IP string. Similarly, the service argument of getaddrinfo can be either a server name or just a port number string.
 To compile the code that uses getaddrinfo, you need to add –D_POSIX_SOURCE gcc compile option (CCFLAGS).
 You may use link-local IPv6 address on Linux to test your TCP client and server. If you do so, you need to set the socket option, SO_BINDTODEVICE, to bind the client socket to the network device (e.g. “eth0”) corresponding to the link-local IPv6 address. You should only use SO_BINDTODEVICE for the socket on the client side and only when the server IP to be connected is a link-local IPv6 address. Also, be aware that this socket option (SO_BINDTODEVICE) can only be set by a root- privileged process.

2. Modify TCPdaytimed.c (Chapter 10.5) to make it a concurrent, connection- oriented server using a multi-process master-slave design. More specifically, the master process of the server should create a new child process (slave) to handle each incoming request. The slave process should convert the time string into the following format:
MM/DD/YYYY HH:MM:SS Weekday
for example, “09/24/2015 10:05:02 Thu”. The slave process should then send the revised time string back to the client.

3. Implement a “ping” program using raw socket.
 This “ping” program only needs to support IPv4 and ICMPv4, not IPv6 and ICMPv6.
You may use the “ping” program from the “Unix Network Programming” textbook as the starting point for your implementation. If you do so, please remove the IPv6 and ICMPv6 related code from the textbook “ping” program.
 This “ping” program should be multi-threaded. Instead of sending the “ping” probes (i.e. ICMP echo requests) from an alarm signal handler, this program should use one thread to send ICMP echo requests and another thread to receive ICMP echo replies.
 The IP TTL should be set to 32 for every ICMP echo request sent by this program.
 Your submission should be self-sufficient and not depend on any source code or
library that are not available from a standard Linux distribution.
