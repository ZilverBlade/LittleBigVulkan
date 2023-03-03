#include <lbvd/apps/tests/window_test.h>
#include <lbvd/apps/tests/shape_test.h>
#include <lbv/log.h>

#ifdef _WIN32	
#include <Windows.h>
#include <commdlg.h>
#include <shlobj_core.h>
#endif

using namespace LittleBigVulkan;
int main() {
	Apps::App* app = new Apps::ShapeTest("LittleBigVulkan v1.0");

	try {
		app->run();
	} catch(std::exception ex) {
		LBVLOG_ERR("%s", ex.what());
#ifdef _WIN32	
		MessageBoxA(nullptr, ex.what(), "FATAL ENGINE ERROR", MB_OK | MB_ICONERROR);
#endif
		return EXIT_FAILURE;
	}

	delete app;
	return EXIT_SUCCESS;
}