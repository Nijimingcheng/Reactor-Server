all:tcpepoll

tcpepoll:tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp
	g++ -o tcpepoll tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp

clean:
	rm -f tcpepoll

