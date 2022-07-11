
# 随意写的makefile

server: main.cpp Acceptor.cpp Channel.cpp Epoll.cpp Handler.cpp InetAddress.cpp method_state.cpp Server.cpp Socket.cpp ThreadPool.cpp util.cpp
	g++ -pthread main.cpp Acceptor.cpp Channel.cpp Epoll.cpp Handler.cpp InetAddress.cpp method_state.cpp Server.cpp Socket.cpp ThreadPool.cpp util.cpp -o server

clean: 
	rm server