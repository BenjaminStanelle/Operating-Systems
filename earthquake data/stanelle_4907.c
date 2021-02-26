/* Benjamin Stanelle
* 10/22/2020
* 1001534907
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct arrThread {
    int thread;
    int maxThreads;
    char** array;
};

#define strMax 256
#define lineMax 13445
void insertionSort(char*** arr, int lowerBound, int rowS, int columnS, int threadNum);
void bubble_Sort(char*** arr, int lowerBound,int rowS, int columnS, int processesNum);
void print_Arr(char*** arr, int rows, int cols);
void* createThread(void* arr);

int main(void) {
    time_t t;
    pid_t p, pp, pid, pid1, pid2;
    int i, j, k, length = 0, processes, sortTime;
    char fileName[strMax];
    char sortedFileName[strMax+6]="sorted_";


    printf("Enter the file name: ");
    scanf("%s", fileName);
    printf("Enter the number of threads(1 or 2 or 4 or 10): ");
    scanf("%d", &processes);

    FILE* fp = fopen(fileName, "r");

    if (fp == NULL) {

        printf("Error opening file, file doesn't exist");
        exit(2);
    }
    
    

    if (processes == 2) {

        struct arrThread b;
        b.array = (char**)calloc((lineMax), sizeof(char*));   //allocate array on heap
        for (i = 0; i < lineMax; i++) {
            b.array[i] = (char*)calloc(strMax, sizeof(char));
        }

        i = 0;
        j = 0;
        while (i < lineMax) {      //put all strings in dynamic array

            while (fgets(b.array[j], strMax, fp) != NULL) {
                j++;
            }
            i++;
        }
        b.thread = 0;
        b.maxThreads = processes;

        pthread_t newthread;
        int sizeUp = 0;
        int sizeLow = 0;
        sizeLow = 1;     //0 through 6722
        sizeUp = (lineMax / 2);

        time_t begin = time(NULL);

        pthread_create(&newthread, NULL, createThread, &b);

        //bubble_Sort(&b.array, sizeLow, sizeUp, strMax, processes);

        pthread_join(newthread, NULL);
        insertionSort(&b.array, 2, lineMax, strMax, 1); 

        time_t end = time(NULL);

        printf("Time taken for 2 threads sort is: %.6g seconds\n", (double)(end - begin));

        strcat(sortedFileName, fileName);       //Reading the sorted data to a file

        fclose(fp);

        FILE* fpW = fopen(sortedFileName, "w");
        if (fpW == NULL) {
            printf("Error: file does not exist");
            exit(2);
        }
        for (i = 0; i < lineMax; i++) {
            fputs(b.array[i], fpW);
        }
        printf("Placed sorted data in file: %s\n", sortedFileName);
        fclose(fpW);

        for (i = 0; i < lineMax; i++) {         //freeing dynamically allocated memory.
            free(b.array[i]);
        }
        free(b.array);

    }

    else if (processes == 4) {
        struct arrThread b;
        b.array = (char**)calloc((lineMax), sizeof(char*));   //allocate array on heap
        for (i = 0; i < lineMax; i++) {
            b.array[i] = (char*)calloc(strMax, sizeof(char));
        }

        i = 0;
        j = 0;
        while (i < lineMax) {      //put all strings in dynamic array

            while (fgets(b.array[j], strMax, fp) != NULL) {
                j++;
            }
            i++;
        }
        b.thread = 0;
        b.maxThreads = processes;

        pthread_t newthreads[processes];

        time_t begin = time(NULL);
        for (i = 0; i < processes; i++) {
            pthread_create(&newthreads[i], NULL, createThread, &b);
        }

        //bubble_Sort(&buffer, sizeLow, sizeUp, strMax, processes);

        for (i = 0; i < processes; i++) {
            pthread_join(newthreads[i], NULL);
        }

        //insertionSort(&b.array, 2, lineMax, strMax, 1);

        time_t end = time(NULL);

        printf("\nTime taken for 4 threads sort is: %.6g seconds\n", (double)(end - begin));

        strcat(sortedFileName, fileName);       //Reading the sorted data to a file

        fclose(fp);

        FILE* fpW = fopen(sortedFileName, "w");
        if (fpW == NULL) {
            printf("Error: file does not exist");
            exit(2);
        }
        for (i = 0; i < lineMax; i++) {
            fputs(b.array[i], fpW);
        }
        printf("Placed sorted data in file: %s\n", sortedFileName);
        fclose(fpW);

        for (i = 0; i < lineMax; i++) {         //freeing dynamically allocated memory.
            free(b.array[i]);
        }
        free(b.array);
    }

    else if (processes == 10) {

    }

    else if (processes==1){
        char** buffer = (char**)calloc((lineMax), sizeof(char*));   //allocate array on heap
        for (i = 0; i < lineMax; i++) {
            buffer[i] = (char*)calloc(strMax, sizeof(char));
        }

        i = 0;
        j = 0;
        while (i < (sizeof(buffer) / sizeof(buffer[0]))) {      //put all strings in dynamic array

            while (fgets(buffer[j], strMax, fp) != NULL) {
                j++;
            }
            i++;
        }
        time_t begin = time(NULL);
        bubble_Sort(&buffer, 0,lineMax, strMax, processes);
        time_t end = time(NULL);
        printf("Time taken for 1 process sort is: %.6g seconds\n", (double)(end - begin));
        //print_Arr(buffer, lineMax, strMax);

        strcat(sortedFileName, fileName);       //Reading the sorted data to a file

        fclose(fp);

        FILE* fpW = fopen(sortedFileName, "w");
        if (fpW == NULL) {
            printf("Error: file does not exist");
            exit(2);
        }
        for (i = 0; i < lineMax; i++) {
            fputs(buffer[i], fpW);
        }
        printf("Placed sorted data in file: %s\n", sortedFileName);
        fclose(fpW);

        for (i = 0; i < lineMax; i++) {         //freeing dynamically allocated memory.
            free(buffer[i]);
        }
        free(buffer);
    }

    else {
        printf("Please try again and enter a valid process number(1 or 2 or 4 or 10).");
        exit(2);
    }
   

    return 0;
}

void bubble_Sort(char*** arr, int lowerBound, int rows, int cols, int processesNum) {
    int i, j;
    char* tempS1;
    char* tempS2;
    char buffer1[strMax];
    char buffer2[strMax];
    char buffer3[strMax];
    char buffer4[strMax];
    double nextLat = 0, curLat = 0;
    if (processesNum == 1) {
        for (i = 1; i < rows; i++) {        //skip row one since it is names e.g lattitude, location
            for (j = 1; j < rows - i; j++) {
                pthread_mutex_lock(&mutex);
                strcpy(buffer1, *((*(arr)+j))); //copy arr[j] into buffer
                tempS1 = strtok(buffer1, ",");      //parse once
                tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value
                        
                strcpy(buffer4, *((*(arr)+j + 1)));     //copy arr[j+1] into buffer
                tempS2 = strtok(buffer4, ",");
                tempS2 = strtok(NULL, ",");             //parse twice
                pthread_mutex_unlock(&mutex);
                nextLat = atof(tempS2);         //convert to float
                curLat = atof(tempS1);

                if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                    //swap
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer2, *((*(arr)+j + 1)));     //swap
                    strcpy(buffer3, *((*(arr)+j)));
                    
                    strcpy(*((*(arr)+j + 1)), buffer3);
                    strcpy(*((*(arr)+j)), buffer2);
                    pthread_mutex_unlock(&mutex);

                }
            }

        }
    }
    else if (processesNum == 2) {           
        for (i = lowerBound; i < rows; i++) {
            for (j = lowerBound; j < rows - i; j++) {
                pthread_mutex_lock(&mutex);
                strcpy(buffer1, *((*(arr)+j)));
                tempS1 = strtok(buffer1, ",");
                tempS1 = strtok(NULL, ",");
                strcpy(buffer4, *((*(arr)+j + 1)));
                tempS2 = strtok(buffer4, ",");
                tempS2 = strtok(NULL, ",");
                pthread_mutex_unlock(&mutex);
                nextLat = atof(tempS2);
                curLat = atof(tempS1);

                if ((curLat > nextLat)) {
                    //swap
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer2, *((*(arr)+j + 1)));
                    strcpy(buffer3, *((*(arr)+j)));

                    
                    strcpy(*((*(arr)+j + 1)), buffer3);
                    strcpy(*((*(arr)+j)), buffer2);
                    pthread_mutex_unlock(&mutex);

                }
            }

        }
    }
    return;
}

void print_Arr(char*** arr, int rows, int cols) {    //change to print to file
    int i;
    for (i = 0; i < rows; i++) {
        printf("%s", *arr[i]);
    }
    return;
}

void insertionSort(char*** arr, int lowerBound, int rowS, int columnS, int threadNum){
    
    int i, j;
    char* tempS1;
    char* tempS2;
    char buffer1[strMax];
    char buffer2[strMax];
    char buffer3[strMax];
    char buffer4[strMax];
    double prevLat = 0, curLat = 0, key=0;

    for (i = lowerBound; i < rowS; i++) {

        strcpy(buffer1, *((*(arr)+i)));       //buffer1 or 2= key
        strcpy(buffer2, buffer1); //copy arr[j] into buffer
        tempS1 = strtok(buffer2, ",");      //parse once
        tempS1 = strtok(NULL, ",");
        
        j = i - 1;
        
        strcpy(buffer3, *((*(arr)+j)));
        strcpy(buffer4, buffer3); //copy arr[j] into buffer
        tempS2 = strtok(buffer4, ",");      //parse once
        tempS2 = strtok(NULL, ",");

        prevLat = atof(tempS1);         //convert to float
        curLat = atof(tempS2);

        while ((j >= 1) && (prevLat< curLat)) {

            strcpy(*((*(arr)+(j+1))), buffer3);


            --j;

            strcpy(buffer3, *((*(arr)+j)));
            strcpy(buffer4, buffer3); 
            tempS2 = strtok(buffer4, ",");      
            tempS2 = strtok(NULL, ",");

            curLat = atof(tempS2);
        }
        
        strcpy(*((*(arr)+(j+1))), buffer1);
    }
    return;
    
}

void* createThread(void* arr) {
    struct arrThread* c = (struct arrThread*)arr;
    if (c->maxThreads == 2) {
        if(c->thread!=c->maxThreads){
            c->thread = (c->thread) + 1;
            printf("%d", c->thread - 1);
        }
        int sizeLow = (lineMax / 2) + 1;        //6723 through lineMax
        int sizeUp = lineMax;
        int i, j;
        char* tempS1;
        char* tempS2;
        char buffer1[strMax];
        char buffer2[strMax];
        char buffer3[strMax];
        char buffer4[strMax];
        double nextLat = 0, curLat = 0;
        for (i = 1; i < sizeUp; i++) {        //skip row one since it is names e.g lattitude, location
            for (j = sizeLow; j < sizeUp - i; j++) {
                pthread_mutex_lock(&mutex);
                strcpy(buffer1, (c->array[j])); //copy buff[j] into buffer
                tempS1 = strtok(buffer1, ",");      //parse once
                tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value

                strcpy(buffer4, (c->array[j]));     //copy buff[j+1] into buffer
                tempS2 = strtok(buffer4, ",");
                tempS2 = strtok(NULL, ",");             //parse twice

                pthread_mutex_unlock(&mutex);

                nextLat = atof(tempS2);         //convert to float
                curLat = atof(tempS1);

                if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                    //swap
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer2, (c->array[j + 1]));     //swap
                    strcpy(buffer3, (c->array[j]));

                    strcpy((c->array[j + 1]), buffer3);
                    strcpy((c->array[j]), buffer2);
                    pthread_mutex_unlock(&mutex);

                }
            }

        }

    }
    else if (c->maxThreads == 4) {
        c->thread = 1;
        pthread_mutex_lock(&mutex);
        if (c->thread == 1) {
            c->thread = 2;
            printf("%d", c->thread - 1);
            pthread_mutex_unlock(&mutex);
            int sizeLow = 1;        //6723 through lineMax
            int sizeUp = (lineMax / 4);
            int i, j;
            char* tempS1;
            char* tempS2;
            char buffer1[strMax];
            char buffer2[strMax];
            char buffer3[strMax];
            char buffer4[strMax];
            double nextLat = 0, curLat = 0;
            for (i = 1; i < sizeUp; i++) {        //skip row one since it is names e.g lattitude, location
                for (j = sizeLow; j < sizeUp - i; j++) {
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer1, (c->array[j])); //copy buff[j] into buffer
                    tempS1 = strtok(buffer1, ",");      //parse once
                    tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value

                    strcpy(buffer4, (c->array[j]));     //copy buff[j+1] into buffer
                    tempS2 = strtok(buffer4, ",");
                    tempS2 = strtok(NULL, ",");             //parse twice

                    pthread_mutex_unlock(&mutex);

                    nextLat = atof(tempS2);         //convert to float
                    curLat = atof(tempS1);

                    if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                        //swap
                        pthread_mutex_lock(&mutex);
                        strcpy(buffer2, (c->array[j + 1]));     //swap
                        strcpy(buffer3, (c->array[j]));

                        strcpy((c->array[j + 1]), buffer3);
                        strcpy((c->array[j]), buffer2);
                        pthread_mutex_unlock(&mutex);

                    }
                }

            }
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        if (c->thread == 2) {
            c->thread = 3;
            printf("%d", c->thread-1);
           pthread_mutex_unlock(&mutex);
            int sizeLow = (lineMax / 4) + 1;        //6723 through lineMax
            int sizeUp = lineMax/2;
            int i, j;
            char* tempS1;
            char* tempS2;
            char buffer1[strMax];
            char buffer2[strMax];
            char buffer3[strMax];
            char buffer4[strMax];
            double nextLat = 0, curLat = 0;
            for (i = 1; i < sizeUp; i++) {        //skip row one since it is names e.g lattitude, location
                for (j = sizeLow; j < sizeUp - i; j++) {
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer1, (c->array[j])); //copy buff[j] into buffer
                    tempS1 = strtok(buffer1, ",");      //parse once
                    tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value

                    strcpy(buffer4, (c->array[j]));     //copy buff[j+1] into buffer
                    tempS2 = strtok(buffer4, ",");
                    tempS2 = strtok(NULL, ",");             //parse twice

                    pthread_mutex_unlock(&mutex);

                    nextLat = atof(tempS2);         //convert to float
                    curLat = atof(tempS1);

                    if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                        //swap
                        pthread_mutex_lock(&mutex);
                        strcpy(buffer2, (c->array[j + 1]));     //swap
                        strcpy(buffer3, (c->array[j]));

                        strcpy((c->array[j + 1]), buffer3);
                        strcpy((c->array[j]), buffer2);
                        pthread_mutex_unlock(&mutex);

                    }
                }

            }
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        if (c->thread == 3) {
            c->thread = 4;
            printf("%d", c->thread - 1);
            pthread_mutex_unlock(&mutex);
            int sizeLow = (lineMax / 2) + 1;        //6723 through lineMax
            int sizeUp = (lineMax / 2)+(lineMax/4);
            int i, j;
            char* tempS1;
            char* tempS2;
            char buffer1[strMax];
            char buffer2[strMax];
            char buffer3[strMax];
            char buffer4[strMax];
            double nextLat = 0, curLat = 0;
            for (i = 1; i < sizeUp; i++) {        //skip row one since it is names e.g lattitude, location
                for (j = sizeLow; j < sizeUp - i; j++) {
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer1, (c->array[j])); //copy buff[j] into buffer
                    tempS1 = strtok(buffer1, ",");      //parse once
                    tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value

                    strcpy(buffer4, (c->array[j]));     //copy buff[j+1] into buffer
                    tempS2 = strtok(buffer4, ",");
                    tempS2 = strtok(NULL, ",");             //parse twice

                    pthread_mutex_unlock(&mutex);

                    nextLat = atof(tempS2);         //convert to float
                    curLat = atof(tempS1);

                    if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                        //swap
                        pthread_mutex_lock(&mutex);
                        strcpy(buffer2, (c->array[j + 1]));     //swap
                        strcpy(buffer3, (c->array[j]));

                        strcpy((c->array[j + 1]), buffer3);
                        strcpy((c->array[j]), buffer2);
                        pthread_mutex_unlock(&mutex);

                    }
                }

            }
            pthread_exit(0);
        }
        pthread_mutex_lock(&mutex);
        if (c->thread == 4) {
            c->thread = 5;
            printf("%d", c->thread - 1);
            pthread_mutex_unlock(&mutex);
            int sizeLow = (lineMax / 2) + (lineMax / 4) + 1;        //6723 through lineMax
            int sizeUp = lineMax;
            int i, j;
            char* tempS1;
            char* tempS2;
            char buffer1[strMax];
            char buffer2[strMax];
            char buffer3[strMax];
            char buffer4[strMax];
            double nextLat = 0, curLat = 0;
            for (i = 1; i < sizeUp; i++) {        //skip row one since it is names e.g lattitude, location
                for (j = sizeLow; j < sizeUp - i; j++) {
                    pthread_mutex_lock(&mutex);
                    strcpy(buffer1, (c->array[j])); //copy buff[j] into buffer
                    tempS1 = strtok(buffer1, ",");      //parse once
                    tempS1 = strtok(NULL, ",");         //parse twice to get lattitude value

                    strcpy(buffer4, (c->array[j]));     //copy buff[j+1] into buffer
                    tempS2 = strtok(buffer4, ",");
                    tempS2 = strtok(NULL, ",");             //parse twice

                    pthread_mutex_unlock(&mutex);

                    nextLat = atof(tempS2);         //convert to float
                    curLat = atof(tempS1);

                    if ((curLat > nextLat)) {       //if a[j]>a[j+1]
                        //swap
                        pthread_mutex_lock(&mutex);
                        strcpy(buffer2, (c->array[j + 1]));     //swap
                        strcpy(buffer3, (c->array[j]));

                        strcpy((c->array[j + 1]), buffer3);
                        strcpy((c->array[j]), buffer2);
                        pthread_mutex_unlock(&mutex);

                    }
                }

            }
            pthread_exit(0);
        }
       

    }
    pthread_exit(0);
}
