#include "utils.h"

#include <stdio.h>
#include <string.h>

static long long normalize(long long number, long long upper)
{
    if (number == 0) {
        return 0;
    }
    long long lower = upper / 10;
    while (number < lower)
        number *= 10;
    while (number > upper)
        number /= 10;
    return number;
}

long long decimals_to_base(int decimals)
{
    long long base = 1;
    while (decimals-- > 0)
        base *= 10;
    return base;
}

long long load_decimal(const char *string, int decimals)
{
    long long base = decimals_to_base(decimals);
    if (!strchr(string, '.')) {
        long long result;
        sscanf(string, "%lld", &result);
        return result * base;
    }

    long long large;
    long long small;
    sscanf(string, "%lld.%lld", &large, &small);

    int null_count = 0;
    char *ptr = strstr(string, ".");
    ptr++;
    while (*ptr == '0' && *ptr != '\0') {
        null_count++;
        ptr++;
    }

    int new_base = base;
    if (null_count != 0) {
        new_base /= power_of_ten(null_count);
    }

    if (large < 0) {
        return large * base - normalize(small, new_base);
    }
    return large * base + normalize(small, new_base);
}
