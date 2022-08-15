# trie-and-pthreads
Dictionary Tree implementation with POSIX Threads in C

-	Main thread reads and processes command line arguments, creates and initializes a shared data structure to be shared between the main thread and worker threads, then spawns two worker threads to carry out the following sequence of logic in that order:

-	1. populatetree thread for reading words from the dictionary source file and populating a dictionary tree.

-	2. countwords thread for reading words from a test file, and for every word read in from the test file: a. search and count the number of words in the dictionary tree that start with this word, b. if the count is equal to or greater than the number specified by an optional command line argument print this word and the count to a file.
