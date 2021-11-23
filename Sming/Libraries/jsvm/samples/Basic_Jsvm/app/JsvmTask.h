
#pragma once

#include <Task.h>
#include <jsvm.h>

/**
 * @brief - Task that runs the `loop` JavaScript function in the background
 */
class JsvmTask : public Task
{
public:
	JsvmTask(Jsvm& jsvm) : jsvm(jsvm)
	{
	}

	void loop() override
	{
		if(!jsvm.runFunction("loop")) {
			error = F("Failed running loop function. Suspending task");
			suspend();
		}
	}

	bool hasError()
	{
		return error.length() > 0;
	}

	String getError()
	{
		return error;
	}

private:
	Jsvm& jsvm;
	String error;
};
