#ifndef SANITYEXCEPTION_H
#define SANITYEXCEPTION_H

#include <exception>

class SanityException : public std::exception {
public:
    SanityException(const char* fmt, ...);
    virtual ~SanityException();
    char msg[80];
private:
};

#endif /* SANITYEXCEPTION_H */

