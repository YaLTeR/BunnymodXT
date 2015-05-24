namespace Interprocess
{
	extern std::unique_ptr<boost::interprocess::message_queue> mq;

	void Initialize();
	void Shutdown();
}
