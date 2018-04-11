#include <string.h>
#include <cstdlib>
#include <cstdio>

using namespace std;

const size_t SECTOR_SIZE = 512;

struct bootSector {
    unsigned char jmp[3];
    unsigned char name[8];
    unsigned char zeros[53];
    long int partitionOffset;
    unsigned long int volumeLength;
    int fatOffset;
    unsigned int fatLength;
    int clusterOffset;
    unsigned int clusterCount;
    unsigned int rootCluster;
    unsigned int volumeSN;
    unsigned short int fsRev;
    unsigned short int flags;
    unsigned char bytesPerSector;
    unsigned char sectorPerCluster;
    unsigned char nbFats;
    unsigned char driveSelect;
    unsigned char pctUse;
};
struct fileAllocationTable {

};

void display(bootSector* boot) {
    printf("JMP : %02hhx %02hhx %02hhx\n", boot->jmp[0], boot->jmp[1], boot->jmp[2]);
    printf("name: %s\n", boot->name);
    printf("\n");
    printf("SN     : %08x\n", boot->volumeSN);
    printf("FS rev : %04hx\n", boot->fsRev);
    printf("\n");
    printf("Bytes Per Sector  : 2^%hhu (%d)\n", boot->bytesPerSector, 1 << boot->bytesPerSector);
    printf("Sector Per Cluster: 2^%hhu (%d)\n", boot->sectorPerCluster, 1 << boot->sectorPerCluster);
    printf("\tCluster size : %lu bytes\n", (1 << boot->bytesPerSector) * (1 << boot->sectorPerCluster));
    printf("\n");
    printf("Partition offset : %ld sectors\n", boot->partitionOffset);
    printf("Volume length    : %lu sectors\n", boot->volumeLength);
    printf("Usage            : %hhu%%\n", boot->pctUse);
    printf("\n");
    printf("FAT offset : %d sectors\n", boot->fatOffset);
    printf("FAT length : %u sectors\n", boot->fatLength);
    printf("Nb of FATs : %hhu\n", boot->nbFats);
    printf("\n");
    printf("Cluster heap offset : %d sectors\n", boot->clusterOffset);
    printf("Cluster count       : %u\n", boot->clusterCount);
    printf("Root cluster        : %u\n", boot->rootCluster);
    printf("\n");
    printf("Flags :\n");
    printf("\tActive FAT: %s\n", (boot->flags & 0x01) == 0 ? "First" : "Second");
    printf("\tDirty     : %s\n", (boot->flags & 0x02) == 0 ? "Clean" : "Dirty");
    printf("\tFailure   : %s\n", (boot->flags & 0x04) == 0 ? "No" : "Yes");
    printf("\n");
    printf("Drive Select: %02hhx\n", boot->driveSelect);
}

unsigned char toPrintableChar(unsigned char byte) {
    return byte >= 0x20 && byte < 0x7f ? byte : '.';
}

void checkArgs(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: exfatdump <device>");
        exit(1);
    }
}

bool showFat(int argc, char** argv) {
    for(int n = 0; n < argc - 1; ++n) {
        char *arg = argv[n];
        if (strcmp(arg, "--fat") == 0) {
            return true;
        }
    }
    return false;
}

char* parseDrive(int argc, char** argv) {
    return argv[argc - 1];
}

int main(int argc, char** argv) {
    checkArgs(argc, argv);
    FILE* drive = fopen(parseDrive(argc, argv), "r");
    if (drive == 0) {
        printf("%s not readable", argv[1]);
        exit(1);
    }

    bootSector* boot = (bootSector*)malloc(sizeof(bootSector));
    fread(boot, sizeof(bootSector), 1, drive);
    display(boot);

    if (showFat(argc, argv)) {
        fileAllocationTable* fat = readFat(drive, boot);
        displayFat(fat);
    }
    fclose(drive);
    return 0;
}
