all:tcpepoll

tcpepoll:tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp
	g++ -o tcpepoll tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp

clean:
	rm -f tcpepoll

