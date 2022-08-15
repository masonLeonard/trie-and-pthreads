/* Mason Leonard 818030805
 * CS 480
 * Professor Ben Shen
 * Assignment 2: Dictionary Tree, Main and Testing
 * 2/22/2022
 *
 * PART 1 of processing the inputted data:
 * Given a text file containing a list of dictionary words
 * Must parse through word list and insert each character
 * of each word into its own node. Thus forming the
 * Dictionary Tree.
 */
#include "dicttree.h"

void *populateTreeStartRoutine(void *arg)
{
    //The strtok function iterates across a buffer, pulling out groups of 
    //    characters separated by a list of characters called delimiters
    const char *delimiters = "\n\r !\"#$%&()*+,-./0123456789:;<=>?@[\\]^_`{|}~";
    char line[1024];
    char *word;
    struct EXEC_STATUS *tinfo = arg;
    FILE *fp = fopen(tinfo->filePath[0], "r");
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        *tinfo->numOfCharsProcessedFromFile[0] += strlen(line); //our monitor for progress!
        word = strtok(line, delimiters);

        if(word == NULL)
        {
            break;
        }
        add(tinfo->dictRootNode, word);
        tinfo->wordCountInFile[0]++;
        word = strtok(NULL, delimiters);
    }
///////////////cleanup/output/////////////////////////////
    tinfo->taskCompleted[0] = true;
    fclose(fp);
    return NULL;
}
