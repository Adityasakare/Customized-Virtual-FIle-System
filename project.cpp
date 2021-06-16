/*
    Project Name : Customised Virtual File subsystem.
*/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define MAX_INODE 100
#define MAX_FILE_SIZE 1024
#define REGULAR 1
#define TRUE 1
#define FALSE 0
#define READ 4
#define WRITE 2
#define SET 1
#define CUR 2
#define END 3
typedef int BOOL;

typedef struct inode
{
    char *filename;
    int Inode_No;
    int FileSize;
    int ActualFileSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission;
    struct inode * next;
}INODE,*PINODE,**PPINODE;

struct SuperBlock
{
    int No_of_Nodes;
    int No_of_free_Nodes;
}SuperObj;

typedef struct FileTable
{
    int ReadOfset;
    int WriteOfset;
    int Mode;
    int count;
    PINODE inodeptr;
}File,*PFile;

struct ufdt
{
    PFile ufdt[MAX_INODE];
}ufdtobj;

struct OpenFiles
{
    char filename[MAX_INODE][80];
    int fd[MAX_INODE];
}openfilesobj;

PINODE Head = NULL;

void InitOpenFiles()
{
    int i = 0;
    for(i=0;i<MAX_INODE;i++)
    {
        openfilesobj.fd[i] = 0;
    }
}
void CreateUFDT()
{
    int i = 0;

    while(i<MAX_INODE)
    {
        ufdtobj.ufdt[i] = NULL;
        i++;
    }
}
void CreateSuperBlock()
{
    SuperObj.No_of_Nodes = MAX_INODE;
    SuperObj.No_of_free_Nodes = MAX_INODE;
}
void CreateDILB()
{
    int i = 0;
    PINODE newnode = NULL;
    PINODE Temp = Head;
    while(i < MAX_INODE)
    {
        newnode = (PINODE)malloc(sizeof(INODE));
        newnode->Inode_No = i;
        newnode->FileSize = MAX_FILE_SIZE;
        newnode->ActualFileSize = 0;
        newnode->FileType = 0;
        newnode->LinkCount = 0;
        newnode->ReferenceCount = 0;
        newnode->Permission = 0;
        newnode->Buffer = NULL;
        newnode->next = NULL;

        if(Head==NULL)
        {
            Head  = newnode;
            Temp = Head;
        }
        else
        {
            Temp->next = newnode;
            Temp=Temp->next;
        }
        i++;
    }
}
void DisplayHelp()
{
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n\n");
    printf("create : To create new files\n");
    printf("ls : To List out all files\n");
    printf("clear : To clear console\n");
    printf("open : To open existing file");
    printf("close : To close already opened file\n");
    printf("closeall : To close all opened files\n");
    printf("read : To read opened file\n");
    printf("write : To write data into opened file\n");
    printf("exit : To exit the project\n");
    printf("stat : To display all the information of the file using filename\n");
    printf("fstat : To display all the information of the file using file descriptor\n");
    printf("truncate : To truncate the file to perticular size\n");
    printf("rm : To remove the file\n");
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
}
void DisplayMan(char *name)
{
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n\n");

    if(name == NULL) 
    {
        return;
    }
    if(strcmp(name,"create") == 0)
    {
        printf("Description : Used to create new regular file\n");
        printf("Usage : create File_name Permission\n");
    }
    else if(strcmp(name,"read") == 0)
    {
        printf("Description : Used to read opened regular file\n");
        printf("Usage : read File_name\n");
    }
    else if(strcmp(name,"write") == 0)
    {
        printf("Description : Used to write data into opened regular file\n");
        printf("Usage : write File_name\n");
        printf("        Write -a File_name to append\n");   
    }
    else if(strcmp(name,"ls") == 0)
    {
        printf("Description : Used to list all the files\n");
        printf("Usage : ls\n");
        printf("        ls -i for list with inodes\n");
        printf("        ls -i for list of open files\n");
    }
    else if(strcmp(name,"stat") == 0)
    {
        printf("Description : Used to display information of file using filename\n");
        printf("Usage : stat File_name\n");
    }
    else if(strcmp(name,"fstat") == 0)
    {
        printf("Description : Used to display information of file using file descriptor\n");
        printf("Usage : fstat File_descriptor\n");
    }
    else if(strcmp(name,"truncate") == 0)
    {
        printf("Description : Used to truncate the file to the perticulat number of bytes\n");
        printf("Usage : truncate No_of_bytes\n");
    }
    else if(strcmp(name,"open") == 0)
    {
        printf("Description : Used to open existing file\n");
        printf("Usage : open File_name Mode\n");
    }
    else if(strcmp(name,"close") == 0)
    {
        printf("Description : Used to close already opened file\n");
        printf("Usage : close File_name\n");
    }
    else if(strcmp(name,"closeall") == 0)
    {
        printf("Description : Used to close all opened file\n");
        printf("Usage : closeall\n");
    }
    else if(strcmp(name,"lseek") == 0)
    {
        printf("Description : Used to change ofset of file\n");
        printf("Usage : lseek No_of_bytes Position File_name\n");
    }
    else if(strcmp(name,"rm") == 0)
    {
        printf("Description : Used to remove the file\n");
        printf("Usage : rm file_name\n");
    }
    else
    {
        printf("ERROR : man page not available.\n");
    }

    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
}
int ChkUFDT()
{
    int i = 0;
    for(i=0;i<MAX_INODE;i++)
    {
        if(ufdtobj.ufdt[i]==NULL)
        {
            break;
        }
    }
    if(i==MAX_INODE)
    {
        return -1;
    }
    else
    {
        return i;
    }
}
int ChkInode()
{
    PINODE Temp = Head;
    int i = 0;
    while(Temp != NULL)
    {
        if(Temp->FileType==0)
        {
            break;
        }
        Temp = Temp->next;
        i++;
    }
    if(Temp==NULL)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

BOOL ChkFile(char *filename)
{
    PINODE Temp = Head;
    while(Temp!=NULL)
    {
        if(Temp->FileType!=0)
        {
            if(strcmp(Temp->filename,filename)==0)
            {
                break;
            }
        }
        Temp = Temp->next;
    }
    if(Temp==NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

int CreateFile(char *filename,int Permission)
{
    
    int FreeInode = 0;
    int FreeUFDT = 0;
    
    if((filename==NULL) || ((Permission != READ) && (Permission != WRITE) && (Permission != (READ+WRITE))))
    {
        return -1;
    }
    if(SuperObj.No_of_free_Nodes==0)
    {
        return -2;
    }
    
    FreeInode = ChkInode();

    FreeUFDT = ChkUFDT();
    if(FreeUFDT==-1)
    {
        return -4;
    }
    BOOL bRet = ChkFile(filename);
    if(bRet==FALSE)
    {
        return -3;
    }           

    ufdtobj.ufdt[FreeUFDT] = (PFile)malloc(sizeof(File));
    ufdtobj.ufdt[FreeUFDT]->ReadOfset = 0;
    ufdtobj.ufdt[FreeUFDT]->WriteOfset = 0;
    ufdtobj.ufdt[FreeUFDT]->count = 1;
    ufdtobj.ufdt[FreeUFDT]->Mode = Permission;
    
    PINODE Temp = Head;
    while(Temp != NULL)
    {
        if(Temp->FileType==0)
        {
            break;
        }
        Temp = Temp->next;
    }
    ufdtobj.ufdt[FreeUFDT]->inodeptr = Temp;

    ufdtobj.ufdt[FreeInode]->inodeptr->filename = (char *)malloc(80);
    strcpy(ufdtobj.ufdt[FreeInode]->inodeptr->filename,filename);
    ufdtobj.ufdt[FreeInode]->inodeptr->FileSize = MAX_FILE_SIZE;
    ufdtobj.ufdt[FreeInode]->inodeptr->ActualFileSize = 0;
    ufdtobj.ufdt[FreeInode]->inodeptr->FileType = REGULAR;
    ufdtobj.ufdt[FreeInode]->inodeptr->LinkCount = 1;
    ufdtobj.ufdt[FreeInode]->inodeptr->ReferenceCount = 0;
    ufdtobj.ufdt[FreeInode]->inodeptr->Buffer = (char *)malloc(1024);
    ufdtobj.ufdt[FreeInode]->inodeptr->Permission = Permission;

    SuperObj.No_of_free_Nodes -= 1;

    return FreeUFDT;
}
