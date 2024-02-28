#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

 namespace ze {
    class ZeWindow {
    public:
        ZeWindow(int w, int h, std::string name);
        ~ZeWindow();

        ZeWindow(const ZeWindow &) = delete;
        ZeWindow &operator=(const ZeWindow&) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; }
        GLFWwindow *getGLFWwindow() const { return window; };

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();

        int width;
        int height;
        bool framebufferResized = false;

        std::string windowName;
        GLFWwindow *window;
    };
}