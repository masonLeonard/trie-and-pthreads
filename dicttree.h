/* Mason Leonard 818030805
 * CS 480
 * Professor Ben Shen
 * Assignment 2: Dictionary Tree, Include, Prototype, Documentation 
 * 2/22/2022
 *
 * Bird's eye view of the program.
 */
#include <stdlib.h>  
#include <stdio.h>    //required by: fopen()
#include <string.h>   //required by: strlen(), strtol()
#include <unistd.h>   //required by: access()
#include <stdbool.h> 
#include <ctype.h>    //required by: tolower()
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h> //required by: stat()
#include <fcntl.h>    //required by: open()

#define NUMOfCHARS 27 /* a-z plus ' */

/////////////////Shared constants and data structures among threads//////////////////
#define NUMOFFILES 2
#define DICTSRCFILEINDEX 0
#define TESTFILEINDEX 1

/* default number of progress marks for representing 100% progress */
#define DEFAULT_NUMOF_MARKS 50

/* minimum number of progress marks for representing 100% progress */
#define MIN_NUMOF_MARKS 10

/* place hash marks in the progress bar every N marks */
#define DEFAULT_HASHMARKINTERVAL 10

/* default minimum number of dictionary words starting from a prefix for printing or
   writing to the output */
#define DEFAULT_MINNUM_OFWORDS_WITHAPREFIX 1

#ifndef DICTTREE_H
#define DICTTREE_H

typedef struct dictNode
{
    // isWord is true if the node represents the
    // last character of a word
    bool isWord;
    // Collection of nodes that represent subsequent characters in
    // words. Each node in the next dictNode*
    // array corresponds to a particular character that continues
    // the word if the node at the array index is NOT NULL:
    // e.g., if next[0] is NOT NULL, it means 'a' would be
    // one of the next characters that continues the current word;
    // while next[0] being NULL means there are no further words
    // with the next letter as 'a' along this path.
    struct dictNode *next[NUMOfCHARS];
} dictNode;


//#ifndef DICTTREE_H
//#define DICTTREE_H

