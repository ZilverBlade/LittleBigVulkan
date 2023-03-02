#include "window_test.h"

namespace LittleBigVulkan::Apps {
	WindowTest::WindowTest(const char* name) : App(name) {
	
	}
	WindowTest::~WindowTest() {}
	void WindowTest::run() {
		LBVLOG_INF("%s", "Hello World!");
		LBVLOG_WRN("%s", "Warning!");
		LBVLOG_ERR("%s", "Error!");
		while (!lbvWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}