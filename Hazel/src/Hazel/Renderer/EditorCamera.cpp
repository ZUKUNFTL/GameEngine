#include "hzpch.h"
#include "EditorCamera.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	//更新投影矩阵
	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	//更新视图矩阵
	void EditorCamera::UpdateView()
	{
		// m_Yaw = m_Pitch = 0.0f; // 锁定摄像机的旋转角度 = 不旋转
		// 焦点沿着向后的方向得到摄像机的位置
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);// 取逆才是物体的视图矩阵
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(TimeStep ts)
	{
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			/*
				若鼠标向右移动,mouse{1,0} - m_InitialMousePosition{0,0}= {1,0}
			*/
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);// 中键平移
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	// 移动焦点，从原点000移动到xy0点
	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		/*
			加上负号是因为，焦点往右方向(1, 0, 0)移动到(1, 0， 0)点，使得物体左移效果
			可见焦点移动方向与物体移动方向相反，加上负号才能使得焦点右移时变为向左移到(-1,0，0)，使得物体为右移
		*/
		/*
			假设鼠标向右移动,delta{1,0},-GetRight(){ 1,0,0}->{-1,0,0},m_FocalPoint=m_FocalPoint{0,0,0}+{-1,0,0}={-1,0,0}
					即焦点移动到-1,0,0的位置(左)是使得物体向右移动效果
			假设鼠标向左移动,delta{-1,0},-GetRigh(){-1,0,0}->{ 1,0,0},m_FocalPoint=m_FocalPoint{0,0,0}+{ 1,0,0}={1,0,0}
					即焦点移动到 1,0,0的位置(右)是使得物体向左移动效果
		*/
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		// 不加负号是因为，焦点往上方向(0,1,0)移动到(0,1,0)，使得物体下移效果
		/*
			假设鼠标向上移动,delta{ 0,1},GetUpDirection(){ 0,1,0},m_FocalPoint=m_FocalPoint{0,0,0}+{0,1,0}={ 0,1,0}
					即焦点移动到 0,1,0(上)的位置是使得物体向下移动效果
			假设鼠标向下移动,delta{0,-1},GetUpDirection(){0,-1,0},m_FocalPoint=m_FocalPoint{0,0,0}+{0,-1,0}={0,-1,0}
					即焦点移动到0,-1,0(下)的位置是使得物体向上移动效果
		*/
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}
	// 获得旋转矩阵沿着y轴的向量。
	// 返回vec3(0, 1, 0);或者 vec3(0, -1, 0)得到是往上还是往下的向量
	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// 获得旋转矩阵沿着x轴的向量。
	// 返回vec3(1, 0, 0)(右);或者 vec3(-1, 0, 0)(左)得到是往右还是往左的向量
	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	// 获得摄像机的位置。m_Distance控制了摄像机的z位置（控制实现缩放效果）
	glm::vec3 EditorCamera::CalculatePosition() const
	{
		// 当m_Pitch、m_Yaw = 0 得到vec3(0,0,-1)向后的向量
		// temp = vec3(0,0,-1) * 10 = vec3(0,0,-10)。
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}