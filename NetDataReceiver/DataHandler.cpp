#include "DataHandler.h"


DataManager::DataManager()
{
}

DataManager& DataManager::GetInstance()
{
	static DataManager instance;
	return instance;
}

DataManager::~DataManager()
{

}

void DataManager::AddHandler(std::vector<std::reference_wrapper<Handler::Handler>>& handlers, Handler::Handler& hand)
{
	handlers.push_back(hand);
}

void DataManager::InitializeHandlers()
{
	AddHandler(handlers, image);
	AddHandler(handlers, message);
}

void DataManager::PushDataToTCPHandler(const std::vector<unsigned char>& frame)
{
	uint8_t codeHead{ 0xFF };
	codeHead = frame.at(static_cast<uint8_t>(FrameLayout::Head));
		

	if (!IsCheckSumValid(frame))
	{

		throw std::invalid_argument("Niewlasciwa sumakontrolna" );
	}

	// w zaleznosci jaki jest kod rozkazu do takiego handlara wyslij same dane
	// do handlerow maja byc juz gole dane bez otoczek ramki

	std::vector<unsigned char> data{ GetData( frame) };

	try
	{
		if (handlers.empty())
		{
			throw std::runtime_error("_handlersTCP is empty");
		}
		switch (codeHead)
		{
		case static_cast<uint8_t>(CodeTcp::Image):
		
			image.Handle(data);
			break;
		
		case static_cast<uint8_t>(CodeTcp::Message):

			for (auto& handler : handlers)
			{
				handler.get().Handle(data);
				break;
			}
			break;

		default:
			break;
		}
	}
	catch (const std::length_error& e)
	{
		std::cout << "Pozor" << e.what();
		throw;
	}
	catch (const std::runtime_error& e)
	{
		std::cout<<"Pozor"<< e.what();
		throw;
	}
}

void DataManager::popFromQueue()
{
	const size_t CHECK_SUM_SIZE{ 2 };
	std::vector<unsigned char> frame{};
	int expectedDataSize{ 0 };
	while (!stopFlag)
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			conditionVariable.wait(lock, [this]() { return incomingData; });

			if (dataQueue.empty())
			{
				continue;
			}
			if (expectedDataSize == 0)
			{
				for (size_t i = 0; i < 7; ++i)
				{
					frame.push_back(dataQueue.front());
					dataQueue.pop();
				}
			}
			expectedDataSize = GetSize(frame) + CHECK_SUM_SIZE;
			auto receivedDataSize{ dataQueue.size() };
			if (receivedDataSize < expectedDataSize)
			{
				continue;
			}
		}

		{
			std::unique_lock<std::mutex> lock(mutex);
			for (size_t i = 0; i < expectedDataSize; ++i)
			{
				frame.push_back(dataQueue.front());
				dataQueue.pop();
			}

			PushDataToTCPHandler(frame);

			expectedDataSize = 0;
			frame.clear();
			if (dataQueue.empty())
			{
				incomingData = false;
			}
		}
	}
}

bool DataManager::IsCheckSumValid(const std::vector<unsigned char>& frame)
{
	if (frame.empty())
	{
		throw std::length_error("Brak danych");
	}

	uint16_t counter = std::accumulate(frame.begin(), frame.end() - 2, 0);
	auto BytesAsInt = [](const std::vector<unsigned char>& input)
		{
			uint16_t output{ 0 };
			std::vector<unsigned char> data = input;

			output = (data[1] << 8) | 0;
			output |= data[0];

			return output;
		};

	uint16_t number = BytesAsInt(std::vector<unsigned char>(frame.end() - 2, frame.end()));
	return counter == number;
}

int DataManager::GetSize(const std::vector<unsigned char>& frame)
{
	//GetLittleEndian
	if (frame.size() != 4)
	{
		throw std::invalid_argument("Input vector must have exactly 4 elements");
	}
	int sizeLayout{ static_cast<uint8_t>(FrameLayout::Size) };

	uint32_t result = 0;
	result |= static_cast<uint32_t>(frame.at(1)) << 0;
	result |= static_cast<uint32_t>(frame.at(2)) << 8;
	result |= static_cast<uint32_t>(frame.at(3)) << 16;
	result |= static_cast<uint32_t>(frame.at(4)) << 24;

	return result;
}

std::vector<unsigned char> DataManager::GetData(const std::vector<unsigned char>& frame)
{
	std::vector<unsigned char> vector { frame.begin() + static_cast<uint8_t>(FrameLayout::Information)
		,frame.end() - static_cast<uint8_t>(FrameLayout::checkSume) };
	return vector;
}



///////////////////////////////////////////////////////////////////////////////////////////


void Handler::Message::Handle(std::vector<unsigned char> frame)
{
}

void Handler::Message::Reset()
{
}

bool Handler::Message::Result()
{
	return false;
}

std::vector<unsigned char> Handler::Message::GetResponse()
{
	return std::vector<unsigned char>();
}

void Handler::Image::Handle(std::vector<unsigned char> frame)
{
}

void Handler::Image::Reset()
{
}

bool Handler::Image::Result()
{
	return false;
}

std::vector<unsigned char> Handler::Image::GetResponse()
{
	return std::vector<unsigned char>();
}




