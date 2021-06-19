#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
	int TotalInodes;
	int FreeInode;
} SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode 
{
	char FileName[50];
	int INodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenceCount;
	int permission;
	struct inode *next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
} FILETABLE, *PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void man (char *name)
{
	if(name == NULL) return;
	if (strcmp(name, "create") == 0)
	{
		printf("Description: Used to create new regular file\n");
		printf("Usage: create File_name Permission\n");
	}
	else if (strcmp(name, "read") == 0)
	{
		printf("Description: Used to read data from regular file\n");
		printf("Usage: read file_name No_Of_Bytes_To_Read \n");
	}
	else if (strcmp(name, "write") == 0)
	{
		printf("Description Used to write data into regular file\n");
		printf("Usage: write File_name\n After this enter the data that we want to write\n");
	}
	else if (strcmp(name, "ls") == 0)
	{
		printf("Description: Used to list all information of files \n");
		printf("Usage: ls\n");
	}
	else if (strcmp(name, "stat") == 0)
	{
		printf("Description: Used to display information of file \n");
		printf("Usage: stat File_name\n");
	}
	else if (strcmp(name, "fstat") == 0)
	{
		printf("Description: Used to display information of file \n");
		printf("Usage: stat File_Discriptor\n");
	}
	else if (strcmp(name, "truncate") == 0)
	{
		printf("Description: Used to remove the data from file \n");
		printf("Usage: truncate File_name \n");
	}
	else if (strcmp(name, "open") == 0)
	{
		printf("Description: Used to open existing file \n");
		printf("Usage: open File_name mode");
	}
	else if (strcmp(name, "close") == 0)
	{
		printf("Description: Used to close opend file\n");
		printf("Usage: Close File_name\n");
	}
	else if (strcmp(name, "closeall") == 0)
	{
		printf("Description: Ised to close all opened file\n");
		printf("Usage: closeall\n");
	}
	else if (strcmp(name, "lseek") == 0)
	{
		printf("Description: Used to change file offset\n");
		printf("Usage: lseek File_name ChnageInOffset StartPoint \n");		
	}
	else if (strcmp(name, "rm") == 0)
	{
		printf("Description: Used to delete the file \n");
		printf("Usage: rm File_Name\n");
	}
	else
	{
		printf("ERROR: No mannual entry available \n");
	}
}

void DisplayHelp()
{
	printf("ls: To list out all files \n");
	printf("clear: To clear console \n");
	printf("open: To open the file \n");
	printf("close : To close the file \n");
	printf("closeall: To close all opend file \n");
	printf("read: To read the contents from file \n");
	printf("write: To write contents into the file \n");
	printf("exit: To terminate the file system \n");
	printf("stat: To display information of file using name\n");
	printf("fsat: To display the infomation of file using file descriptor \n");
	printf("truncate : To remove all data from file \n");
	printf("rm: To delete the file \n");
}

int GetFDFromName(char *name)
{
	int i = 0;

	while (i < 50)
	{
		if (UFDTArr[i].ptrfiletable != NULL)
		{
			if (strcmp( (UFDTArr[i].ptrfiletable -> ptrinode -> FileName), name) == 0)
			{
				break;
			}
		}
		i++;
	}

	if (i == 50)
	{
		return -1;
	}
	else
	{
		return i;
	}
}

PINODE Get_Inode (char *name)
{
	PINODE temp = head;
	int i = 0;

	if (name == NULL)
	{
		return NULL;
	}
	while (temp != NULL)
	{
		if (strcmp (name, temp -> FileName) == 0)
		{
			break;
		}
		temp = temp -> next;
	}
	return temp;
}

void CreateDILB()
{
	int i = 1;
	PINODE newn = NULL;
	PINODE temp = head;

	while (i <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));

		newn -> LinkCount = newn -> ReferenceCount = 0;
		newn -> FileType = newn -> FileSize = 0;
		newn -> Buffer = NULL;
		newn -> next = NULL;
		newn -> INodeNumber = i;
		if (temp == NULL)
		{
			head = newn;
			temp = head;
		}
		else
		{
			temp -> next = newn;
			temp = temp -> next; 
		}
		i++;
	}
	printf("DILB created Successfully \n");
}

void InitialiseSuperBlock()
{
	int i = 0;
	while (i < 50)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInodes = MAXINODE;
	SUPERBLOCKobj.FreeInode = MAXINODE;
}

