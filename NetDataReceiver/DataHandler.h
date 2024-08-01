#pragma once

enum class CodeTcp
{
	Message = 0x0A
	, Image = 0x0B
};


namespace Handler
{
	class Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};
	public:
		Handler(uint8_t res = 0) : resCode(res) {}
		virtual ~Handler() = default;
		virtual void Handle(std::vector<unsigned char> frame) = 0;
		virtual void Reset() = 0;
		virtual bool GetResult() { return result; };
		virtual std::vector<unsigned char> GetResponse() { return response; };
		uint8_t resCode;
	};

	class Message : public Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};
	public:

		Message() : Handler(static_cast<uint8_t>(CodeTcp::Message)) {}
		void Handle(std::vector<unsigned char> frame) override;
		void Reset() override;
		bool GetResult() override;
		std::vector<unsigned char> GetResponse() override;
	};

	class Image : public Handler
	{
		bool result{ false };
		std::vector<unsigned char> response{};
	public:

		Image() : Handler(static_cast<uint8_t>(CodeTcp::Image)) {}
		void Handle(std::vector<unsigned char> frame) override;
		void Reset() override;
		bool GetResult() override;
		std::vector<unsigned char> GetResponse() override;
	};

}


class DataManager
{
	std::mutex _mutex;

	void AddHandler(std::vector<std::reference_wrapper<Handler::Handler>>& handlers, Handler::Handler& hand);

	// Kontener na referencje do handlerow
	// dodac nastepne kontenery jak bedzie kreslone wiecej opcji
	std::vector<std::reference_wrapper<Handler::Handler>> _handlersTCP; 


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
	void PushReceivedDataToTCPHandler(const std::vector<unsigned char>& frame);
