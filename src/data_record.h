#pragma once

#include <cstdint>
#include <string>

namespace mds
{

/**
 * @brief Represents a single validated mission data record.
 *
 * This structure is immutable after construction.
 */
struct DataRecord
{
    std::string source_id;
    double value;
    std::uint64_t timestamp;

    DataRecord() = delete;

    DataRecord(std::string id,
               double val,
               std::uint64_t ts) noexcept
        : source_id(std::move(id)),
          value(val),
          timestamp(ts)
    {
    }
};

} // namespace mds
