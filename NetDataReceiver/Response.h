#pragma once
#include "TCPServer.h"

class Response
{
	std::thread replyThread_;
	TCPServer& server;
	std::mutex mutex;

	void reply();
public:
	bool stopFlag;
	Response(TCPServer& server);
	~Response();

};

Response::Response(TCPServer& server)
	: server_(server)
	, stopFlag(false)
{
	reply();
}

Response::~Response()
{
	stopFlag = true;
	if (replyThread_.joinable())
	{
		replyThread_.join();
	}
}

void Response::reply()
{
	replyThread_ = std::thread([this]
		{
			std::cout << "Responding has begun!" << std::endl;
			while (!stopFlag)
			{
				if (DataManager::GetInstance().image.GetResult())
				{
					std::cout << "Respond Foto:" << std::endl;
					auto response = DataManager::GetInstance().image.GetResponse();
					std::string extension(response.begin(), response.begin() + 1);
					extension += ".jpg";
					std::string photoName = { "foto" + extension };
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::Image) << std::endl;
					server_.SendImage(photoName);
					DataManager::GetInstance().image.Reset();
				}

				if (DataManager::GetInstance().rideSettingsHandler.GetResult())
				{
					std::lock_guard<std::mutex> lock(mutex_);
					std::cout << "Respond Ride Settings" << std::endl;
					auto vec = DataManager::GetInstance().rideSettingsHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::RideSettings) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::RideSettings) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().rideSettingsHandler.Reset();
				}

				if (DataManager::GetInstance().projectSettingsHandler.GetResult())
				{
					std::lock_guard<std::mutex> lock(mutex_);
					std::cout << "Respond Project Settings" << std::endl;

					auto vec = DataManager::GetInstance().projectSettingsHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::ProjectSettings) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::ProjectSettings) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().projectSettingsHandler.Reset();
				}

				if (DataManager::GetInstance().selfDiagnosticsHandler.GetResult())
				{
					std::lock_guard<std::mutex> lock(mutex_);
					std::cout << "Respond Self Diagnostics" << std::endl;
					auto vec = DataManager::GetInstance().selfDiagnosticsHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::SelfDiagnostics) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::SelfDiagnostics) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().selfDiagnosticsHandler.Reset();
				}

				//if (DataManager::GetInstance().FromControler.GetResult())
				//{
				//	std::lock_guard<std::mutex> lock(mutex_);
				//	std::cout << "Respond From Controler" << std::endl;
				//	auto vec = DataManager::GetInstance().fromTrolley.GetResponse();

				//	//Debug:
				//	std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::FromControler) << std::endl;
				//	std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::FromControler) << std::endl;
				//	std::cout << "Response size: " << vec.size() << std::endl;

				//	server_.Send(vec);
				//	DataManager::GetInstance().fromTrolley.Reset();
				//}

				if (DataManager::GetInstance().startCartHandler.GetResult())
				{
					std::lock_guard<std::mutex> lock(mutex_);
					std::cout << "Respond Start Cart" << std::endl;
					auto vec = DataManager::GetInstance().startCartHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::StartCart) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::StartCart) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().startCartHandler.Reset();
				}

				if (DataManager::GetInstance().stopCartHandler.GetResult())
				{
					std::lock_guard<std::mutex> lock(mutex_);
					std::cout << "Respond Stop Cart" << std::endl;
					auto vec = DataManager::GetInstance().stopCartHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::StopCart) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::StopCart) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().stopCartHandler.Reset();
				}

				if (DataManager::GetInstance().emergencyStopHandler.GetResult())
				{
					std::cout << "Respond E-Stop" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().emergencyStopHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::EmergencyStop) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::EmergencyStop) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().emergencyStopHandler.Reset();
				}

				if (DataManager::GetInstance().endRideHandler.GetResult())
				{
					std::cout << "Respond End Ride" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().endRideHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::EndRide) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::EndRide) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().endRideHandler.Reset();
				}

				if (DataManager::GetInstance().camera1ParametersHandler.GetResult())
				{
					std::cout << "Respond Camera 1" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().camera1ParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::Camera1Parameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::CameraParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().camera1ParametersHandler.Reset();
				}

				if (DataManager::GetInstance().camera2ParametersHandler.GetResult())
				{
					std::cout << "Respond Camera 2" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().camera2ParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::Camera2Parameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::CameraParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().camera2ParametersHandler.Reset();
				}

				if (DataManager::GetInstance().imuParametersHandler.GetResult())
				{
					std::cout << "Respond Imu" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().imuParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::IMUParameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::IMUParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().imuParametersHandler.Reset();
				}

				if (DataManager::GetInstance().inclinometerParametersHandler.GetResult())
				{
					std::cout << "Respond Inclinometr" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().inclinometerParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::InclinometerParameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::InclinometerParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().inclinometerParametersHandler.Reset();
				}

				if (DataManager::GetInstance().nucleoParametersHandler.GetResult())
				{
					std::cout << "Respond Nucleo" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().nucleoParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::NucleoParameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::NucleoParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().nucleoParametersHandler.Reset();
				}

				if (DataManager::GetInstance().profilometer1ParametersHandler.GetResult())
				{
					std::cout << "Respond Progilometr 1" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().profilometer1ParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::Profilometer1Parameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::ProfilometerParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().profilometer1ParametersHandler.Reset();
				}

				if (DataManager::GetInstance().profilometer2ParametersHandler.GetResult())
				{
					std::cout << "Respond Progilometr 2" << std::endl;
					std::lock_guard<std::mutex> lock(mutex_);
					auto vec = DataManager::GetInstance().profilometer2ParametersHandler.GetResponse();

					//Debug:
					std::cout << "Command Code Tcp: " << static_cast<uint8_t>(GlobalData::CommandCodeTcp::Profilometer2Parameters) << std::endl;
					std::cout << "Data size Tcp: " << static_cast<uint8_t>(GlobalData::DataSize::ProfilometerParameters) << std::endl;
					std::cout << "Response size: " << vec.size() << std::endl;

					server_.Send(vec);
					DataManager::GetInstance().profilometer2ParametersHandler.Reset();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
		});
}
