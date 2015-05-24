#include "stdafx.hpp"

#include "sptlib-wrapper.hpp"
#include "interprocess.hpp"

namespace Interprocess
{
	std::unique_ptr<boost::interprocess::message_queue> mq;

	void Initialize()
	{
		if (boost::interprocess::message_queue::remove("BunnymodXT-TASView")) {
			EngineDevMsg("Cleaned up the old message queue.\n");
		}
		try {
			mq = std::make_unique<boost::interprocess::message_queue>(
				boost::interprocess::create_only,
				"BunnymodXT-TASView",
				1000,
				256);

			EngineDevMsg("Created the message queue successfully.\n");
		} catch (boost::interprocess::interprocess_exception &ex) {
			EngineWarning("Failed to create the message queue, TASView integration is not available: %s\n", ex.what());
			mq.reset();
		}
	}

	void Shutdown()
	{
		mq.reset();
		if (boost::interprocess::message_queue::remove("BunnymodXT-TASView")) {
			EngineDevMsg("Closed the message queue.\n");
		}
	}
}
