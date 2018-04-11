#include "Fat.h"
#include "SanityException.h"
#include <cstdlib>

Fat::Fat(FILE* drive, Boot* boot) {
    this->nbEntries = boot->sector->clusterCount + 2;
    this->entries = (FatEntry*)calloc(this->nbEntries, sizeof(FatEntry));
    fseek(drive, boot->sector->fatOffset * SECTOR_SIZE, SEEK_SET);
    fread(this->entries, sizeof(FatEntry), this->nbEntries, drive);
    try {
        sanityCheck();
    } catch(SanityException* ex) {
        if (boot->sector->nbFats > 1) {
            printf("%s - Moving to backup FAT\n", ex->msg);
            fseek(drive, (boot->sector->fatOffset + boot->sector->fatLength) * SECTOR_SIZE, SEEK_SET);
            fread(this->entries, sizeof(FatEntry), this->nbEntries, drive);
            sanityCheck();
        } else {
            throw ex;
        }
    }
}

Fat::~Fat() {
    free(this->entries);
}

void Fat::sanityCheck() {
    FatEntry* entries = this->entries;
    if(entries[0] != 0xfffffff8) {
        throw new SanityException("Media type entry invalid: %08x", entries[0]);
    }
    if(entries[1] != 0xffffffff) {
        throw new SanityException("Second entry invalid: %08x", entries[1]);
    }
}

void Fat::display() {
    printf("Here is the FAT !\n");
    int badCluster = 0;
    for(int i = 2; i < this->nbEntries; ++i) {
        if (this->entries[i] == 0xfffffff7) {
            ++badCluster;
        }
    }
    printf("%d bad clusters\n", badCluster);
}