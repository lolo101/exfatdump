#include "Fat.h"
#include "SanityException.h"
#include <cstdlib>

Fat::Fat(FILE* drive, Boot* boot) {
    this->entries = (FatEntry*)calloc(boot->sector->clusterCount, sizeof(FatEntry));
    fseek(drive, boot->sector->fatOffset * SECTOR_SIZE, SEEK_SET);
    fread(this->entries, sizeof(FatEntry), boot->sector->clusterCount, drive);
    try {
        sanityCheck();
    } catch(SanityException* ex) {
        if (boot->sector->nbFats > 1) {
            printf("%s - Moving to backup FAT\n", ex->msg);
            fseek(drive, (boot->sector->fatOffset + boot->sector->fatLength) * SECTOR_SIZE, SEEK_SET);
            fread(this->entries, sizeof(FatEntry), boot->sector->clusterCount, drive);
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
    //TODO
    printf("Here is the FAT !\n");
}