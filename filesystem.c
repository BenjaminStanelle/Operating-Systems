/*Benjamin Stanelle
1001534907
11/28/2020
Windows 10 Pro x64 
*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK_SIZE 4096		//bytes
#define SECTOR_SIZE 512
#define INODES_PER_BLOCK 128
#define POINTERS_PER_BLOCK 1024
#define POINTERS_PER_INODE 5

#define MAGIC_NUMBER 0xf0f03410
/*
1). Superblock- How many inodes and datablocks, where inode table begins.
2). bitmap for inode and data to see if blocks are free
3). inode (structure)second few block sthat hold meta info
4). Data files

blk = (inumber * sizeof(inode_t)) / blockSize;
sector = ((blk * blockSize) + inodeStartAddr) / sectorSize;

extent- is simply a disk pointer plus a length (in blocks); thus, instead of requiring a pointer for every block of a file, all one needs is a pointer and a length to specify the on-disk location of a file.
*/
struct SuperBlock {		//12 bytes		
	//int magic;
	int Blocks;			//# blocks in FS
	int InodeBlocks;	//# of blocks reserved for inodes
	int Inodes;			//# inodes in FS
};
struct Inode { //66 bytes
	int valid;	//4 bytes
	int Fsize;		//4 bytes, size of file
	int timeDate;	//in seconds
	int Inum;
	int Direct[POINTERS_PER_INODE];		//8*5= 50bytes
	int Indirect;	// 8bytes
	int user;
};
union Block {		
	struct SuperBlock Super;	
	struct Inode Inodes[INODES_PER_BLOCK];	
	int Pointers[POINTERS_PER_BLOCK];	
	char Data[BLOCK_SIZE];		//	4096 bytes
};
struct FNT {
	int valid;
	char fileName[256];

};


