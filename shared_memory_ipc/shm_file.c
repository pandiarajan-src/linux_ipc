#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>


#if 0
void * mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);

int munmap(void *addr, size_t length);
#endif

typedef struct student_
{
    int roll_no;
    int marks;
    char name[128];
    char city[128];
} student_t;

int main(int argc, char* argv[1])
{
    if(argc < 2)
    {
        printf("Please enter a file name as command line arg : ../shm_file <filename> \n");
        return 0;
    }
    const char* file_path = argv[1];

    int fd = open(file_path, O_RDWR);
    if(fd < 0)
    {
        printf("Couldn't open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    /* Extract the size of the file */
    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err < 0)
    {
        printf("Couldn't open file %s\n", file_path);
        exit(EXIT_FAILURE);    
    }

    char *ptr = mmap(NULL, /*Let Kernel decide the starting address of mapping in Virtual Memory */
                                  statbuf.st_size,  /* Memory size to Map */
                                  PROT_READ | PROT_WRITE,  /* Map the memory in Read-Write mode, meaning the Virtual Memory can be read 
                                                                and written by the process. Note that, this permission must be compatible
                                                                with permissions with which the file is open using open () */
                                MAP_SHARED,  /* To see the changes in the file, should be always MAP_SHARED.   */
                                fd,                         /* FD represents the identifier of the external data source, in this case it is a file */
                                0);                         /* Offset into the text file from which the mapping has to begin, pass zero to map from the 
                                                                    beginning of the file */ 
    if(ptr == MAP_FAILED){
        printf("Mapping Failed, errorno = %d\n", errno);
        return 1;
    }
    
     close(fd);      /* We will not going to read/write into file using file-handling routines, hence close the fd. mmap can still
                                use this file even if its fd is closed */

    student_t stud = {1234, 100, "Pandiarajan", "Chennai"};
    memset(ptr, 0, sizeof(stud));
    memcpy (ptr, &stud, sizeof(stud));

    msync(ptr, sizeof(stud), MS_SYNC);

    err = munmap(ptr, statbuf.st_size);     /* Destroy the mapping once done */
   
    if(err != 0){
        printf("UnMapping Failed\n");
        return 1;
    }
    
    return 0;

}

