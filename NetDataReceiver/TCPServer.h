#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <iomanip>
#include <fstream>

#include "DataHandler.h"


//using boost::asio::ip::tcp;

class TCPServer
{
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
	std::thread serviceThread;
	std::mutex mutex;
	std::string myIP;
	std::string myPort;

	std::vector<unsigned char> rawData;

public:
	TCPServer(const std::string& address, const std::string& port);
	~TCPServer();
	void StopServer();
	void StartAccept();
	void Read();
	void Run();
	void RecreateSocket();
	void Send(const std::string& message);
	void Send(const std::vector<unsigned char>& message);
	void SendImage(const std::string& filePath);

	bool stopFlag; 

};

