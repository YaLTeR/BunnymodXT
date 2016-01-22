#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include "../shared.hpp"
#include "../interprocess.hpp"

namespace Interprocess
{
	mqd_t mq = static_cast<mqd_t>(-1);

	void Initialize()
	{
		mq = mq_open(
			"/" MQ_NAME,
			O_WRONLY | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
			nullptr);

		if (mq == static_cast<mqd_t>(-1)) {
			EngineDevWarning("Error opening the message queue: %s\n", strerror(errno));
			EngineWarning("TASView integration is not available.\n");
			return;
		}

		EngineDevMsg("Opened the message queue.\n");
	}

	void Shutdown()
	{
		if (mq != static_cast<mqd_t>(-1)) {
			mq_close(mq);
			mq_unlink("/" MQ_NAME);
			EngineDevMsg("Closed the message queue.\n");
		}

		mq = static_cast<mqd_t>(-1);
	}

	void Write(const std::vector<char>& data)
	{
		if (mq == static_cast<mqd_t>(-1))
			return;

		if (mq_send(mq, data.data(), data.size(), 0) == -1) {
			// This shouldn't really happen.
			// Perhaps the message queue was force-deleted or something.
			EngineDevWarning("mq_send failed with %d (%s).\n", errno, strerror(errno));
			Shutdown();
			Initialize();
		}
	}
}
