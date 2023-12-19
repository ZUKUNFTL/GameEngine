#include "hzpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>
namespace Hazel {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	// 根据m_OrthographicSize来计算宽高比与父类Camera的投影矩阵
	void SceneCamera::RecalculateProjection()
	{
		// TODO:根据父类的类型来计算正交投影矩阵还是透视投影矩阵
		// 这里的m_OrthographicSize除了一半，相当于5，而OrthographicCamera的m_ZoomLevel不会除以一半
		float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicSize * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;

		// 设置父类Camera的投影矩阵
		m_Projection = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
	}

}