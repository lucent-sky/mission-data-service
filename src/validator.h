#pragma once

#include <string>
#include <optional>

#include "data_record.h"

namespace mds
{

/**
 * @brief Provides validation utilities for incoming data.
 *
 * Stateless utility class.
 */
class Validator
{
public:
    /**
     * @brief Validates fields and constructs a DataRecord if valid.
     *
     * @param source_id Source identifier string
     * @param value Numeric value
     * @param timestamp Epoch timestamp
     * @param error_message Output error message if validation fails
     *
     * @return std::optional<DataRecord> containing valid record or std::nullopt
     */
    static std::optional<DataRecord> validate(
        const std::string& source_id,
        double value,
        std::uint64_t timestamp,
        std::string& error_message);

private:
    static bool is_source_valid(const std::string& source_id) noexcept;
    static bool is_value_valid(double value) noexcept;
    static bool is_timestamp_valid(std::uint64_t timestamp) noexcept;
};

} // namespace mds
