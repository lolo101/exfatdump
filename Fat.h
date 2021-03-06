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
    uint32_t nbEntries;
    FatEntry* entries;

    void sanityCheck();
    size_t sizeInCluster(FatEntry firstEntry);
    size_t incSize(FatEntry entry, size_t sizeInCluster);
};
#endif /* FATC_H */
