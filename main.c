/* Mason Leonard 818030805
 * CS 480
 * Professor Ben Shen
 * Assignment 2: Main and Thread Management
 * 2/22/2022
 *
 * Numerous points of interest
 * The common data structure is created and populated.
 * With the help of POSIX threads, two workers are spawned.
 * The code within main allows these two threads to
 * cooperate and thus work together to compute the results.
 */
#include "dicttree.h"

int main(int argc, char *argv[])
{
/////////////////arguments, creation and initialization//////////////////////////////
    int pFLAG = false, hFLAG = false, nFLAG = false;
    int pMarks, hInterval, nMinimum;
    int s, opt;
    char *strUsage = "file1 file2 [-p N progress marks] [-h N hash interval] [-n N dict entries]";
/////////////////prepare the common data structure I//////////////////////////////
    struct EXEC_STATUS *tinfo; 
    tinfo = calloc(1, sizeof(struct EXEC_STATUS));
    if(tinfo == NULL)
    {
        printf("Problem with calloc\n");
        exit(EXIT_FAILURE);
    }   
    tinfo->filePath[0] = argv[1]; //set the appropriate file name
    tinfo->filePath[1] = argv[2]; //for each inputted file
/////////////////handle args from command line//////////////////////////////
    if(access(tinfo->filePath[0], R_OK) != 0)
    {
        printf("Unable to open <<%s>>\n", tinfo->filePath[0]);
        return -1;
    }
    if(access(tinfo->filePath[1], R_OK) != 0)
    {
        printf("Unable to open <<%s>>\n", tinfo->filePath[1]);
        return -2;
    }

    while((opt = getopt(argc, argv, "p:h:n:")) != -1)
    {
        switch(opt)
        {
            case 'p':
                if(strtol(optarg, NULL, 10) == 0) //set 0 as default value
                {
                     pMarks = 50;    //default value
                     pFLAG = true;   
                     break;
                }
                else if(strtol(optarg, NULL, 10) < 10)
                {
                    printf("improper p value of %ld, p >= 10\n", strtol(optarg, NULL, 10));
                    return 9;
                }
                pMarks = strtol(optarg, NULL, 10);
                pFLAG = true;
                break;
            
            case 'h':
                if(strtol(optarg, NULL, 10) == 0)
                {
                     hInterval = 10;
                     hFLAG = true;
                     break;
                }
                else if(strtol(optarg, NULL, 10) < 0 || strtol(optarg, NULL, 10) > 10)
                { 
                    printf("improper h value of %ld, 0 < h <= 10\n", strtol(optarg, NULL, 10));
                    return 11;
                }
                hInterval = strtol(optarg, NULL, 10);
                hFLAG = true;
                break;
         
            case 'n':
                if(strtol(optarg, NULL, 10) == 0)
                {
                     nMinimum = 1;
                     nFLAG = true;
                     break;
                }
                else if(strtol(optarg, NULL, 10) < 0)
                {
                    printf("improper n value of %ld, n >= 0\n", strtol(optarg, NULL, 10));
                    return 11;
                }
                nMinimum = strtol(optarg, NULL, 10);
                nFLAG = true;
                break;
           
            default:
                printf("Usage: %s %s\n", argv[0], strUsage);
                exit(EXIT_FAILURE);
        }
    }

    //if any of these three variables haven't been touched, set to default value
    if(!pFLAG)
    {
        pMarks = 50;
    }
    if(!hFLAG)
    {
        hInterval = 10;
    }
    if(!nFLAG)
    {
        nMinimum = 1;
    }
/////////////////prepare the common data structure II//////////////////////////////
    tinfo->numOfProgressMarks = pMarks;
    tinfo->hashmarkInterval = hInterval;
    tinfo->minNumOfWordsWithAPrefixForPrinting = nMinimum;
    //create the node for the dictionary tree and handoff purposes
    struct dictNode *root = getNode();
    tinfo->dictRootNode = root;
    //create values for calculating and storing the file sizes of argv[1] and argv[2]
    struct stat buf;
    stat(tinfo->filePath[0], &buf);
    long size = buf.st_size;
    tinfo->totalNumOfCharsInFile[0] = size;      
    stat(tinfo->filePath[1], &buf);
    size = buf.st_size;
    tinfo->totalNumOfCharsInFile[1] = size;      
    //allocate to the heap for our monitor values to do their job
    tinfo->numOfCharsProcessedFromFile[0] = calloc(1, sizeof(long *));
    tinfo->numOfCharsProcessedFromFile[1] = calloc(1, sizeof(long *));
    //set appropirate flags for scheduling
    tinfo->taskCompleted[0] = false;
    tinfo->taskCompleted[1] = false;
/////////////////pthread code//////////////////////////////
    pthread_t populateTreeWorker, countWordsWorker; 
    s = pthread_create(&populateTreeWorker, NULL, &populateTreeStartRoutine, (void *) tinfo);
    if(s != 0)
    {
        printf("tPopulateTree: pthread_create error\n");
        exit(EXIT_FAILURE); 
    }  
/////////////////progress bar code//////////////////////////////
    /* We want main.c to keep pace with each worker thread.
     * Initially, wait for populateTreeWorker, not busy wait
     * need to print the progress as it is happening, real time.
     * Then do the same for countWordsWorker.
     *
     * Using two flags found in the common data structure
     * we can enforce a specific order of operations.
     */
    int i = 1, j = 1;
    double result, answer;
    while(!tinfo->taskCompleted[0])
    {
        result = (double) *tinfo->numOfCharsProcessedFromFile[0] / (double) tinfo->totalNumOfCharsInFile[0];
        if(result >= (i / (double) tinfo->numOfProgressMarks))
        {
            if(i % tinfo->hashmarkInterval == 0) 
            {
                printf("#");
                fflush(stdout);
            }
            else
            {
                printf("-");
                fflush(stdout);
            }
            i++;            //printf("milestone\n");
        }
    }
    printf("\n");
    printf("There are %ld words in %s.\n", tinfo->wordCountInFile[0], tinfo->filePath[0]);
    
    s = pthread_create(&countWordsWorker, NULL, &countWordsStartRoutine, (void *) tinfo);
    if(s != 0)
    {
        printf("tCountWords: pthread_create error\n");
        exit(EXIT_FAILURE);
    }
    
    while(!tinfo->taskCompleted[1])
    {
        answer = (double) *tinfo->numOfCharsProcessedFromFile[1] / (double) tinfo->totalNumOfCharsInFile[1];
        if(answer >= (j / (double) tinfo->numOfProgressMarks))
        {
            if(j % tinfo->hashmarkInterval == 0)
            {
                printf("#");
                fflush(stdout);
            }
            else
            {
                printf("-");
                fflush(stdout);
            }
            j++;            //printf("milestone\n");          
        }         
    } 
    printf("\n");
    printf("There are %ld words in %s.\n", tinfo->wordCountInFile[1], tinfo->filePath[1]);
///////////////clean up/////////////////////////////
    s = pthread_join(populateTreeWorker, NULL);
    if(s != 0)
    {
        printf("tPopulateTree: pthread_join error\n");
        exit(EXIT_FAILURE);
    }
    s = pthread_join(countWordsWorker, NULL);
    if(s != 0)
    {
        printf("tCountWords: pthread_join error\n");
        exit(EXIT_FAILURE);
    }
    free(tinfo->numOfCharsProcessedFromFile[0]);
    free(tinfo->numOfCharsProcessedFromFile[1]);
    free(tinfo);
    return 0; //successful run (or so he thinks)
}