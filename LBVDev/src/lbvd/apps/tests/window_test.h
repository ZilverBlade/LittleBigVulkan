#pragma once

#include <lbvd/apps/app.h>

namespace LittleBigVulkan::Apps {
	class WindowTest : public App {
	public:
		WindowTest(const char* name);
		virtual ~WindowTest();

		virtual void run();
	protected:

	};
}