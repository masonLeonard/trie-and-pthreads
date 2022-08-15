/* Mason Leonard 818030805
 * CS 480
 * Professor Ben Shen
 * Assignment 2: Dictionary Tree, Pointer Tree Data Structure
 * 2/22/2022
 *
 * Tree operations:
 * Insert a word to the tree.
 * Count all words in the tree that start with a specific string.
 */
#include "dicttree.h"

struct dictNode *getNode(void)
{
    struct dictNode *parent = NULL;

    parent = (struct dictNode *)malloc(sizeof(struct dictNode));
    if(parent)
    {
        int i;
        parent->isWord = false;
        
        for(i = 0; i < NUMOfCHARS; i++)
        {
            parent->next[i] = NULL;
        }
    }
    return parent;
}

int charToIndex(const char *word, int level)
{
    int actual = tolower(word[level]);
    
    if(actual == 39)
    {
        return 0;
    }
    
    int indexOfLetter = actual - 96;
    return indexOfLetter;
}

void add(struct dictNode *dictNode, const char *word)
{
    int level, index;
    int length = strlen(word);
    struct dictNode *tempRoot = dictNode;

    for(level = 0; level < length; level++)
    {
        index = charToIndex(word, level);
        if(!tempRoot->next[index])
        {
            tempRoot->next[index] = getNode();
        }
        
        tempRoot = tempRoot->next[index];
    }
    tempRoot->isWord = true;
}

dictNode* findEndingNodeOfAStr(struct dictNode *dictnode, const char *word)
{
    int level, index;
    int length = strlen(word);
    struct dictNode *tempRoot = dictnode;

    for (level = 0; level < length; level++)
    {
        index = charToIndex(word, level);
        if (!tempRoot->next[index]) //!NULL == 1/true 
        {
            return tempRoot->next[index]; //therefore a live node is returned
        }

        tempRoot = tempRoot->next[index];
    }
    return tempRoot;
}

int prefixCount(struct dictNode *dictnode)
{
    int i;
    int result = 0;

    //recursive search may find no matches 
    //thus no word begins with such a prefix
    if(dictnode == NULL) 
    {
        return 0;
    }
    else if(dictnode->isWord)
    {
        result++;
    }

    for (i = 0; i < NUMOfCHARS; i++)
    {
        if (dictnode->next[i])
        {
            result += prefixCount(dictnode->next[i]);
        }
    }
    return result;
}
