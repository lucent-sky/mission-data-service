#include "data_store.h"

#include <limits>

namespace mds
{

void DataStore::add_record(const DataRecord& record)
{
    std::lock_guard<std::mutex> lock(mutex_);
    records_.push_back(record);
}

std::size_t DataStore::record_count() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return records_.size();
}

std::optional<DataStore::Stats> DataStore::compute_stats() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (records_.empty())
    {
        return std::nullopt;
    }

    double min_val = std::numeric_limits<double>::max();
    double max_val = std::numeric_limits<double>::lowest();
    double sum = 0.0;

    for (const auto& record : records_)
    {
        const double value = record.value;

        if (value < min_val)
        {
            min_val = value;
        }

        if (value > max_val)
        {
            max_val = value;
        }

        sum += value;
    }

    const std::size_t count = records_.size();
    const double average = sum / static_cast<double>(count);

    return Stats{
        count,
        min_val,
        max_val,
        average
    };
}

} // namespace mds
