#pragma once

#include <lbv/log.h>

#include <lbv/window.h>
#include <lbv/vkapi/device.h>

namespace LittleBigVulkan::Apps {
	class App {
	public:
		App(const char* name);
		virtual ~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = delete;
		App& operator=(App&&) = delete;

		virtual void run();
	protected:
		LBVDevice lbvDevice;
		LBVWindow lbvWindow;
	};
}