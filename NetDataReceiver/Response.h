#pragma once
#include "TCPServer.h"

class Response
{
	std::thread replyThread;
	TCPServer& server;
	std::mutex mutex;

	void reply();
public:
	bool stopFlag;
	Response(TCPServer& server);
	~Response();

};

Response::Response(TCPServer& server)
	: server(server)
	, stopFlag(false)
{
	reply();
}

Response::~Response()
{
	stopFlag = true;
	if (replyThread.joinable())
	{
		replyThread.join();
	}
}

void Response::reply()
{
	replyThread = std::thread([this]
		{
			std::cout << "Responding has begun!" << std::endl;
			while (!stopFlag)
			{
				if (DataManager::GetInstance().image.Result())
				{
					std::cout << "Respond Foto:" << std::endl;
					auto response = DataManager::GetInstance().image.GetResponse();
					std::string extension(response.begin(), response.begin() + 1);
					extension += ".jpg";
					std::string photoName = { "foto" + extension };
				
					server.SendImage(photoName);
					DataManager::GetInstance().image.Reset();
				}

				if (DataManager::GetInstance().message.Result())
				{

					DataManager::GetInstance().message.Reset();
				}

				
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
		});
}