int main(){
	char filename[256];
	int blocks, i, j, inodeBlocksNumber, choice, filenum, boolean;

	printf("Enter the number of blocks: ");
	scanf("%d", &blocks);
	if(blocks <100){
		printf("Please try a larger block size.");
		exit(1);
	}
	else if (blocks > 1000) {
		printf("Please try a smaller block size.");
		exit(1);
	}
	union Block block[blocks];

	//super block initialization
	block[0].Super.Blocks = blocks;
	block[0].Super.InodeBlocks = block[0].Super.Blocks / 10;	//blocks for inodes
	block[0].Super.Inodes = INODES_PER_BLOCK * block[0].Super.InodeBlocks;	//total inodes
	//super block initialized

	//initializing valid bits in fnt and inodes(1-10) to 0
	struct FNT fnt[block[0].Super.Inodes];	//1kb of file names
	for (i = 0; i < block[0].Super.Inodes; i++) {	//initializing valid bits to 0 in FNT and inodes
		fnt[i].valid = 0;
		if (i >= 1 && (i<= block[0].Super.InodeBlocks)) {	//superblock comes first
			for (j = 0; j < INODES_PER_BLOCK; j++) {
				block[i].Inodes[j].valid = 0;
			}
		}
	}
	//loading in a file name
	/*char readme[] = "readme.txt";
	strcpy(fnt[0].fileName, readme);
	fnt[0].valid = 1;
	printf("%s", fnt[0].fileName);
	block[0].Inodes[0].valid = 1;
	*/
	struct stat fileStat;		//file stat for inode info
	while (1) {
		printf("\nEnter a command for what you would like to do.\n");
		printf("1). List files\t\t2). Remove Name\t\t 3). Rename oldname newname\n4). Put ExternalFile\t5). Get ExternalFile\t 6). User name\n7).link/unlink\t\t8). Show Username/change 9). Savefs\n10).Openfs\nCommand: ");
		scanf("%d", &choice);
		switch (choice) {
		case 1:			//list files
			printf("Files on this disk:\n");
			for (i = 0; i < block[0].Super.Inodes; i++) {
				if (fnt[i].valid == 1) {
					printf("%d). %s\n", i, fnt[i].fileName);

					if ((i <= block[0].Super.InodeBlocks)) {	//superblock comes first
						for (j = 0; j < INODES_PER_BLOCK; j++) {
							if (block[i].Inodes[j].valid == 1) {
								printf("Size: %d\nInum: %d\nTime: %d", block[i].Inodes[j].Fsize, block[i].Inodes[j].Inum, block[i].Inodes[j].timeDate);
							}

						}
					}
				}
			}
			break;
		case 2:
			printf("Enter the file numer you would like to remove.");
			printf("Files on this disk:\n");		//listing the file names that are valid
			for (i = 0; i < block[0].Super.Inodes; i++) {
				if (fnt[i].valid == 1) {
					printf("%d). %s\n", i, fnt[i].fileName);
				}
			}
			scanf("%d", &filenum);
			char empty[256] = "";
			strcpy(fnt[filenum].fileName, empty);
			break;
		case 3:
			printf("Files on this disk:\n");		//listing the file names that are valid
			for (i = 0; i < block[0].Super.Inodes; i++) {
				if (fnt[i].valid == 1) {
					printf("%d). %s\n", i, fnt[i].fileName);
				}
			}
			printf("Enter the file number you would change the name of: ");
			scanf("%d", &filenum);
			printf("Enter the new name for the file:");
			char temp[256];
			scanf("%s", temp);
			strcpy(fnt[filenum].fileName, temp);
			break;
		case 4:
			printf("Enter the file name in this directory you would like to read from: ");
			char filename1[256];
			scanf("%s", filename1);

			FILE* fp = fopen(filename1, "rb");//fnt[filenum].fileName instead of readme.txt
			if (stat(filename1, &fileStat)){
				printf("Bad call\n");
				fclose(fp);
			}
			else {
				int result;
				i = 0;
				boolean = 1;
				while(i < block[0].Super.Inodes && (boolean==1)) {		//reading in data
					if (fnt[i].valid == 0) {
						if (i >= 1 && i<= block[0].Super.InodeBlocks) {	//superblock comes first so inodes come after
							for (j = 0; j < INODES_PER_BLOCK; j++) {
								if (block[i].Inodes[j].valid == 0) {	//if it doesn't have file contents stored already
									fnt[i-1].valid = 1;
									strcpy(fnt[i-1].fileName, filename1);
									block[i].Inodes[j].valid = 1;			//change it to in use
									block[i].Inodes[j].Fsize = fileStat.st_size;	//4 bytes, size of file
									block[i].Inodes[j].timeDate = fileStat.st_mtime;	//in seconds
									block[i].Inodes[j].Inum = j;
									block[i].Inodes[j].user = fileStat.st_rdev;
									//block[i].Inodes[j].Direct[];		//8*5= 50bytes
									//Indirect;	// 8bytesfileStat.st_size;
									//block[i].Inodes[j].Direct[0];
									if (i + 11 <= block[0].Super.Blocks) {
										result = fread(block[i + 11].Data, 1, block[i].Inodes[j].Fsize, fp);
									}
									boolean = 0;
									break;
								}
							}
						}
					}
					i++;
				}
				

			}
				//printf("File size : %ld\n", fileStat.st_size);
			fclose(fp);
			break;
		case 5:

			printf("Files on this disk:\n");		//listing the file names that are valid
			for (i = 0; i < block[0].Super.Inodes; i++) {
				if (fnt[i].valid == 1) {
					printf("%d). %s\n", i, fnt[i].fileName);
				}
			}
			printf("Enter the file number you would like to read from: ");
			int filenum;
			char externalfile[256];
			scanf("%d", &filenum);
			printf("Enter the name of the external file you would like to write to: ");
			scanf("%s", externalfile);
			FILE* fp1 = fopen(externalfile, "wb");
			if (access(externalfile, F_OK) == 0) {
					printf("\nfsize %d  contents: %s", block[filenum + 1].Inodes[0].Fsize, block[12].Data);
				

				size_t r1 = fwrite(block[0+12].Data, 1,block[filenum + 1].Inodes[0].Fsize, fp1);
				fclose(fp1);
			}
			else {
				printf("Failed to access file: %s", externalfile);
				fclose(fp1);
			}
			break;
		case 6://user name
			if (block[1].Inodes[0].valid == 0) {
				printf("There are no users currently, please enter a file to establish a user.");
			}
			else {
				printf("The current user is %d", block[1].Inodes[0].user);
			}
			break;
		case 7:
			break;
		case 8: 
			break;
		case 9:
			break;
		case 10:
			break;
		default: printf("Please enter a valid number (1-7)\n");
			break;
		}
	}
	/*for (i = 0; i < block[0].Super.Blocks; i++) {
		printf("\nBlocks num: %d\nInodes Num: %d\nBlocks for Inodes num: %d\n\n", block[i].Super.Blocks,block[i].Super.Inodes, block[i].Super.InodeBlocks);
	}*/
	//int inodeBitMap[blocks];
	//int dataBitMap[blocks];

  return 0;
}

