#include "window.h"

// std
#include <stdexcept>

namespace LittleBigVulkan {

    LBVWindow::LBVWindow(VkInstance instance, int w, int h, const char* name) : instance(instance), width{w}, height{h}, windowName{name} {
        initWindow();
        createWindowSurface(instance, &surface);
    }
    
    LBVWindow::~LBVWindow() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    
    void LBVWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }
    
    void LBVWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }
    
    void LBVWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
      auto wnd = reinterpret_cast<LBVWindow *>(glfwGetWindowUserPointer(window));
      wnd->framebufferResized = true;
      wnd->width = width;
      wnd->height = height;
    }
}
