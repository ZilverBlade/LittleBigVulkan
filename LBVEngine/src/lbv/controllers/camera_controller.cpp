#include "camera_controller.h"
#include <limits>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace LittleBigVulkan::Controllers {
	void CameraController::moveTranslation(LBVWindow& window, float dt, Actor actor) {
		//actor = actor;

		float yaw = actor.getComponent<Components::TransformComponent>().rotation.z;
		float pitch = actor.getComponent<Components::TransformComponent>().rotation.x;
		const glm::vec3 forwardDir{ sin(yaw), cos(yaw), -sin(pitch) };
		const glm::vec3 rightDir{ forwardDir.y, -forwardDir.x, 0.f, };
		const glm::vec3 upDir{ 0.f, 0.f, 1.f };

		glm::vec3 moveDir{ 0.f };

		if (window.isKeyDown(keys.moveForward)) moveDir += forwardDir;
		if (window.isKeyDown(keys.moveBackward)) moveDir -= forwardDir;
		if (window.isKeyDown(keys.moveRight)) moveDir += rightDir;
		if (window.isKeyDown(keys.moveLeft)) moveDir -= rightDir;
		if (window.isKeyDown(keys.moveUp)) moveDir += upDir;
		if (window.isKeyDown(keys.moveDown)) moveDir -= upDir;

		speedModifier = 0.25f + 0.75f * (!window.isKeyDown(keys.slowDown)) + 1.75f * window.isKeyDown(keys.speedUp);

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			actor.getComponent<Components::TransformComponent>().translation += moveSpeed * dt * glm::normalize(moveDir) * speedModifier;
		}
	}
	void CameraController::moveOrientation(LBVWindow& window, float dt, Actor actor) {
		//actor = _actor;
		GLFWwindow* wnd = window.getGLFWwindow();
		if (window.isMoulbvButtonDown(buttons.canRotate)) {
			glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			int width;
			int height;
			glfwGetWindowSize(wnd, &width, &height);

			// Prevents camera from jumping on the first click
			if (firstClick) {
				glfwSetCursorPos(wnd, (width / 2.f), (height / 2.f));
				firstClick = false;
			}

			double moulbvX;
			double moulbvY;
			glfwGetCursorPos(wnd, &moulbvX, &moulbvY);

			float rotX = sensitivity * (float)(moulbvY - (height / 2.f)) / height;
			float rotY = sensitivity * (float)(moulbvX - (width / 2.f)) / width;


			// force the roll to be pi*2 radians
			orientation.z = 6.283185482f;

			// up down rotation
			orientation = glm::rotate(orientation, glm::radians(rotX), upVec);
			// to make sure it doesnt over-rotate			
			orientation.x = glm::clamp(orientation.x, -1.5707963f, 1.5707963f);

			// left right rotation
			orientation = glm::rotate(orientation, glm::radians(rotY), glm::normalize(glm::cross(orientation, upVec)));

			glfwSetCursorPos(wnd, (width / 2.f), (height / 2.f));
		}
		else { glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL); firstClick = true; }

		if (glm::dot(orientation, orientation) > std::numeric_limits<float>::epsilon()) {
			actor.getComponent<Components::TransformComponent>().rotation = { orientation.x, 0.f, orientation.y };
		}
	}
	//bool EditorMovementController::moulbvScrollEvent(Events::MoulbvScrollEvent& e) {
	//	if (!e.getWindowPointer()->isKeyDown(GLFW_KEY_LEFT_ALT)) {
	//		float fov = actor.getComponent<Components::CameraComponent>().fov;
	//		fov -= e.getYOffset() * 3.5f;
	//		actor.getComponent<Components::CameraComponent>().fov = glm::clamp(fov, 10.f, 170.f);
	//	}
	//	else {
	//		moveSpeed = glm::clamp(moveSpeed + e.getYOffset() * 0.667f, 0.5f, 100.f);
	//		//moveSpeed = glm::clamp(moveSpeed + e.getYOffset() * 0.667f + e.getYOffset() * glm::pow(moveSpeed * moveSpeed / 20.f - glm::abs(-1 + glm::sign(e.getYOffset())) * moveSpeed / 4.f, glm::sign(e.getYOffset())), 0.5f, 100.f);
	//	}
	//	return false;
	//
}
