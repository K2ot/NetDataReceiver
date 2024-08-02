#pragma once
#include "TCPServer.h"


TCPServer::TCPServer(const std::string& address, const std::string& port)
	: ioService()
	, acceptor(ioService)
	, socket(ioService)
	, myIP(address)
	, myPort(port)
	, stopFlag(false)
{
	rawData.reserve(1024);

	boost::system::error_code ec;
	boost::asio::ip::tcp::resolver resolver(ioService);
	boost::asio::ip::tcp::resolver::query query(myIP, myPort);

	auto endpoint_iterator = resolver.resolve(query, ec);

	if (ec || endpoint_iterator == boost::asio::ip::tcp::resolver::iterator())
	{
		std::cerr << "B³¹d podczas rozwi¹zywania endpoint: " << ec.message() << std::endl;
		throw std::runtime_error("Nie mo¿na rozwi¹zaæ endpoint.");
	}

	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query).begin();
	acceptor.open(endpoint.protocol(), ec);
	if (ec)
	{
		std::cerr << "B³¹d podczas otwierania acceptor: " << ec.message() << std::endl;
		throw std::runtime_error("Nie mo¿na otworzyæ acceptor.");
	}

	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	if (ec)
	{
		std::cerr << "B³¹d podczas ustawiania opcji reuse_address: " << ec.message() << std::endl;
	}

	acceptor.bind(endpoint, ec);
	if (ec)
	{
		std::cerr << "B³¹d podczas wi¹zania z endpoint: " << ec.message() << std::endl;
	}

	acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
	if (ec)
	{
		std::cerr << "B³¹d podczas nas³uchiwania: " << ec.message() << std::endl;
	}
}


TCPServer::~TCPServer()
{
	StopServer();
}

void TCPServer::StopServer()
{
	stopFlag = true;
	ioService.stop();

	socket.cancel();
	if (socket.is_open())
	{
		socket.close();
	}

	acceptor.cancel();
	if (acceptor.is_open())
	{
		acceptor.close();
	}

	if (serviceThread.joinable())
	{
		serviceThread.join();
	}
}

void TCPServer::Run()
{
	try
	{
		StartAccept();
		serviceThread = std::thread([this]
			{

			//	GlobalMethods::stringOut("Local Endpoint TCP address: ", static_cast<std::string>(acceptor.local_endpoint().address().to_string().c_str()));
			//	GlobalMethods::stringOut("Local Endpoint TCP port: ", static_cast<std::string>(std::to_string(acceptor_.local_endpoint().port()).c_str()));
				//GlobalMethods::stringOut("TCP Acceptor is open: ", acceptor_.is_open());
			//	GlobalMethods::stringOut("io_service TCP is run");

				ioService.run();

				//GlobalMethods::stringOut("io_service TCP is stop");

			});
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in Run(): " << e.what() << std::endl;
	}
}

void TCPServer::RecreateSocket()
{
	if (!socket.is_open())
	{
		socket.close();
		socket = boost::asio::ip::tcp::socket(ioService); // Re-inicjalizacja socketu
		std::cout << "New socet!" << std::endl;
	}
}

void TCPServer::StartAccept()
{
	RecreateSocket();

	acceptor.async_accept(socket, [this](const boost::system::error_code& error)
		{
			if (!error)
			{
				std::cout << "New connection:\nIP: " << socket.remote_endpoint().address().to_string()
					<< "\nPort: " << socket.remote_endpoint().port() << std::endl;
				Read();
			}

			if (!stopFlag)
			{
				StartAccept(); // Kontynuuj nas³uchiwanie na nowe po³¹czenia
			}
		});
}

void TCPServer::Read()
{
	rawData.resize(1024);
	auto readData = [this](const boost::system::error_code& err, std::size_t bytes_transferred)
		{
			if (!err)
			{
				try
				{
					std::lock_guard<std::mutex> lock(mutex);
					rawData.resize(bytes_transferred);

					std::stringstream ss;
					ss << "\nMessage read successfully: " << bytes_transferred << "\n";
					for (auto data : rawData)
					{
						ss << " 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data);
					}
					ss << "\n";
					std::cout << ss.str() << std::endl;
					ss.str(std::string());
					ss.clear();
					for (auto f : rawData)
					{
						if (f >= 21 && f <= 126)
						{
							ss << static_cast<char>(f);
						}
					}
					std::cout << ss.str() << std::endl;

					//DataManager::GetInstance().PushReceivedDataToTCPHandler(rawData_);
				}
				catch (const std::out_of_range& e)
				{
					std::cerr << __FUNCTION__ << " B³¹d: " << e.what() << std::endl;
				}
				catch (const std::exception& e)
				{
					std::cout << "Blad " << e.what() << std::endl;
				}
				Read();
			}
			else
			{
				std::cerr << "ReceiveData: Error code: " << err.value() << " - " << err.message() << "\n**************************************\n" << std::endl;
				socket.close();
			}
		};

	socket.async_read_some(boost::asio::buffer(rawData), readData);
}

void TCPServer::Send(const std::string& message)
{
	auto messageV = std::vector<unsigned char>(message.begin(), message.end());

	boost::asio::async_write(socket, boost::asio::buffer(messageV),
		[this, messageV](const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			if (!error)
			{
				std::stringstream ss;
				ss << "Message sent successfully: " << bytes_transferred << "\n";
				for (auto data : messageV)
				{
					ss << " 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data);
				}
				ss << "\n";
				std::cout << ss.str() << std::endl;
			}
			else
			{
				std::cerr << "Send error: " << error.message() << std::endl;
			}
		});
}

void TCPServer::Send(const std::vector<unsigned char>& message)
{
	boost::asio::async_write(socket, boost::asio::buffer(message),
		[this, message](const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			if (!error)
			{
				std::stringstream ss;
				ss << "Message sent successfully: " << bytes_transferred << "\n";
				for (auto data : message)
				{
					ss << " 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data);
				}
				ss << "\n";
				std::cout << ss.str() << std::endl;
			}
			else
			{
				std::cerr << "Send error: " << error.message() << std::endl;
			}
		});
}

void TCPServer::SendImage(const std::string& filePath)
{
	if (!socket.is_open())
	{
		std::cerr << "nie mo¿na wys³aæ pliku, zamkniête gniazdo! " << std::endl;
		return;
	}
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Nie mo¿na otworzyæ pliku: " << filePath << std::endl;
		return;
	}

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer;
	buffer.reserve(fileSize);
	buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	std::vector<unsigned char> frame{ 0xBD, 0x00 ,0x00 };
	//TcpDataFrameMod::AddSize(frame, buffer.size());

	std::stringstream ss;
	ss << "Wielkoœæ foto: " << buffer.size() << "\n";
	buffer.insert(buffer.begin(), frame.begin(), frame.end());
	ss << "Wielkoœæ ramki: " << buffer.size() << "\n";
	//TcpDataFrameMod::AddCheckSum(buffer);
	ss << "Wielkoœæ ramki z sum¹: " << buffer.size() << "\n";
	std::cout << ss.str() << std::endl;
	try
	{
		std::size_t bytes_transferred = boost::asio::write(socket, boost::asio::buffer(buffer));
		std::cout << "Plik zosta³ wys³any pomyœlnie. Liczba wys³anych bajtów: " << bytes_transferred << std::endl;
		buffer.reserve(0);
	}
	catch (const boost::system::system_error& e)
	{
		std::cerr << "B³¹d podczas wysy³ania pliku: " << e.what() << std::endl;
		buffer.reserve(0);
	}

}
