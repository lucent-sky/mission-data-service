#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "data_store.h"
#include "data_record.h"

using namespace mds;

TEST(DataStoreTests, InitiallyEmpty)
{
    DataStore store;
    EXPECT_EQ(store.record_count(), 0);
    EXPECT_FALSE(store.compute_stats().has_value());
}

TEST(DataStoreTests, SingleRecordStats)
{
    DataStore store;
    store.add_record(DataRecord("s1", 10.0, 1));

    auto stats = store.compute_stats();

    ASSERT_TRUE(stats.has_value());
    EXPECT_EQ(stats->count, 1);
    EXPECT_DOUBLE_EQ(stats->min, 10.0);
    EXPECT_DOUBLE_EQ(stats->max, 10.0);
    EXPECT_DOUBLE_EQ(stats->average, 10.0);
}

TEST(DataStoreTests, MultipleRecordStats)
{
    DataStore store;

    store.add_record(DataRecord("s1", 5.0, 1));
    store.add_record(DataRecord("s2", 15.0, 2));
    store.add_record(DataRecord("s3", 10.0, 3));

    auto stats = store.compute_stats();

    ASSERT_TRUE(stats.has_value());
    EXPECT_EQ(stats->count, 3);
    EXPECT_DOUBLE_EQ(stats->min, 5.0);
    EXPECT_DOUBLE_EQ(stats->max, 15.0);
    EXPECT_DOUBLE_EQ(stats->average, 10.0);
}

TEST(DataStoreTests, ThreadSafetyUnderConcurrentWrites)
{
    DataStore store;

    constexpr int thread_count = 8;
    constexpr int records_per_thread = 1000;

    auto worker = [&store](int base)
    {
        for (int i = 0; i < records_per_thread; ++i)
        {
            store.add_record(DataRecord("s", base + i, i + 1));
        }
    };

    std::vector<std::thread> threads;

    for (int t = 0; t < thread_count; ++t)
    {
        threads.emplace_back(worker, t * 1000);
    }

    for (auto& th : threads)
    {
        th.join();
    }

    EXPECT_EQ(store.record_count(), thread_count * records_per_thread);

    auto stats = store.compute_stats();
    ASSERT_TRUE(stats.has_value());
    EXPECT_EQ(stats->count, thread_count * records_per_thread);
}
