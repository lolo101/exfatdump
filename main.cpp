#include "Boot.h"
#include "Fat.h"
#include "SanityException.h"
#include <cstring>
#include <iostream>

using namespace std;

void checkArgs(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: exfatdump <device>");
        exit(1);
    }
}

char* parseDrive(int argc, char** argv) {
    return argv[argc - 1];
}

int analyse(FILE* drive) {
    try {
        Boot boot = Boot(drive);
        boot.display();

        printf("\n----------------\n\n");
        Fat fat = Fat(drive, &boot);
        fat.display();

        return 0;
    } catch (SanityException* ex) {
        cout << ex->msg << endl;
        return -1;
    }
}

int main(int argc, char** argv) {
    checkArgs(argc, argv);
    FILE* drive = fopen(parseDrive(argc, argv), "r");
    if (drive == 0) {
        printf("%s not readable\n", argv[1]);
        exit(1);
    }

    int retCode = analyse(drive);
    fclose(drive);

    return retCode;
}
