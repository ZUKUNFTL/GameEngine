#include "hzpch.h"
#include "UUID.h"

#include <random>
namespace Hazel {

	//std::random_device 是 C++ 标准库提供的用于生成真正的随机数的设备类。
	static std::random_device s_RandomDevice;
	//std::mt19937_64 是 Mersenne Twister 算法的一个具体实现，用于生成伪随机数。mt19937_64 表示这个特定实现的位数是 64 位。这个类的构造函数接受一个种子作为参数，
	//用于初始化生成器。在这里，s_RandomDevice() 生成了一个种子，使得每次程序运行时，都有不同的随机数序列。
	static std::mt19937_64 s_Engine(s_RandomDevice());
	//std::uniform_int_distribution 是一个分布类，用于将生成的伪随机数映射到一个指定的整数范围。在这里，uint64_t 表示生成的随机数的类型是 64 位的无符号整数。
	//如果你希望生成其他类型的随机数，你可以在 uniform_int_distribution 的模板参数中指定。
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_Engine)) 
	{

	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{

	}

}
