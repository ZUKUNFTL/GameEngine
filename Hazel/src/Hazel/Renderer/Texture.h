#pragma once

#include <string>

#include "Hazel/Core/Core.h"

namespace Hazel {

	// Texture可以分为多种类型, 比如CubeTexture, Texture2D
	class Texture {

	public:
		virtual ~Texture() = default;


		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	// 注意, 这里额外包了个Texture2D类, 继承于Texture类
	class Texture2D : public Texture
	{
	public:
		// 由于之前的OpenGLTexture2D是通过读取图片路径创建的, 这里的WhiteTexture需要一种额外的创建方法
		// 与之前创建Texture, 调用Create函数相同, 仍然需要在Texture2D里加上这个函数：
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
	};
}