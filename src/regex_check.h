#ifndef REGEX_CHECK_H
#define REGEX_CHECK_H

#include <stdbool.h>

#define IPV4_REG \
        "^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9]?[0-9])$"

#define PORT_REG \
        "^(6553[0-6]|655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[0-5]?[0-9]{0,4})$"

bool
correctness_check(const char *str, const char *regstr);

#endif
