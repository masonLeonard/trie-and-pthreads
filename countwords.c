/* Mason Leonard 818030805
 * CS 480
 * Professor Ben Shen
 * Assignment 2: Dictionary Tree, Main and Testing
 * 2/22/2022
 *
 * PART 2 of processing the inputted data:
 * Given a text file containing a string of sentences ie. an eBook.
 * Must parse and search/compare text source with PART 1's dictionary.
 * Ultimately printing the word being searched and the number of words
 * that start with that prefix to a named file.
 */
#include "dicttree.h"

void *countWordsStartRoutine(void *arg)
{
    struct EXEC_STATUS *tinfo = arg;
    while(!tinfo->taskCompleted[0])
    {
        //busy wait
        //other case of busy wait found in main.c
    }
////////////initializations//////////////////////////////
    //The strtok function iterates across a buffer, pulling out groups of
    //    characters separated by a list of characters called delimiters
    const char *delimiters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";
    char line[1024]; 
    char *word;
    FILE *fp = fopen(tinfo->filePath[1], "r");
    FILE *cwOut = fopen("countwords_output.txt", "w");
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        *tinfo->numOfCharsProcessedFromFile[1] += strlen(line); //our monitor for progress!
        word = strtok(line, delimiters);
        while(word != NULL)
        {
            if(prefixCount(findEndingNodeOfAStr(tinfo->dictRootNode, word)) >=
                                                tinfo->minNumOfWordsWithAPrefixForPrinting)
            {
                fprintf(cwOut, "%s ", word);
                fprintf(cwOut, "%d\n", prefixCount(findEndingNodeOfAStr(tinfo->dictRootNode, word)));
            }
            tinfo->wordCountInFile[1]++;
            word = strtok(NULL, delimiters);
        } 
    }
    tinfo->taskCompleted[1] = true;
    fclose(fp);
    return NULL;
}
