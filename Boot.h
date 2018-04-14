#ifndef BOOTC_H
#define BOOTC_H

#include <cstdio>
#include <cstdint>

using namespace std;

struct BootSector {
    uint8_t jmp[3];
    char name[8];
    uint8_t zeros[53];
    int64_t partitionOffset;
    uint64_t volumeLength;
    int32_t fatOffset;
    uint32_t fatLength;
    int32_t clusterOffset;
    uint32_t clusterCount;
    uint32_t rootCluster;
    uint32_t volumeSN;
    uint8_t fsRev[2];
    uint16_t flags;
    uint8_t bytesPerSector;
    uint8_t sectorPerCluster;
    uint8_t nbFats;
    uint8_t driveSelect;
    uint8_t pctUse;
};

class Boot {
public:
    Boot(FILE* drive);
    virtual ~Boot();

    void display();

    BootSector* sector;
    size_t sectorSize();
    size_t clusterSize();

private:

    void sanityCheck();
};

#endif /* BOOTC_H */

