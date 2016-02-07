Name:Nishant Patil
SJSU ID:010720918

Assignment 3:
25 Nov 2015

Exercise 1:
Implement a “ping” program using raw socket.
1. ping support IPv4 and ICMPv4, not IPv6 and ICMPv6.
2. Multi threaded, create threads to send packets and also receive packets.
3. set ttl to 32.
4. should be self-sufficient and not depend on any source code or library that are not available from a standard Linux distribution.

Solution:
In this exercise ping program is written to send icmp echo request and receive icmp echo reply and display the ttl value and roundtrip time of each packet request-reply.

compile using:
$make
or
$gcc -pthread ping.c -o ping

execute using the below command:
$sudo ./ping www.google.com

sudo should be given to run the program as a root user.
