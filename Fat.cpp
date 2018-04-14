#include "Fat.h"
#include "SanityException.h"
#include <cstdlib>

const FatEntry END_OF_CHAIN = 0xffffffff;

Fat::Fat(FILE* drive, Boot* boot) {
    this->nbEntries = boot->sector->clusterCount + 2;
    this->entries = (FatEntry*)calloc(this->nbEntries, sizeof(FatEntry));
    size_t sectorSize = boot->sectorSize();
    fseek(drive, boot->sector->fatOffset * sectorSize, SEEK_SET);
    fread(this->entries, sizeof(FatEntry), this->nbEntries, drive);
    try {
        sanityCheck();
    } catch(SanityException* ex) {
        if (boot->sector->nbFats > 1) {
            printf("%s - Moving to backup FAT\n", ex->msg);
            fseek(drive, (boot->sector->fatOffset + boot->sector->fatLength) * sectorSize, SEEK_SET);
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
    if(entries[1] != END_OF_CHAIN) {
        throw new SanityException("Second entry invalid: %08x", entries[1]);
    }
}

void Fat::display() {
    printf("Here is the FAT !\n");
    int badCluster = 0;
    int endOfChain = 0;
    for(int i = 2; i < this->nbEntries; ++i) {
        if (this->entries[i] == 0xfffffff7) {
            ++badCluster;
        }
        if (this->entries[i] == END_OF_CHAIN) {
            ++endOfChain;
        }
    }
    printf("%d bad clusters\n", badCluster);
    printf("%d end of chains\n", endOfChain);
    //printf("FAT Entry #2366 size in cluster : %zu\n", sizeInCluster(2366));
}

size_t Fat::sizeInCluster(FatEntry firstEntry) {
    return incSize(firstEntry, 1);
}

size_t Fat::incSize(FatEntry entry, size_t sizeInCluster) {
    FatEntry nextEntry = this->entries[entry];
    return nextEntry == END_OF_CHAIN ? sizeInCluster : incSize(nextEntry, sizeInCluster + 1);
}
