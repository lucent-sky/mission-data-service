#pragma once

#include <vector>
#include <mutex>
#include <optional>

#include "data_record.h"

namespace mds
{

/**
 * @brief Thread-safe in-memory storage for mission data records.
 */
class DataStore
{
public:
    DataStore() = default;
    ~DataStore() = default;

    DataStore(const DataStore&) = delete;
    DataStore& operator=(const DataStore&) = delete;

    /**
     * @brief Adds a validated record to the store.
     */
    void add_record(const DataRecord& record);

    /**
     * @brief Returns the total number of stored records.
     */
    std::size_t record_count() const noexcept;

    /**
     * @brief Structure representing computed statistics.
     */
    struct Stats
    {
        std::size_t count;
        double min;
        double max;
        double average;
    };

    /**
     * @brief Computes statistics across all stored records.
     *
     * @return std::optional<Stats> empty if no records exist.
     */
    std::optional<Stats> compute_stats() const;

private:
    std::vector<DataRecord> records_;
    mutable std::mutex mutex_;
};

} // namespace mds
