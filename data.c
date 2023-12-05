#include "data.h"

//returns queue of each line in void* form (cast to char* when using)
Queue* readFile(char* fileName) {
    Queue* fileContents = createQueue();
    char* m_line;

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read; //ssize_t

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening %s\n", fileName);
        exit(-1);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        m_line = malloc(strlen(line)+1);
        if(m_line == NULL) {
            printf("failed to malloc string for next line of file\n");
            exit(-1);
        }
        strcpy(m_line, line);
        enqueue(fileContents, (void*) m_line); //cast back to char* when using line
    }

    fclose(fp);
    if(line) free(line);
    return fileContents;
}

//WARNING: destructive to fileContents!
Queue* convertData(Queue* fileContents) {
    Queue* traceData = createQueue();
    char *line1 = NULL, *line2, *line3;
    traceItem *item;
    char tmpRAW[6], tmp[3];
    bool firstEntry = true;

    while(!isEmpty(fileContents)) {
        if(firstEntry) {
            do {
                if(line1) free(line1);
                line1 = (char*) dequeue(fileContents);
            } while(strcmp(line1, "\n") == 0 || strcmp(line1, "\r\n") == 0);
            firstEntry = false;
        } else {
            line1 = (char*) dequeue(fileContents);
        }
        if(strcmp(line1, "") == 0 || strcmp(line1, "\n") == 0 || strcmp(line1, "\r\n") == 0) {
            free(line1);
            break;
        }
        line2 = (char*) dequeue(fileContents);
        line3 = (char*) dequeue(fileContents);
        item = (traceItem*) malloc(sizeof(traceItem));
        if(item == NULL) {
            printf("failed to malloc trace item\n");
            exit(-1);
        }
        sscanf(line1, "%*s %s %x", tmpRAW, &item->addrOfInstr);
        tmp[0] = tmpRAW[1];
        tmp[1] = tmpRAW[2];
        tmp[2] = '\0';
        item->lenOfInstr = (int) strtol(tmp, NULL, 16);
        sscanf(line2, "%*s %x %*s %*s %x", &item->dstM, &item->srcM);
        free(line1);
        free(line2);
        free(line3);
        enqueue(traceData, (void*) item);
    }

    free(fileContents);
    return traceData;
}

//prints file contents
void printFileContents(Queue* fileContents) {
    if(fileContents == NULL) {
        printf("error: fileContents is NULL\n");
        exit(-1);
    }
    LLNode* current = fileContents->qFront;

    printf("PRINTING FILE CONTENTS\n\n");
    while(current != NULL) {
        printf("%s", (char*) current->qt);
        current = current->pNext;
    }
    printf("PRINTING COMPLETE\n\n");
}

//prints trace data structure
void printTraceData(Queue* traceData) {
    if(traceData == NULL) {
        printf("error: traceData is NULL\n");
        exit(-1);
    }
    LLNode* current = traceData->qFront;
    traceItem* item;

    printf("PRINTING DATA STRUCT\n\n");
    while(current != NULL) {
        item = (traceItem*) current->qt;
        printf("lenOfInstr:  %-8x  dstM: %-8x\naddrOfInstr: %-8x  srcM: %-8x\n\n", item->lenOfInstr, item->dstM, item->addrOfInstr, item->srcM);
        //printf("lenOfInstr: %-8x\taddrOfInstr: %x\ndstM:       %-8x\tsrcM:        %-8x\n\n", item->lenOfInstr, item->addrOfInstr, item->dstM, item->srcM);
        current = current->pNext;
    }
    printf("PRINTING COMPLETE\n\n");
}