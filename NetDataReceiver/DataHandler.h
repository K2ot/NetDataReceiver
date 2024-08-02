#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>
#include <numeric>

enum class CodeTcp
{
	Message = 0x0A
	, Image = 0x0B
};

enum class FrameLayout
{
	Head = 0x00
	, Size = 0x04
	, Information = 0x05
	, checkSume = 0x02
};


namespace Handler
{
	class Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};

	public:

		Handler()  {}
		virtual ~Handler() = default;
		virtual void Handle(std::vector<unsigned char> frame) = 0;
		virtual void Reset() = 0;
		virtual bool Result() { return result; };
		virtual std::vector<unsigned char> GetResponse() { return response; };
	};

	class Message : public Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};

	public:

		Message() : Handler() {}
		void Handle(std::vector<unsigned char> frame) override;
		void Reset() override;
		bool Result() override;
		std::vector<unsigned char> GetResponse() override;
	};

	class Image : public Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};

	public:

		Image() : Handler() {}
		void Handle(std::vector<unsigned char> frame) override;
		void Reset() override;
		bool Result() override;
		std::vector<unsigned char> GetResponse() override;
	};

}


class DataManager
{
	std::mutex mutex;
	std::condition_variable conditionVariable;

	void AddHandler(std::vector<std::reference_wrapper<Handler::Handler>>& handlers, Handler::Handler& hand);

	// Kontener na referencje do handlerow
	// dodac nastepne kontenery jak bedzie kreslone wiecej opcji
	std::vector<std::reference_wrapper<Handler::Handler>> handlers;

	bool IsCheckSumValid(const std::vector<unsigned char>& frame);
	int GetSize(const std::vector<unsigned char>& frame);
	std::vector<unsigned char> GetData(std::vector<unsigned char>& frame);

	bool incomingData{ false };
	bool stopFlag{ false };
	std::thread popQueueThread;
	std::queue<unsigned char> dataQueue;

	const short _SAMPLE{ 20 };
	const short _DELAY{ 200 };
	DataManager();
	

public:

	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	~DataManager();
	static DataManager& GetInstance();

	//Dodaje obiekty handler
	void InitializeHandlers();


	//przekazanie danych do handlera (TCP)
	void PushDataToTCPHandler(const std::vector<unsigned char>& frame);

	void popFromQueue();

	

	Handler::Image image;
	Handler::Message message;
};
