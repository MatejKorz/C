#include "libs/cut.h"
#include "libs/utils.h"
#include "libs/mainwrap.h"

#include "../src/decimals.h"

TEST(decimals_to_base) {
    int value;

    SUBTEST(2) {
        value = decimals_to_base(2);
        ASSERT(value == 100);
    }
    SUBTEST(1) {
        value = decimals_to_base(1);
        ASSERT(value == 10);
    }
    SUBTEST(0) {
        value = decimals_to_base(0);
        ASSERT(value == 1);
    }
    SUBTEST(-1) {
        value = decimals_to_base(-1);
        ASSERT(value == 1);
    }
}

TEST(load_decimal) {
    long long value;
    SUBTEST("1, 2 -> 100") {
        value = load_decimal("1", 2);
        ASSERT(value == 100);
    }
    SUBTEST("1.23, 2 -> 123") {
        value = load_decimal("1.23", 2);
        ASSERT(value == 123);
    }
    SUBTEST("10.01, 2 -> 1001") {
        value = load_decimal("10.01", 2);
        printf("%lld", value);
        ASSERT(value ==  1001);
    }
    SUBTEST("-10.12, 2 -> -112") {
        value = load_decimal("-10.12", 2);
        printf("%lld", value);
        ASSERT(value == -1012);
    }
}