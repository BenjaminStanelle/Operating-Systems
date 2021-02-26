/*  Benjamin Stanelle
* CSE-3320-001
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

int main(void) {
    long path_max;
    pid_t child;
    DIR* d;        //Directory Pointer
    struct dirent* de;     //directory 
    int i, c, k, j, dirNum, fileNum, p, err, temp;
    char s[PATH_MAX+1], cmd[PATH_MAX + 1];          //fixed length buffers. 256
    char files[1024][NAME_MAX];
    char directories[1024][NAME_MAX];
    time_t t;


    while (1) {
        k = 'N';
        path_max = pathconf(".", _PC_PATH_MAX); //Returns the max number of characters in the current directory path.

        dirNum = 0;
        getcwd(s, PATH_MAX+1);
        if (s == NULL) {
            printf("Current working directory not found");
            exit(0);
        }
        d = opendir(".");
        if (d == NULL) {
            printf("Cannot open the current directory.");
            perror(" /error");
            closedir(d);
            break;
        }
        while ((de = readdir(d)) != NULL) {         //reading directories into an array
            if ((de->d_type) == DT_DIR) {
                strcpy(directories[dirNum], de->d_name);
                dirNum++;
            }
        }
        
        
        c = 0;
        i = 0;
        j = 0;
        fileNum = 0;
        rewinddir(d);
        if (d == NULL) {
            printf("Cannot open the current directory.");
            perror(" /error");
            closedir(d);
            break;
        }
        while ((de = readdir(d)) != NULL) {     //reading file names into an array
            if (((de->d_type) == DT_REG))
            {
                strcpy(files[fileNum], de->d_name);
                fileNum++;
            }
        }
        closedir(d);
        while (k != 'Q') {
            if ((i % 5) == 0) {         //every 5 times you print file names it asks for a new char
                t = time(NULL); //Current time in seconds
                printf("-----------------------------------------------\n");
                printf("Time: %s\n", ctime(&t));   //ctime puts time into readable form instead of seconds
                printf("-----------------------------------------------\n");
                printf("\nCurrent Directory: %s \n", s);
                p = 0;
                while (p < dirNum) {
                    printf(" ( %d Directory:  %s ) \n", p, directories[p]);
                        p++;
                }
                printf("-----------------------------------------\n");
                printf("Hit N for Next 5 files or P for the previous 5 files\n Press Q to continue:\n");

                while(1) {    //if not equal to N Q P keep asking for char
                    k = getchar(); getchar();
                    if (k != 'N' || k != 'Q' || k != 'P') {
                        break;
                    }
                }
            }
            if (k == 'N' && ((fileNum % 2)==0)) {     //checks if file number is even for next
                if (j < fileNum) {
                        printf(" ( %d File:  %s ) \n", i++, files[j]);  //print those files
                    j++;
                    
                    if (j == fileNum && fileNum % 5 == 1) {        //We print every 5 if not at 5 then adds to 5
                        i +=4;
                    }

                    if (j == fileNum && fileNum % 5 == 4) {         
                        i += 1;
                    }

                    if (j == fileNum && fileNum % 5 == 3) {         
                        i += 2;
                    }
                    if (j == fileNum && fileNum % 5 == 2) {        
                        i += 3;
                    }

                }
            }
            if (k == 'N'&& ((fileNum % 2) != 0)) { //checks if file number is odd for next
                if (j < fileNum) {
                        printf(" ( %d File:  %s ) \n", i++, files[j]);
                    j++;
                    if (j==fileNum && fileNum % 5 == 1) {
                        i += 4;
                    }
                    if (j == fileNum && fileNum % 5 == 3) {
                        i += 2;
                    }

                    if (j == fileNum && fileNum % 5 == 2) {         
                        i += 3;
                    }
                    if (j == fileNum && fileNum % 5 == 4) {         
                        i += 1;
                    }

                    
                }
            }
            if (k == 'P' && ((fileNum % 2) != 0)) {  //checks if the file number is odd for previous
                if (j > 0) {
                    if (j == fileNum && fileNum % 5 == 1) {         
                        i -= 4;
                    }

                    if (j == fileNum && fileNum % 5 == 3) {         
                        i -= 2;
                    }

                    if (j == fileNum && fileNum % 5 == 2) {        
                        i -= 3;
                    }

                    if (j == fileNum && fileNum % 5 == 4) {         
                        i -= 1;
                    }
                    --j;
                    printf(" ( %d File:  %s ) \n", --i, files[j]);

                }
            }

            if (k == 'P' && ((fileNum % 2) == 0)) {     //even number of files for when p is pressed
                if (j > 0) {
                    if (j == fileNum && fileNum % 5 == 1) {         
                        i -= 4;
                    }

                    if (j == fileNum && fileNum % 5 == 2) {         
                        i -= 3;
                    }

                    if (j == fileNum && fileNum % 5 == 3) {         
                        i -= 2;
                    }
                    if (j == fileNum && fileNum % 5 == 4) {         
                        i -= 1;
                    }
                    --j;
                    printf(" ( %d File:  %s ) \n", --i, files[j]);

                }
            }
            
        }

        printf("-----------------------------------------\n");

        printf("Operation:\nq to quit\ne to edit\nr to run\nc to change directory\n");
        c = getchar(); getchar();
        switch (c) {
        case 'q': printf("\nGoodbye.\n"); exit(0); /* quit */
        case 'e': printf("Edit what?:");
            scanf("%s", s);
            strcpy(cmd, "pico ");
            strcat(cmd, s);
            system(cmd);
            break;
        case 'r': printf("Run what?:");
            scanf("%s", cmd);

            err=system(cmd);
            if (err == -1) {
                printf("Child process could not be created, or its status could not  be retrieved");
                    exit(1);
            }
            else if (err==0) {
                printf("No Shell Available");
                exit(1);
            }
            break;
        case 'c':
            temp = 0;
            printf("\nEnter the directory number you would like to move to (0 is current directory): ");
            scanf("%d", &temp);
            if (temp < dirNum && temp >= 0) {
                printf("Press enter to continue to your new directory.");
                getchar();
                if (chdir(directories[temp]) != 0) {
                    printf("\nDirectory could not be changed to %s:", cmd);
                    perror(" /error");
                }
            }
            else {
                printf("\nPlease enter a number between 1 and %d.\n", dirNum);
            }
            break;

        }
    }
}



/*
Sources:
http://www.qnx.com/developers/docs/6.3.2/neutrino/lib_ref/g/getcwd.html


*/
