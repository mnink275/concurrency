#include <gtest/gtest.h>

#include "core/thread-pool.hpp"

namespace ink::test {
    
TEST(KThreadPool, Units) {
    KThreadPool<int> tp(5);
    EXPECT_EQ(tp.GetCapacity(), 5);
}

}  // namespace ink::test
