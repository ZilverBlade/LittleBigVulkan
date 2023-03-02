#include "app.h"

namespace LittleBigVulkan::Apps {
	App::App(const char* name) : lbvDevice(), lbvWindow{ lbvDevice.getInstance(), 1280, 720, name } {
		
	}
	App::~App() {
	
	}
	void App::run() {
		
	}
}