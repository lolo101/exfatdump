#ifndef FATC_H
#define FATC_H

#include "Boot.h"

typedef uint32_t FatEntry;

class Fat {
public:
    Fat(FILE* drive, Boot* boot);
    virtual ~Fat();

    void display();

private:
    FatEntry* entries;

    void sanityCheck();
};

#endif /* FATC_H */

