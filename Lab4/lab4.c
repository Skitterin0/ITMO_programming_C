#include <stdio.h>
#include <string.h>
#include <stdlib.h>
FILE* fin;
size_t finSize;
char* fileName;

char* FileCPY () {
    char header[10];
    fread(header, 1, 10, fin);
    char* fileCopy = (char*) malloc(finSize);
    fread(fileCopy, 1, finSize - 10, fin);

    return fileCopy;

}

void Show() {
    char* fileCopy = FileCPY();

    int  i = 0;
    while (fileCopy[i] != '\0') {
        char frame[5];
        for (int j = 0; j < 4; j++, i++) {
            frame[j] = fileCopy[i];
        }
        frame[4] = '\0';
        printf("%s:", frame);

        int size = 0;
        for (int j = 3; j >= 0; j--, i++) {
            size += (fileCopy[i] >> (j*8));
        }

        i += 2;

        for (int j = 0; j < size; j++, i++) {
            printf("%c", fileCopy[i]);
        }

        printf("\n");
    }

}

void Get(char* propName) {
    char* fileCopy = FileCPY();

    int i = 0;
    while (fileCopy[i] != '\0') {

        char frame[5];
        for (int j = 0; j < 4; j++, i++) {
            frame[j] = fileCopy[i];
        }
        frame[4] = '\0';

        if (!strcmp(frame, propName)) {
            printf("%s:", frame);

            int size = 0;
            for (int j = 3; j >= 0; j--, i++) {
                size += (fileCopy[i] >> (j*8));
            }

            i += 2;
            for (int j = 0; j < size; j++, i++) {
                printf("%c", fileCopy[i]);
            }
        }
        else {
            int size = 0;
            for (int j = 3; j >= 0; j--, i++) {
                size += (fileCopy[i] >> (j*8));
            }
            i += size + 2;
        }
    }
}

void Set(char* propName, char* propValue) {
    char* fileCopy = (char*) malloc(finSize);
    fread(fileCopy, 1, finSize, fin);

    for (int i = 0; i < finSize - 4; i++) {

        char frame[5];
        for (int j = 0; j < 4; j++) {
            frame[j] = fileCopy[i+j];
        }
        frame[4] = '\0';

        if (!strcmp(propName, frame)) {
            FILE *tmpFile = fopen("tmp.mp3", "ab");
            fwrite(fileCopy, 1, i + 10, tmpFile);

            i += 7;
            int size = fileCopy[i];
            fwrite(propValue, 1, size, tmpFile);
            fseek(fin, i + size + 3, SEEK_SET);

            char tmp[1000] = {0};
            int count;
            while (!feof(fin)) {
                count = fread(&tmp, 1, 1000, fin);
                fwrite(&tmp, 1, count, tmpFile);
            }

            fclose(fin);
            fclose(tmpFile);
            tmpFile = fopen("tmp.mp3", "rb");
            fin = fopen(fileName, "wb");

            fseek(tmpFile, 0, SEEK_SET);
            while (!feof(tmpFile)) {
                count = fread(&tmp, 1, 1000, tmpFile);
                fwrite(&tmp, 1, count, fin);
            }

            fclose(tmpFile);
            remove("tmp.mp3");
            printf("\nTag Value has been changed\n");
            break;
        }
    }

    free(fileCopy);

}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        return -1;
    }

    char* buffer;
    char* fileOperation;

    char* propName;
    char* propValue;

    int command;
    for (int i = 1; i < 3; i++) {
        fileOperation = strtok_r(argv[i], "=", &buffer);
        if (!strcmp(fileOperation, "--filepath")) {
            fileName = buffer;
        }
        else if (!strcmp(fileOperation, "--set")) {
            command = 0;
            propName = strtok_r(NULL, " ", &buffer);
            strtok_r(NULL, "=", &buffer);
            propValue = buffer;
        }
        else if (!strcmp(fileOperation, "--show")) {
            command = 1;
        }
        else if(!strcmp(fileOperation, "--get")) {
            command = 2;
            propName = buffer;
        }
        else {
            printf("Found unknown command\n");
            return -1;
        }
    }

    fin = fopen(fileName, "rb");
    fseek(fin, 0, SEEK_END);
    finSize = ftell(fin);
    rewind(fin);

    switch (command) {
        case 0:
            Set(propName, propValue);
            break;
        case 1:
            Show();
            break;
        case 2:
            Get(propName);
            break;
    }

    fclose(fin);
    return 0;
}
