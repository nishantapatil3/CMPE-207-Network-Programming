all:
	gcc -c tcp_server.c
	gcc -c udp_server.c
	gcc -c tcpudpselect.c
	gcc -o tcpudpselect tcpudpselect.o tcp_server.o udp_server.o -lpthread
	
	gcc -o tcpclient tcp_client.c
	gcc -o udpclient udp_client.c

clean:
	rm -r tcpudpselect
	rm -r tcpclient
	rm -r udpclient
	rm -r tcpudpselect.o
	rm -r tcp_server.o
	rm -r udp_server.o
