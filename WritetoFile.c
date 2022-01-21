//  Jennifer Vasquez
//  Compile Command: gcc JVasquez_prj1_sec13_src.c
//  ./a.out sourceFile destinationFile

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZ 128

int readfile( const char *,int);
int writefile( const char *,int);


/* Reading done by child process */
int readfile( const char *inputfile, int fd)
{     /* Here, child process reads from downstream of pipe and writes the content into destination output file */
    int f2;
    char buff[SIZ];
    
    read(fd, buff, SIZ); /* reading from downstream (fd) and storing the read content in an array buffer. */
    if((f2 = open(inputfile, O_WRONLY|O_CREAT , 0666)) == -1)
    {    /*opening the destination file to write the content read from downstream of pipe */
        printf("Can't open file: %s\n", inputfile);
        return(-1);
    }
    write(f2,buff,SIZ); /* writing into output file */
    close(f2);   /*closing the opened destination file */
    close(fd);
    return 1;
}

/* Writing done by parent process */
int writefile(const char *outputfile, int fd)
{ /* Parent process reads content from input file and writes that into the upstream of pipe */
    int f1;
    char buff2[SIZ];
    
    /* Can we open the input/source file? */
    if((f1 = open(outputfile, O_RDONLY)) == -1)
    {
        printf("Error opening file: %s\n", outputfile);
        return(-1);
    }
    read(f1, buff2, SIZ); /* reads content is stored in the array buff2. */
    write(fd,buff2,SIZ); /* writing the content in buff2 to the upstream of pipe */
    close(f1); /* closing the source file */
    close(fd);
    return 1;
    
}
int main(int argc, const char *argv[])
{
    
    int fd[2], pid;
    
    /* Do we have the right # of arguments? */
    if( argc !=3 )
    {
        printf("Wrong number of command line arguments\n");
        return 1;
    }
    /*Creates the pipe */
    pipe(fd);
    if (pipe(fd) == -1){
        fprintf(stderr, "Pipe failed\n");
        return 1;
    }
    /*Forks a child process*/
    pid = fork();
    
    if(pid < 0) { /* error occurred */
        fprintf(stderr, "Fork failed\n");
        return 1;
    }
    if(pid > 0){ /* parent process */
        close(fd[0]);   /* closing the read end of pipe */
        writefile(argv[1],fd[1]);    /* invoking the writefile */
    }
    else { /* child process */
        close(fd[1]);   /* closing write end of pipe */
        readfile(argv[2],fd[0]);    /* invoking the readfile */
        
    }
    return 0;
    
}
