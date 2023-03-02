#pragma once
#include <lbv/dll.h>
#include <string>

namespace LittleBigVulkan {
	using ResourceKey = uint64_t;

	class LBVAPI ResourceID {
	public:
		ResourceID(const std::string& asset);
		ResourceID() : file("\0"), id(0) {}
		uint64_t getID() const { return id; }
		std::string getAsset() const { return file; }
		operator ResourceKey() const { return getID(); }
		operator bool() const { return id != 0; }
		bool operator==(const ResourceID & other) const {
			return id == other.id;
		}
	private:
		std::string file;
		uint64_t id;
	};

}
namespace std {
	template<>
	struct hash<LittleBigVulkan::ResourceID> {
		size_t operator()(const LittleBigVulkan::ResourceID& id) const {
			return id.getID();
		}
	};
}