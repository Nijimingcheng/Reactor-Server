all:tcpepoll

tcpepoll:tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp
	g++ -o tcpepoll tcpepoll.cpp INETAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp

clean:
	rm -f tcpepoll

