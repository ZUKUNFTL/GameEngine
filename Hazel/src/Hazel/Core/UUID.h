#pragma once

namespace Hazel {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t () const {
			return m_UUID;
		}

	private:
		uint64_t m_UUID;
	};
}


/*
	uuid一般是128位，16字节，这里我们只用了64位，8字节实现表示UUID。
	如果uuid作为key对应实体存储在map中时，若是unordered_map<uint64_t, std::string> m_Map,这种结构，UUID类只需提供类型转换函数。
	若是unordered_map<UUID, std::string> m_Map;需要为UUID类提供哈希函数。不然使用UUID类作为key会有bug
*/
namespace std {
	// 哈希函数
	template<>
	struct hash<Hazel::UUID> {
		std::size_t operator()(const Hazel::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}
