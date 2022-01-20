// Jeniffer Vasquez
// Compile Command: gcc Jvasquez_prj3_sec13_src.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define SIZE 20

pthread_mutex_t mutex;

//initialized globak shared variables
int top;
int pushSum;
int popSum;
int popOrder[120];

//global shared array stack
int stack[SIZE];

//check if stack is empty or full
int is_full() {
    return top >= SIZE;
}
int is_empty(){
    return top == 0;
}

//Push integers into stack for Thread 1 until stack is full
void push(int i){
    while (1){
        pthread_mutex_lock(&mutex);
        if (is_full()) {
            // if the stack is full, release lock calls yield
            pthread_mutex_unlock(&mutex);
            sched_yield();
        }
        else
        {
            stack[top] = i;
            top++;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
}
//Pop function pops elemnts from the stack
int pop(){
    while (1){
        pthread_mutex_lock(&mutex);
        if (is_empty()) {
            //release lock and yield if empty
            pthread_mutex_unlock(&mutex);
            sched_yield();
        }
        else {
            // pop from stack if not empty
            int result = stack[top-1];
            // update top of the stack
            top--;
            pthread_mutex_unlock(&mutex);
            return result;
        }
    }
}

//Thread1 pushes values into stack & adds sum of digits pushed
void* fthread1(void* arg){
    for(int i = 1; i < 121; i++){
        if (is_full()) {
            sched_yield();
        }
        push(i);
        pushSum += i;
        if ( i % 10 == 0) {
            // yield every 10 pushes
            sched_yield();
        }
    }
    return NULL;
}

//Thread2 pops values from stack & adds sum of values popped
void* fthread2(void* arg){
    int index = 0;
    //pops all 120 elements
    while ( index < 120){
        //if stack is full, we pop all elements
        if (is_full()){
            while (!is_empty()){
                int x = pop();
                popSum = popSum + x;
                popOrder[index] = x;
                index++;
            }
        }
        else{
            int topValue = top;
            if (topValue >= 5) {
                // if the stack is not full and has more than 5 elements
                // pop 5 elements and yield
                for (int i = 0; i<5; i++) {
                    int x = pop();
                    popOrder[index] = x;
                    popSum += x;
                    index++;
                }
            }
            
            else
            {
                // if the stack is not full and has less than 5 elements
                // pop all elements and yield
                for (int i = 0; i < topValue; i++) {
                    int x = pop();
                    popOrder[index] = x;
                    popSum += x;
                    index++;
                }
            }
        }
        sched_yield();
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    
    pthread_t thread1, thread2;
    int iret1, iret2;
    
    pthread_mutex_init(&mutex, NULL);
    
    //Creates threads that will do a specific task
    iret1 = pthread_create( &thread1, NULL, &fthread1, (void*) NULL);
    iret2 = pthread_create( &thread2, NULL, &fthread2, (void*) NULL);
    
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    
    // open file & give writing permission
    FILE *file = fopen(argv[1], "w");
    // write the popOrder
    fprintf(file, "All 120 elements of Pthread 2:");
    for (int i = 0; i < 120; i++) {
        fprintf(file, "%d ", popOrder[i]);
    }
    fprintf(file, "\n");
    // write pushSum
    fprintf(file, "Value of Pthread 1 %d\n", pushSum);
    //write popSUm
    fprintf(file, "Value of Pthread 2 %d\n", popSum);
    // close file
    fclose(file);
    
    return 0;
}
