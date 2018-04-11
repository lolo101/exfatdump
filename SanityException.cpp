#include "SanityException.h"
#include <cstdio>
#include <cstdarg>

SanityException::SanityException(const char* fmt, ...) {
    va_list args;
    snprintf(this->msg, sizeof(this->msg), fmt, args);
}

SanityException::~SanityException() {
}

