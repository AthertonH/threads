#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h> // for errno
#include <string.h>
#include <ctype.h>

const unsigned long MIN_THREADS = 1;
const unsigned long MAX_THREADS = 24;
const unsigned long MIN_LIMIT = 100;
const unsigned long BLOCK = 5000;
unsigned long currCheck = 1;
unsigned long userLimit = 0;
unsigned long evilNumberCount = 0;
unsigned long odiousNumberCount = 0;

pthread_mutex_t m;

bool odiousNumberCheck(unsigned long);


// Reads and verifies command line arguments. Includes error and range
// checking based on provided parameters. The error strings are defined
// in the example output. Due to the potential size of the limit,
// unsigned long data type should be used. The function option must be
// 1 or 2.
bool getArguments(int argc, char* argv[], int* tc, unsigned long* lv, int* fo)
{
    // Usage
    if(argc == 1)
        {printf("Usage: ./evilNums -t <threadCount> -l <limitValue>\n"); return false;}
    
    // Command line options
    if(argc != 7)
        {printf("Error, invalid command line options.\n"); return false;}
    
    // -t
    if(strcmp(argv[1], "-t") != 0)
        {printf("Error, invalid thread count specifier.\n"); return false;}
    
    // Thread amount
    for(int i = 0; argv[2][i] != '\0'; i++)
    {
        if(!isdigit(argv[2][i]))
            {printf("Error, invalid thread count value.\n"); return false;}  
    }
    // Range of threads
    if(atoi(argv[2]) > MAX_THREADS || atoi(argv[2]) < MIN_THREADS)
        {printf("Error, thread count out of range.\n"); return false;}
    
    // -l
    if(strcmp(argv[3], "-l") != 0)
        {printf("Error, invalid limit specifier.\n"); return false;}
        
    // Limit is digit
    for(int i = 0; argv[4][i] != '\0'; i++)
    {
        if(!isdigit(argv[4][i]))
            {printf("Error, invalid limit value.\n"); return false;}
    }
    // Range of limit
    if(atoi(argv[4]) < MIN_LIMIT)
        {printf("Error, limit must be > 100.\n"); return false;}

    // -f
    if(strcmp(argv[5], "-f") != 0)
        {printf("Error, invalid function specifier.\n"); return false;}

    // Function option 1|2
    if(atoi(argv[6]) != 1 && atoi(argv[6]) != 2)
        {printf("Error, function option must be 1|2.\n"); return false;}

    *tc = atoi(argv[2]);
    *lv = atoi(argv[4]);
    *fo = atoi(argv[6]);
    return true;
}


// Called as a thread function and determine the count of Evil numbers.
// Will be called with 1.
// Thread must perform  updates to the global variables in a critical
// section and avoid race conditions
void* findEvilNumbersCnt1(void* arg)
{
    while(true)
    {
        unsigned long index = 0;

        // Lock the critical section, update the block
        index = currCheck;
        currCheck += BLOCK;

        // Enter remainder code
        if(index > userLimit) break;

        for(unsigned long i = index; i < index + BLOCK && i <= userLimit; i++)
        {
            bool isOdious = odiousNumberCheck(i);

            if(isOdious) 
                odiousNumberCount++;
            else 
                evilNumberCount++;
        }
    }
    return NULL;
}

// Called as a thread function and determine the count of EVil numbers.
// Will be called with 2.
// Thread must perform  updates to the global variables in a critical
// section and avoid race conditions
void* findEvilNumbersCnt2(void* arg)
{
    while(true)
    {
        unsigned long index = 0;
        unsigned long evil = 0;
        unsigned long odious = 0;

        // Lock the critical section, update the block
        index = currCheck;
        currCheck += BLOCK;

        // Enter remainder code
        if(index > userLimit) break;

        // Check if odious up to userLimit
        for(unsigned long i = index; i < index + BLOCK && i <= userLimit; i++)
        {
            bool isOdious = odiousNumberCheck(i);

            // Update local counts
            if(isOdious)
                odious++;
            else
                evil++;
        }

        // Update global counts
        odiousNumberCount += odious;
        evilNumberCount += evil;
    }
    return NULL;
}

bool odiousNumberCheck(unsigned long number)
{
    int count = 0;              // Count of 1s
    while(number > 0)
    {
        count += number & 1;    // Check least significant bit
        number /= 2;            // Right shift
    }
    int evilOrOd = count % 2;   // Determine if evil or odd
    return(evilOrOd);
}

int main(int argc, char *argv[])
{
    // Initialize variables
    int threadCount;
    int functionOption;

    // If we fail getting arguments, return error
    if(!getArguments(argc, argv, &threadCount, &userLimit, &functionOption))
        return 1;

    printf("CS 370 - Project #5-A\n");
    printf("Evil/Odious Numbers Program\n\n");
    printf("Hardware Cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf("Thread Count: %d\n", threadCount);
    printf("Numbers Limit: %ld\n\n", userLimit);
    printf("Please wait. Running...\n\n");

    // Initialize thread
    pthread_mutex_init(&m, NULL);

    // Create array of threads
    pthread_t threads[threadCount];

    // Initialize each thread to work on findEvilNumbersCnt1 function.
    if(functionOption == 1)
        for(int i = 0; i < threadCount; i++)
            pthread_create(&threads[i], NULL, findEvilNumbersCnt1, NULL);
    else if(functionOption == 2)
        for(int i = 0; i < threadCount; i++)
            pthread_create(&threads[i], NULL, findEvilNumbersCnt2, NULL);
    
    // Join the threads to ensure that main doesn't execute until all
    // threads have completed their tasks.
    for(int i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);
    
    // Print results to terminal.
    printf("Evil/Odious Numbers Results\n");
    printf("Evil Number Count: %ld\nOdious Number Count: %ld\n",
    evilNumberCount, odiousNumberCount);
}