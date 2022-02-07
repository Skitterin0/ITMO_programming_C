#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE* fin;
int isLast;

char* ReadString (int lenOfString) {
    char symbol = getc(fin);
    char* stringOfLog;

    if (symbol != '\n' && symbol != EOF) {
        stringOfLog = ReadString(lenOfString + 1);
        stringOfLog[lenOfString] = symbol;
    }
    else {
        stringOfLog = (char*)malloc(lenOfString + 1);
        stringOfLog[lenOfString] = '\0';
        if (symbol == EOF) {
            isLast = 1;
        }
        else {
            isLast = 0;
        }
    }
    return stringOfLog;
}

char* ParseString (char** time, char** request, char** status) {
    char* string = ReadString(0);
    char* buffer;

    strtok_r(string, "[", &buffer);
    *time = strtok_r(NULL, "]", &buffer);
    strtok_r(NULL, "\"", &buffer);
    *request = strtok_r(NULL, "\"", &buffer);
    *status = strtok_r(NULL, " ", &buffer);

    return string;
}

int ParseTime (char* timeOfRequest) {
    struct tm time;
    char* currString;
    char* buffer;

    currString = strtok_r(timeOfRequest, "/", &buffer);
    time.tm_mday = atoi(currString);
    currString = strtok_r(NULL, "/", &buffer);

    const char* month[12] = 
            {"Jan", "Feb", "Mar",
             "Apr", "May", "Jun",
             "Jul", "Aug", "Sep",
             "Oct", "Nov", "Dec"};

    for (int i = 0; i < 12; i++) {
        if (!strcmp(currString, month[i])) {
            time.tm_mon = i;
            break;
        }
    }

    currString = strtok_r(NULL, ":", &buffer);
    time.tm_year = atoi(currString) - 1900;

    currString = strtok_r(NULL, ":", &buffer);
    time.tm_hour = atoi(currString);

    currString = strtok_r(NULL, ":", &buffer);
    time.tm_min = atoi(currString);

    currString = strtok_r(NULL, " ", &buffer);
    time.tm_sec = atoi(currString);

    return mktime(&time);
}

int Period () {
    char periodOfTime[100];
    printf("\nInput time period by following example:\n");
    printf("<num> day/days <num> hour/hours <num> minute/minutes <num> second/seconds\n");
    printf("in the \"<num>\" input your value: ");
    gets(periodOfTime);

    char* buffer;
    char* currString;
    currString = strtok_r(periodOfTime, " ", &buffer);
    if (currString[0] >= 0x41 && currString[0] <= 0x7A) {
        return -1;
    }

    int period = 0;
    int number, type;

    while (currString != NULL) {
        number = atoi(currString);

        currString = strtok_r(NULL, " ", &buffer);
        if (currString !=NULL) {
            if (!strcmp(currString, "second") || !strcmp(currString, "seconds")) {
                type = 1;
            } else if (!strcmp(currString, "minute") || !strcmp(currString, "minutes")) {
                type = 60;
            } else if (!strcmp(currString, "hour") || !strcmp(currString, "hours")) {
                type = 3600;
            } else if (!strcmp(currString, "day") || !strcmp(currString, "days")) {
                type = 3600 * 24;
            } else {
                return -1;
            }

            period += number * type;
            currString = strtok_r(NULL, " ", &buffer);
            if (currString != NULL) {
                if (currString[0] >= 0x41 && currString[0] <= 0x7A) {
                    return -1;
                }
            }
        }
        else {
            return -1;
        }
    }

    return period;
}

typedef struct Request {
    int time;
    int index;
    struct Request* prev;
    struct Request* next;
} Request;

typedef struct RequestList {
    Request* head;
    Request* tail;
} RequestList;

void Push (RequestList* list, int time, int number) {
    Request* push = (Request*)calloc(1, sizeof(Request));
    push->time = time;
    push->index = number;
    push->prev = push->next = NULL;

    if (list->head == NULL) {
        list->tail = push;
    }
    else {
        push->prev = list->head;
        list->head->next = push;
    }
    list->head = push;
}

void Pop (RequestList* list) {
    Request* pop = list->tail;
    if (pop->next != NULL) {
        list->tail = pop->next;
        list->tail->prev = NULL;
    }
    else {
        list->tail = NULL;
    }
    free(pop);
}

int main() {

    char fileName[100];
    printf("Input the file with logs: ");
    gets(fileName);

    fin = fopen(fileName, "r");

    int period = Period();
    while (period == -1 || period == 0) {
        if (period == -1) {
            printf("\nIncorrect input of period\n");
            period = Period();
        }
        else {
            printf("\nPeriod can't be 0\n");
            period = Period();
        }
    }

    int requestTime;
    int badRequestCount = 0;
    isLast = 0;

    char* timeOfRequest;
    char* request;
    char* statusOfRequest;
    char* tmp;

    int maxRequests;
    int startOfPeriod;
    int endOfPeriod;

    RequestList list = {NULL, NULL};

    printf("\nRequests with 5xx ERROR:\n\n");

    for (int line = 0; !isLast; line++) {
        tmp = ParseString(&timeOfRequest, &request, &statusOfRequest);
        if (timeOfRequest != NULL) {
            requestTime = ParseTime(timeOfRequest);
            Push(&list, requestTime, line);

            while (requestTime - list.tail->time > period) {
                Pop(&list);
            }

            int countRequests = line - list.tail->index + 1;

            if (countRequests > maxRequests) {
                maxRequests = countRequests;
                startOfPeriod = list.tail->time;
                endOfPeriod = requestTime;
            }
        }

        if (statusOfRequest != NULL && statusOfRequest[0] == '5') {
            printf("%s \n", request);
            badRequestCount++;
        }

        free(tmp);
    }

    printf("\nAmount of bad requests: %d\n", badRequestCount);

    time_t timeInSeconds;
    char* tmpTime;

    timeInSeconds = startOfPeriod;
    tmpTime = asctime(localtime(&timeInSeconds));
    printf("\nStart of period with most requests: %s\n", tmpTime);

    timeInSeconds = endOfPeriod;
    tmpTime = asctime(localtime(&timeInSeconds));
    printf("End of period: %s\n", tmpTime);

    printf("Number of request in period: %d\n", maxRequests);

    fclose(fin);
    return 0;
}