int CreateFile (char *name, int permission)
{
	int i = 0;
	PINODE temp = head;

	if ( (name == NULL) || (permission == 0) || (permission > 3))
	{
		return -1;
	}

	if (SUPERBLOCKobj.FreeInode == 0)
	{
		return -2;
	}
	if (Get_Inode(name) != NULL)
	{
		return -3;
	}

	(SUPERBLOCKobj.FreeInode)--;
	while (temp != NULL)
	{
		if(temp -> FileType == 0)
			break;
		temp = temp -> next;
	}

	while (i < 50)
	{
		if (UFDTArr[i].ptrfiletable == NULL)
			break;
		i++;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if (UFDTArr[i].ptrfiletable == NULL)
	{
		return -4;
	}
	UFDTArr[i].ptrfiletable -> count = 1;
	UFDTArr[i].ptrfiletable -> mode = permission;
	UFDTArr[i].ptrfiletable -> readoffset = 0;
	UFDTArr[i].ptrfiletable -> writeoffset = 0;

	UFDTArr[i].ptrfiletable -> ptrinode = temp;
	strcpy(UFDTArr[i].ptrfiletable -> ptrinode-> FileName,name);
	UFDTArr[i].ptrfiletable -> ptrinode -> FileType = REGULAR;
	UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount = 1;
	UFDTArr[i].ptrfiletable -> ptrinode -> LinkCount = 1;
	UFDTArr[i].ptrfiletable -> ptrinode -> FileSize = MAXFILESIZE;
	UFDTArr[i].ptrfiletable -> ptrinode -> FileActualSize = 0;
	UFDTArr[i].ptrfiletable -> ptrinode -> permission = permission;
	UFDTArr[i].ptrfiletable -> ptrinode -> Buffer = (char *)malloc(sizeof(MAXFILESIZE));
	memset(UFDTArr[i].ptrfiletable -> ptrinode ->Buffer, 0, 1024);

	return i;
}

int rm_File(char *name)
{
	int fd = 0;

	fd = GetFDFromName(name);
	if (fd == -1)
	{
		return -1;
	}

	(UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount)--;

	if (UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable -> ptrinode -> FileType = 0;
		free(UFDTArr[fd].ptrfiletable);
	}

	UFDTArr[fd].ptrfiletable -= NULL;
	(SUPERBLOCKobj.FreeInode)++;
}

int ReadFile (int fd, char *arr, int isize)
{
	int read_size = 0;

	if (UFDTArr[fd].ptrfiletable == NULL)
	{
		return -1;
	}

	if (UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable -> mode != READ+WRITE)
	{
		return -2;
	}	

	if (UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ && UFDTArr[fd].ptrfiletable -> permission != READ+WRITE)
	{
		return -2;
	}

	if (UFDTArr[fd].ptrfiletable -> readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)
	{
		return -3;
	}	

	if (UFDTArr[fd].ptrfiletable -> ptrinode -> FileType != REGULAR)
	{
		return -4;
	}

	read_size = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) - (UFDTArr[fd].ptrfiletable -> readoffset);
	if (read_size < isize)
	{
		strcpy(arr, (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), read_size);
		UFDTArr[fd].ptrfiletable -> readoffset = UFDTArr[fd].ptrfiletable -> readoffset + read_size;
	}
	else
	{
		strcpy(arr, (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), isize);
		UFDTArr[fd].ptrfiletable -> readoffset = UFDTArr[fd].ptrfiletable -> readoffset + isize;	
	}

	return isize;
}

int WriteFile(int fd, char *arr, int isize)
{
	if (UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable -> mode != READ+WRITE)
	{
		return -1;
	}	

	if (UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ && UFDTArr[fd].ptrfiletable -> permission != READ+WRITE)
	{
		return -1;
	}	

	if ((UFDTArr[fd].ptrfiletable -> writeoffset) == MAXFILESIZE)
	{
		return -2;
	}
	if ( (UFDTArr[fd].ptrfiletable -> ptrinode -> FileType) != REGULAR)
	{
		return -3;
	}

	strcpy((UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> writeoffset), arr, isize);

	(UFDTArr[fd].ptrfiletable -> ptrinode -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + isize;

	(UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + isize;

	return isize;
}

int OpenFile (char *name, int mode)
{
	
}

int main(int argc, char const *argv[])
{
	
	return 0;
}