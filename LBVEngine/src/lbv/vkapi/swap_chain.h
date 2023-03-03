#pragma once
#include <lbv/dll.h>

#include <lbv/window.h>
#include <lbv/vkapi/device.h>
#include <lbv/vkapi/render_pass.h>

namespace LittleBigVulkan {
	class LBVAPI LBVSwapChain {
	public:
        LBVSwapChain(LBVDevice& device, LBVWindow& window, VkExtent2D windowExtent, LBVSwapChain* oldSwapChain = nullptr);
        ~LBVSwapChain();

        LBVSwapChain(const LBVSwapChain&) = delete;
        LBVSwapChain& operator=(const LBVSwapChain&) = delete;
        LBVSwapChain() = default;

        VkFramebuffer getFramebuffer(int index) {
            return swapChainFramebuffers[index];
        }
        VkRenderPass getRenderPass() {
            return swapChainRenderPass;
        }
        VkImageView getImageView(int index) {
            return swapChainImageViews[index];
        }
        size_t getImageCount() {
            return swapChainImageViews.size();
        }
        VkFormat getSwapChainImageFormat() {
            return swapChainImageFormat;
        }
        VkExtent2D getSwapChainExtent() {
            return swapChainExtent;
        }
        uint32_t width() {
            return swapChainExtent.width;
        }
        uint32_t height() {
            return swapChainExtent.height;
        }
        uint32_t getImageIndex() {
            return currentFrame;
        }

        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkSubmitInfo getSubmitInfo(uint32_t* imageIndex);
        VkResult present(VkQueue queue, uint32_t* imageIndex);

        VkFence getFence(uint32_t imageIndex) {
            return inFlightFences[imageIndex];
        }

        bool compareSwapFormats(const LBVSwapChain& swapChain) const {
            return swapChain.swapChainImageFormat == swapChainImageFormat;
        }

        void beginRenderPass(VkCommandBuffer commandBuffer) {
            VkRenderPassBeginInfo renderPassBeginInfo{};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = swapChainRenderPass;
            renderPassBeginInfo.framebuffer = swapChainFramebuffers[currentFrame];
            renderPassBeginInfo.renderArea.extent = swapChainExtent;
            renderPassBeginInfo.clearValueCount = 1;
            VkClearValue clearColor{};
            clearColor.color = { 0.0f, 0.0f, 0.0f, 1.0 };
            renderPassBeginInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(swapChainExtent.width);
            viewport.height = static_cast<float>(swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor = {};
            scissor.extent = swapChainExtent;
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        }
        void endRenderPass(VkCommandBuffer commandBuffer) {
            vkCmdEndRenderPass(commandBuffer);
        }
    private:
        void init();
        void createSwapChain(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D extent, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode);
        void createImageViews();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();
        void setImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

        // Helper functions
        VkSurfaceFormatKHR choolbvSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>&availableFormats);


        VkPresentModeKHR choolbvSwapPrelbvntMode(
            const std::vector<VkPresentModeKHR>&availablePrelbvntModes);
        VkExtent2D choolbvSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);

        VkFormat swapChainImageFormat{};
        VkExtent2D swapChainExtent{};

        std::vector<VkFramebuffer> swapChainFramebuffers{};
        std::vector<VkImageView> swapChainImageViews{};
        VkRenderPass swapChainRenderPass{};

        LBVDevice& lbvDevice;
        LBVWindow& lbvWindow;
        VkExtent2D windowExtent{};

        VkSwapchainKHR swapChain{};
        LBVSwapChain* oldSwapChain = nullptr;

        std::vector<VkSemaphore> imageAvailableSemaphores{};
        std::vector<VkSemaphore> renderFinishedSemaphores{};
        std::vector<VkFence> inFlightFences{};
        std::vector<VkFence> imagesInFlight{};
        size_t currentFrame = 0;
        uint32_t imageCount = 3;
	};
}