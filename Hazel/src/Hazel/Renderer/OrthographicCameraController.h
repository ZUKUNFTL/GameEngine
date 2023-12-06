#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {

	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:

		OrthographicCameraController(float aspectRatio, bool rotation);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);
		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) {
			m_ZoomLevel = level; CalculateView();
		}

		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
	private:
		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		//宽高比
		float m_AspectRatio;
		//视野范围
		float m_ZoomLevel = 1.0f;
		//摄像机的边界类
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;

		bool m_Rotation = false;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };

		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;

	};
}
