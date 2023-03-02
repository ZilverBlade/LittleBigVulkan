#pragma once
#include <lbv/dll.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace LittleBigVulkan {

class LBVAPI LBVWindow {
 public:
  LBVWindow(VkInstance instance, int w, int h, const char* name);
  ~LBVWindow();

  LBVWindow(const LBVWindow &) = delete;
  LBVWindow &operator=(const LBVWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }
  GLFWwindow *getGLFWwindow() const { return window; }
  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  bool isKeyDown(int keyCode) { return glfwGetKey(window, keyCode) == GLFW_PRESS; }
  bool isMoulbvButtonDown(int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; }

  VkSurfaceKHR getSurface() {
	  return surface;
  }
 private:
  static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
  void initWindow();

  int width;
  int height;
  bool framebufferResized = false;

  const char* windowName;
  GLFWwindow *window;
  VkSurfaceKHR surface{};
  VkInstance instance;
};
}  // namespace LittleBigVulkan
