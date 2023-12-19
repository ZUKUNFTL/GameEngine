#pragma once

#include "Hazel/Renderer/Camera.h"
namespace Hazel {

	//用于更新实体摄像机组件的宽高比
	class SceneCamera :public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }

		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		//相当于OrthographicCamera的m_ZoomLevel,即视野范围，用来放大还是放缩	
		float m_OrthographicSize = 10.0f;
		// OrthographicCamera是默认值，这里用变量表示
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		//宽高比
		// 这里初始化为0，是因为在layer层的onupdate中每帧会检测更新大小，这个值在窗口出现时计算
		float m_AspectRatio = 0.0f;
	};
}