/* Common data shared between threads */
typedef struct EXEC_STATUS
{
    /**
    * root node of the dictionary tree
    */
    dictNode *dictRootNode;
    
    
    /**
    * parameters for printing progress bar
    */
    int numOfProgressMarks;
    
    
    int hashmarkInterval;
    
    
    /**
    * print a word and its count to the output file only if the
    * number of dictionary words starting from the word is equal to or
    * greater than this number
    */
    int minNumOfWordsWithAPrefixForPrinting;
    
    
    /**
    * filePath[0] - file path for the dictionary vocabulary file
    * providing words to populate the dictionary tree
    * filePath[1] - file path for the test source file
    * providing words to be used for testing
    */
    const char *filePath[NUMOFFILES];
    
    
    /** store total number of characters in files, total word count in files
    * totalNumOfCharsInFile[DICTSRCFILEINDEX]
    * - number of total chars in the dictionary vocabulary file.
    * use stat, lstat, or fstat system call
    * totalNumOfCharsInFile[TESTFILEINDEX]
    * - number of total chars in the test file
    *
    * Hints: see man stat, lstat, or fstat to get the total number of bytes of the file
    * stat or lstat uses a filename, whereas fstat requires a file descriptor from a
    * low-level I/O call: e.g. open. If you are using high-level I/O, either use stat
    * (or lstat), or open the file first with the low-level I/O, then call fstat,
    * then close it.)
    *
    * Important: assume all text files are in Unix text file format, meaning, the end
    * of each line only has a line feed character. The stat, lstat, fstat would include the
    * count of the line feed character from each line.
    */
    long totalNumOfCharsInFile[NUMOFFILES];
    
    
    /**
    * Use numOfCharsProcessedFromFile to track ongoing progress of
    * number of characters read in from files and the subsequent thread logic.
    * We will refer to the long integer to which this variable dereferences as the
    * progress indicator.
    *
    * This progress indicator is updated by worker threads, and used by the main
    * thread to display the progress for tracking the execution of the worker threads
    *
    * Important: this number can be incremented by the number of characters in
    * the line that is being read and processed, plus one to include the
    * line feed character at the end of each line of a Unix text file.
    * Do NOT convert the text files to a Windows DOS format.
    *
    * File is read in line by line, for each line read in:
    * 1) tokenize the line by delimiters to a word array, with each separated word:
    * insert the word to the dictionary tree or search, count, and print the word
    * in the dictionary tree, and then increment the word count of the file:
    * wordCountInFile (see below)
    *
    * 2) update the numOfCharsProcessedFromFile by incrementing it by the
    * number of characters in the line, plus one to include the line feed
    * character at the end of each line of a Unix text file.
    *
    * numOfCharsProcessedFromFile[DICTSRCFILEINDEX]
    * - number of chars read in and processed from
    * the dictionary vocabulary file
    * numOfCharsProcessedFromFile[TESTFILEINDEX]
    * - number of chars read in and processed from
    * the test file
    */
    long *numOfCharsProcessedFromFile[NUMOFFILES];
    
    
    /**
    * wordCountInFile[DICTSRCFILEINDEX]
    * - number of total words in the dictionary vocabulary file.
    * wordCountInFile[TESTFILEINDEX]
    * - number of total words in the test file
    */
    long wordCountInFile[NUMOFFILES];
    
    
    /**
    * completed flags indicate the completion of the thread logic, one
    * for the populatetree thread, one for countwords thread
    * Important: the completed flag of populatetree thread may be
    * used to force the precedence constraint that the populatetree
    * thread logic must be executed before the countwords thread
    *
    * taskCompleted[DICTSRCFILEINDEX]
    * - boolean flag to indicate whether the tree population
    * thread has completed the task: read words from the
    * dictionary source file and populate the tree
    *
    * important: you may want to set the completed flag for the
    * populatetree thread to true only after the main thread fully displays
    * the progress bar upon the completion of the thread logic
    *
    * taskCompleted[TESTFILEINDEX]
    * - boolean flag to indicate whether the counting words
    * thread has completed the task for reading and processing all words from the
    * test file.
    */
    
    bool taskCompleted[NUMOFFILES];
} EXEC_STATUS;

#endif

struct dictNode *getNode(void);
/* Blueprint for instantiating new nodes
 * Each Node can potentially point at 27 characters
 */

int charToIndex(const char *word, int level);
/* Works its magic on the alphabetical characters.
 * Single quote: ' = 39
 * Uppercase: A = 65, B = 66, ..., Y = 89, Z = 90.
 * Lowercase: a = 97, b = 98, ..., y = 121, z = 122.
 * a and A are set to 1, ..., z and Z are set to 26, and
 * ' is set to 0.
 * Point of Interest:
 * The variable "level" is used to illustrate the concept of iterating
 * through the lowest letter, from left-to-right, to the 
 * highest letter of any particular word throughout dictionary.c.
 */

void add(struct dictNode *dictnode, const char *word);
/* Piece by Piece the dictionary tree is assembled.
 * Takes in a node that the user wants to be the root of the tree
 * and a particular word to add to said tree.
 * Result is a node that is the root of a complete dictionary tree.
 */

dictNode* findEndingNodeOfAStr(struct dictNode *dictnode, const char *word);
/* Traverse the characters of a particular given word
 * that also starts at a particular node.
 * Returns the node that points to the final character
 * in the word.
 */

int prefixCount(struct dictNode *dictnode);
/* Recursivley search the sub-trees of a given node.
 * Looking for a true isWord flag.
 * Returning the total sum of true isWord flags found
 * potential prefix matches are between 0 and 10000+.
 */

void *populateTreeStartRoutine(void *arg);
/* Handles the DICTSRCFILEINDEX, basically a wrapper for the 
 * previous dictionary tree prototypes. 
 * Also handles progress mark creation by actively updating
 * numOfCharsProcessedFromFile.
 */

void *countWordsStartRoutine(void *arg);
/* Handles the TESTFILEINDEX, basically a wrapper for the
 * previous word analysis prototypes.
 * Also handles progress mark creation by actively updating
 * numOfCharsProcessedFromFile.
 */
