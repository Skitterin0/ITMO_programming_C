#include <stdio.h>
#include <string.h>

int CountLines (FILE* in) {
    char ch;
    int lineCounter = 1;
    int isEmpty = 0;
    while ((ch = getc(in)) != EOF) {
        if (ch == '\n') {
            lineCounter++;
            isEmpty = 1;
        }
        else {
            isEmpty = 0;
        }
    }

    return lineCounter - isEmpty;
}

int CountWords (FILE* in) {
    int numberOfWords = 0;
    int isWord = 0;
    char ch;

    while ((ch = getc(in)) != EOF)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t')
            isWord = 0;
        else if (isWord == 0)
        {
            isWord = 1;
            numberOfWords++;
        }
    }
    return numberOfWords;
}

int CountBytes (FILE* in) {
    fseek(in, 0, SEEK_END);
    return ftell(in);
}

int main(int argc, char* argv[]) {
    FILE* fin;
    int l = 0;
    int c = 0;
    int w = 0;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lines")) {
            l = 1;
        }
        else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--bytes")) {
            c = 1;
        }
        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--words")) {
            w = 1;
        }
        else {
            fin = fopen(argv[i], "r");
        }
    }

    if (l) {
        printf("\tNumber of lines in your file: %d\n", CountLines(fin));
    }
    if (c) {
        printf("\tSize of your file: %d\n", CountBytes(fin));
    }
    if (w) {
        printf("\tNumber of words in your file: %d\n", CountWords(fin));
    }
    return 0;
}
