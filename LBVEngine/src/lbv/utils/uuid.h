#pragma once
#include <lbv/dll.h>

#include <cstdint>
#include <xhash>

namespace LittleBigVulkan {
    class LBVAPI UUID {
    public:
        UUID();
        UUID(uint64_t override);
        UUID(const UUID&) = default;
        ~UUID();

        operator uint64_t() const { return uuid; }
    private:
        uint64_t uuid;
    };
}

namespace std {
    template<>
    struct hash<LittleBigVulkan::UUID> {
        size_t operator()(const LittleBigVulkan::UUID& id) const {
            return static_cast<uint64_t>(id);
        }
    };
}