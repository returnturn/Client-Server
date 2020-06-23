main : server.cpp client.cpp
	g++ -o server -std=c++11 server.cpp
	g++ -o client -std=c++11 client.cpp

