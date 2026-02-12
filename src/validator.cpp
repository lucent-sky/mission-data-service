#include "validator.h"

#include <cmath>
#include <limits>

namespace mds
{

std::optional<DataRecord> Validator::validate(
    const std::string& source_id,
    double value,
    std::uint64_t timestamp,
    std::string& error_message)
{
    if (!is_source_valid(source_id))
    {
        error_message = "Invalid source_id: must be non-empty.";
        return std::nullopt;
    }

    if (!is_value_valid(value))
    {
        error_message = "Invalid value: must be finite numeric value.";
        return std::nullopt;
    }

    if (!is_timestamp_valid(timestamp))
    {
        error_message = "Invalid timestamp: must be non-zero.";
        return std::nullopt;
    }

    error_message.clear();

    return DataRecord{source_id, value, timestamp};
}

bool Validator::is_source_valid(const std::string& source_id) noexcept
{
    return !source_id.empty();
}

bool Validator::is_value_valid(double value) noexcept
{
    if (!std::isfinite(value))
    {
        return false;
    }

    constexpr double upper_bound = std::numeric_limits<double>::max();
    constexpr double lower_bound = -std::numeric_limits<double>::max();

    return (value <= upper_bound) && (value >= lower_bound);
}

bool Validator::is_timestamp_valid(std::uint64_t timestamp) noexcept
{
    return timestamp != 0U;
}

} // namespace mds
