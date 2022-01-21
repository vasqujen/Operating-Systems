//  Jennifer Vasquez
//  Compile Command: gcc JVasquez_prj2_sec13_src.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // library utilizing threads
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>

#define BUFFER1_SIZE 20 //buffer size between threads 1 and 2
#define BUFFER2_SIZE 10 //buffer size between threads 2 and 3

char* buffer1[BUFFER1_SIZE];
char* buffer2[BUFFER2_SIZE];
int lines = 0;


//in and out for buffer 1 and 2
int in1=0;
int in2=0;
int out1=0;
int out2=0;
int thread1_done = 0;
int thread2_done = 0;

void *printMessage( void *ptr );

char* readLine(int file){
    
    int maxLength = 128;
    //malloc function allocates memory that has the size to hold 128 character
    char *lineBuffer = (char *)malloc(sizeof(char) * maxLength);
    char ch;
    
    int ret = read(file, &ch, 1);
    
    int count = 0;
    
    if ( ret != 0 ) {
        
        while (ch != '\n') {
            
            if (count == maxLength) { //if it reaches 128 character on a line you make a bigger buffer and reallocate mem
                maxLength += 128;
                lineBuffer = realloc(lineBuffer, maxLength);
            }
            lineBuffer[count] = ch;
            count++;
            
            int ret = read(file, &ch, 1);
            if (ret == 0) {
                break;
            }
        }
        
    }
    
    lineBuffer[count] = '\0';
    
    return lineBuffer;
}


//PRODUCER THREAD 1
void *fthread1(void *ptr )
{
    char* line = NULL;
    while (1) {
        
        while (((in1 + 1) % BUFFER1_SIZE) == out1){
            //allows a thread to give up control of a processor so that another thread can have the opportunity to run
            sched_yield();
        }
        char * line = readLine( *((int*)ptr) );
        if (*line=='\0') {
            thread1_done=1;
            return NULL;
        }
        
        buffer1[in1] = line;
        in1 = (in1 + 1) % BUFFER1_SIZE;
        
    }
    
}


//Traverse string from start, increment index until '\0'
int len(char *str) {
    int counter =0;
    while(*str !='\0'){
        counter ++;
        str++;
    }
    return counter;
}

//PRODUCER & CONSUMER THREAD
void *fthread2(void *ptr )
{
    char* line;
    while (1) {
        while (in1 == out1) {
            if(thread1_done) {
                thread2_done = 1;
                break;
            }
            sched_yield();
        }
        if(thread2_done) {
            break;
        }
        line = buffer1[out1];
        
        out1 = (out1 + 1) % BUFFER1_SIZE;
        
        lines++;
        
        // parse the line and store in buffer2
        
        while (1) {
            int i = 0; //position in word_buffer
            char* word_buffer= malloc(sizeof(char) * (i+1));
            // consume white spaces
            while (*line != '\0' && ((*line == ' ' || *line == '.' || *line == ','))) {
                line++;
            }
            while (*line != '\0' && !((*line == ' ' || *line == '.' || *line == ','))) {
                word_buffer[i] = *line;
                i++;
                line++;
            }
            word_buffer[i] = '\0';
            
            
            // send word to thread 3
            if (*word_buffer != '\0') {
                while (((in2 + 1) % BUFFER2_SIZE) == out2){
                    //allows a thread to give up control of a processor so that another thread can have the opportunity to run
                    sched_yield();
                }
                
                buffer2[in2] = word_buffer;
                in2 = (in2 + 1) % BUFFER2_SIZE;
            }
            if (*line == '\0') {
                break;
            }
        }
    }
    pthread_exit(NULL);
}


//thread 3
void *fthread3(void *ptr ){
    int counter = 0;
    int thread3_done = 0;
    while (1) {
        while (in2 == out2) {
            if(thread2_done){
                thread3_done = 1;
                break;
            }
            sched_yield();
        }
        if(thread3_done){
            break;
        }
        char* word = buffer2[out2];
        out2 = (out2 + 1) % BUFFER2_SIZE;
        counter++;
        write(STDOUT_FILENO,word,len(word));
        write(STDOUT_FILENO,"\n",1);
    }
    printf("WORD COUNT: %d\n", counter);
    printf("LINE COUNT: %d", lines);
    
    pthread_exit(NULL);
}


int main()
{
    int fd = open("proj2Input.txt", O_RDONLY); // Open file
    
    pthread_t thread1, thread2, thread3;
    int  iret1, iret2, iret3;
    
    
    iret1 = pthread_create( &thread1, NULL, &fthread1, (void*) &fd);
    iret2 = pthread_create( &thread2, NULL, &fthread2, (void*) NULL);
    iret3 = pthread_create( &thread3, NULL, &fthread3, (void*) NULL);
    
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);
    
    
    exit(0);
    
}
