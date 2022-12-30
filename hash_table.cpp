/*
 Student Name: Anastasia Kotova
 Date: 10/31/2021

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local variable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
    // Allocate memory for a hash table entry on the heap
    HashTableEntry* newTableEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));

    // Initialize the entry with key and value
    newTableEntry->key = key;
    newTableEntry->value = value;
    // Initialize pointer to the next entry as NULL
    newTableEntry->next = NULL;

    return newTableEntry;
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
    // Create a pointer to the head of the list
    HashTableEntry* currTableEntry = hashTable->buckets[hashTable->hash(key)];

    // Traverse the list until hit NULL
    while (currTableEntry) {
        if (currTableEntry->key == key) {
            // If the key found, return the pointer to that hash table entry
            return currTableEntry;
        }
        currTableEntry = currTableEntry->next;
    }

    // If the key DNE...
    return NULL;
}

/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
    HashTableEntry* currTableEntry;
    HashTableEntry* temp;

    for (int i = 0; i < hashTable->num_buckets; i++) {
        // Initialize a pointer to the head of the list
        currTableEntry = hashTable->buckets[i];
        // Traverse the list until hit NULL
        while (currTableEntry) {
                // Save the pointer to the current entry at the dummy pointer
            temp = currTableEntry; 
            // Go to the next entry 
            currTableEntry = currTableEntry->next; 
            // Free the dummy
            free(temp->value); 
            free(temp); 
        }
    }
    // Free the hash table
    free(hashTable->buckets);
    free(hashTable);
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
    // Use the findItem function to look up for the existing entry
    HashTableEntry* currTableEntry = findItem(hashTable, key);
    // If the key is in the list...
    if (currTableEntry) {
        // Save the current entry value at the dummy variable for return
        void* temp = currTableEntry->value;
        // Update the value at that entry
        currTableEntry->value = value;
        return temp;
    }

    // If the key is NOT in the list, create the new hash table entry
    currTableEntry = createHashTableEntry(key, value);
    // Check if DNE
    if (!currTableEntry) {
        return NULL; 
    }
  // Otherwise, insert the etry
  currTableEntry->next = hashTable->buckets[hashTable->hash(key)];
  hashTable->buckets[hashTable->hash(key)] = currTableEntry; 
  return NULL;
}

void* getItem(HashTable* hashTable, unsigned int key) {
    // Use the findItem function to look up; return its value if found
    if (findItem(hashTable, key)) {
        return findItem(hashTable, key)->value;
    }
    return NULL;
}

void* removeItem(HashTable* hashTable, unsigned int key) {
    // Initialize a pointer to the head of the list
  HashTableEntry* currTableEntry = hashTable->buckets[hashTable->hash(key)];

  // If the key is at the head...
  if (currTableEntry && currTableEntry->key == key) {
    // Save the current value at the dummy variable for return
    void* temp = currTableEntry->value;
    // Go to the next entry 
    hashTable->buckets[hashTable->hash(key)] = currTableEntry->next;
    // Free the current entry
    free(currTableEntry);
    return temp;
  }

    // If the item is NOT at the head of the list...
  while (currTableEntry && currTableEntry->next) {
    if (currTableEntry->next->key == key) {
        // Save the next value at the dummy variable for return
      HashTableEntry* tempEntry = currTableEntry->next;
      // Go to the next-next entry 
      void* temp = tempEntry->value;
      currTableEntry->next = currTableEntry->next->next;
      // Free the next entry
      free(tempEntry);
      return temp;
    }
    // Go to the next entry
    currTableEntry = currTableEntry->next;
  }
  return NULL;
}

void deleteItem(HashTable* hashTable, unsigned int key) {
    // Use the removeItem function to free the entry
    free(removeItem(hashTable, key));
}