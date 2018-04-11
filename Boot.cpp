#include "Boot.h"
#include "SanityException.h"
#include <cstring>

Boot::Boot(FILE* drive) {
    this->sector = new BootSector();

    fseek(drive, 0, SEEK_SET);
    fread(this->sector, sizeof(BootSector), 1, drive);
    try {
        sanityCheck();
    } catch(SanityException* ex) {
        printf("%s\n", ex->msg);
        printf("Moving to Backup boot sector...\n");
        fseek(drive, 12 * SECTOR_SIZE, SEEK_SET);
        fread(this->sector, sizeof(BootSector), 1, drive);
        sanityCheck();
    }
}

Boot::~Boot() {
    delete this->sector;
}

void Boot::sanityCheck() {
    BootSector* sector = this->sector;
    if (
            sector->jmp[0] != 0xeb ||
            sector->jmp[1] != 0x76 ||
            sector->jmp[2] != 0x90) {
        throw new SanityException("JMP instruction invalid");
    }
    if (strcmp(sector->name, "EXFAT   ") != 0) {
        throw new SanityException("Name invalid: %s", sector->name);
    }
    for (int i = 0; i < sizeof(sector->zeros); ++i) {
        if (sector->zeros[i] != 0) {
            throw new SanityException("Not all bytes are zero in the zeroes area");
        }
    }
    if (sector->clusterCount > 4294967285) {
        throw new SanityException("Cluster count may not exceed 4294967285: %u", sector->clusterCount);
    }
    if (sector->bytesPerSector < 9 || sector->bytesPerSector > 12) {
        throw new SanityException("Bytes per sector must be in range [9..12]: %hhu", sector->bytesPerSector);
    }
    if (sector->sectorPerCluster > 25) {
        throw new SanityException("Sector per cluster must be in range [0..25]: %hhu", sector->sectorPerCluster);
    }
    if (sector->pctUse != 0xff && sector->pctUse > 100) {
        throw new SanityException("Percentage must be in range [0..100], or 256 when undefined: %hhu", sector->pctUse);
    }
}

void Boot::display() {
    printf("JMP : %02hhx %02hhx %02hhx\n", sector->jmp[0], sector->jmp[1], sector->jmp[2]);
    printf("name: %s\n", sector->name);
    printf("\n");
    printf("SN     : %08x\n", sector->volumeSN);
    printf("FS rev : %02hhx.%02hhx\n", sector->fsRev[1], sector->fsRev[0]);
    printf("\n");
    printf("Bytes Per Sector  : 2^%hhu (%d)\n", sector->bytesPerSector, 1 << sector->bytesPerSector);
    printf("Sector Per Cluster: 2^%hhu (%d)\n", sector->sectorPerCluster, 1 << sector->sectorPerCluster);
    printf("\tCluster size : %d bytes\n", (1 << sector->bytesPerSector) * (1 << sector->sectorPerCluster));
    printf("\n");
    printf("Partition offset : %ld sectors\n", sector->partitionOffset);
    printf("Volume length    : %lu sectors\n", sector->volumeLength);
    printf("Usage            : %hhu%%\n", sector->pctUse);
    printf("\n");
    printf("FAT offset : %d sectors\n", sector->fatOffset);
    printf("FAT length : %u sectors\n", sector->fatLength);
    printf("Nb of FATs : %hhu\n", sector->nbFats);
    printf("\n");
    printf("Cluster heap offset : %d sectors\n", sector->clusterOffset);
    printf("Cluster count       : %u\n", sector->clusterCount);
    printf("Root cluster        : %u\n", sector->rootCluster);
    printf("\n");
    printf("Flags :\n");
    printf("\tActive FAT: %s\n", (sector->flags & 0x01) == 0 ? "First" : "Second");
    printf("\tDirtiness : %s\n", (sector->flags & 0x02) == 0 ? "Clean" : "Dirty");
    printf("\tFailure   : %s\n", (sector->flags & 0x04) == 0 ? "No" : "Yes");
    printf("\n");
    printf("Drive Select: %02hhx\n", sector->driveSelect);
}
