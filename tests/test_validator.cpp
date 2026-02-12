#include <gtest/gtest.h>
#include <cmath>
#include "validator.h"

using namespace mds;

TEST(ValidatorTests, AcceptsValidRecord)
{
    std::string error;
    auto result = Validator::validate("sensor_1", 42.0, 1700000000, error);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->source_id, "sensor_1");
    EXPECT_DOUBLE_EQ(result->value, 42.0);
    EXPECT_EQ(result->timestamp, 1700000000);
}

TEST(ValidatorTests, RejectsEmptySource)
{
    std::string error;
    auto result = Validator::validate("", 10.0, 1700000000, error);

    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(error.empty());
}

TEST(ValidatorTests, RejectsNaN)
{
    std::string error;
    auto result = Validator::validate("sensor", std::nan(""), 1700000000, error);

    EXPECT_FALSE(result.has_value());
}

TEST(ValidatorTests, RejectsInfinity)
{
    std::string error;
    auto result = Validator::validate("sensor", std::numeric_limits<double>::infinity(), 1700000000, error);

    EXPECT_FALSE(result.has_value());
}

TEST(ValidatorTests, RejectsZeroTimestamp)
{
    std::string error;
    auto result = Validator::validate("sensor", 10.0, 0, error);

    EXPECT_FALSE(result.has_value());
}
