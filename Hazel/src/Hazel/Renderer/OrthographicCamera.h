#pragma once

#include <glm/glm.hpp>

//正交摄像机用于2D渲染

namespace Hazel {

	class OrthographicCamera
	{
	public:
		//传参就是它的边界
		// 构造函数, 由于正交投影下, 需要Frustum, 默认near为-1, far为1, 就不写了
		// 不过这个构造函数没有指定Camera的位置, 所以应该是默认位置
		OrthographicCamera(float left, float right, float bottom, float top);

		//设置正交投影矩阵
		void SetProjection(float left, float right, float bottom, float top);

		// 读写Camera的位置和朝向, 这些数据是用于设置View矩阵的
		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) {m_Position = position; RecalculateViewMatrix();}
		//因为它是一个正交相机，因此它的旋转只会沿着Z轴旋转，因此只用设置一个float值
		float GetRotation()const { return m_Rotation; }
		void SetRotation(float rotation) {m_Rotation = rotation; RecalculateViewMatrix();}

		// 返回三个矩阵的接口
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		//重现计算我们的矩阵，在我们改变一些值时调用
		void RecalculateViewMatrix();
	private:
		//投影矩阵
		glm::mat4 m_ProjectionMatrix;
		//视图矩阵
		glm::mat4 m_ViewMatrix;
		//视图投影矩阵,作为计算时的Cache
		glm::mat4 m_ViewProjectionMatrix;

		// 正交投影的相机位置
		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		// 正交投影下的相机只会有绕Z轴的旋转
		float m_Rotation = 0.0f;
	};
}