/* Module defining a number of symbol table functions with using a
   hash table implementation. */

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

/* All possible bucket numbers */
static const size_t buckets[] = 
{509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/* Each key-value binding is stored in a BucketNode. BucketNodes
   are placed in buckets to form lists. */
struct BucketNode
{
   /* The binding's key. */
   const char *pcKey;

   /* The binding's value. */
   const void *pvValue;

   /* The address of the next BucketNode. */
   struct BucketNode *psNextNode;
};

/* A SymTable tracks a hash table containing lists of key-value
   bindings, in addition to tracking its resizing/size. */
struct SymTable
{
   /* The address of the first element of an array of 
      BucketNodes, each element of which is a Bucket
      in the SymTable's hash table */
   struct BucketNode **hashTable;
   
   /* The number of bindings in the SymTable. */
   size_t nodeCount;

   /* The number of buckets in the hash table. */
   size_t hashTableSize;
};

/* Calculates and returns the proper hash of string pcKey given a
   certain number of buckets (uBucketCount). */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->hashTable = calloc(buckets[0],sizeof(struct BucketNode*));
   if (oSymTable->hashTable == NULL) return NULL;

   oSymTable->nodeCount = 0;
   oSymTable->hashTableSize = buckets[0];
   return oSymTable;
}

/* Resizes oSymTable's hash table to be newSize, copying all 
   bindings into the new hash table. */
static void SymTable_expand(SymTable_T oSymTable, size_t newSize) {
   struct BucketNode *psCurrentNode;
   struct BucketNode *psNextNode;
   struct BucketNode **table;
   size_t hash;
   size_t hashNew;

   table = calloc(newSize,sizeof(struct BucketNode*));
   if (table == NULL) return;

   /* Iterate through oSymTable and move all bindings into table */
   for(hash = 0; hash < oSymTable->hashTableSize; hash++) {
      if(oSymTable->hashTable[hash] != NULL) {
         psCurrentNode = oSymTable->hashTable[hash];
         while(psCurrentNode != NULL) {
            psNextNode = psCurrentNode->psNextNode;

            hashNew = SymTable_hash(psCurrentNode->pcKey, newSize);
            
            psCurrentNode->psNextNode = table[hashNew];
            table[hashNew] = psCurrentNode;

            psCurrentNode = psNextNode;
         }
      }
   }
   
   oSymTable->hashTableSize = newSize;
   free(oSymTable->hashTable);
   oSymTable->hashTable = table;
}

void SymTable_free(SymTable_T oSymTable) {
   struct BucketNode *psCurrentNode;
   struct BucketNode *psNextNode;
   size_t hash;

   assert(oSymTable != NULL);

   for(hash = 0; hash < oSymTable->hashTableSize; hash++) {
      if (oSymTable->hashTable[hash] != NULL) {
         for (psCurrentNode = oSymTable->hashTable[hash];
         psCurrentNode != NULL;
         psCurrentNode = psNextNode) 
            {
            psNextNode = psCurrentNode->psNextNode;
            free((char*)(psCurrentNode->pcKey));
            free(psCurrentNode);
            psCurrentNode = NULL;
         }
      }
   }
   free(oSymTable->hashTable);
   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   return oSymTable->nodeCount;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct BucketNode *psNewNode;
   char *pcTempKey;
   size_t hash;
   long unsigned int i;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);
  
   /* Allocate data to new node, make sure there is enough space */
   psNewNode = (struct BucketNode*)malloc(sizeof(struct BucketNode));
   if (psNewNode == NULL) 
      return 0;

   if (SymTable_contains(oSymTable, pcKey)) { 
      free(psNewNode); 
      return 0;
   }
   pcTempKey = malloc(strlen(pcKey) + 1);
   if (pcTempKey == NULL) {
      free(psNewNode);
      return 0;
   }

   strcpy(pcTempKey, pcKey);

   psNewNode->pcKey = pcTempKey;
   psNewNode->pvValue = pvValue;

   /* insert the new binding into the symbol table */
   psNewNode->psNextNode = oSymTable->hashTable[hash];
   oSymTable->hashTable[hash] = psNewNode;
   oSymTable->nodeCount++;

   if (oSymTable->nodeCount == oSymTable->hashTableSize) {
      for (i = 0; i < (sizeof(buckets)/sizeof(buckets[0]) - 1); i++) {
         if (oSymTable->nodeCount == buckets[i])
         SymTable_expand(oSymTable, buckets[i + 1]);
      }
   }

   return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct BucketNode *tempNode;
   const void *tempValue;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);
   
   tempNode = oSymTable->hashTable[hash];
   while (tempNode != NULL) {
      if(strcmp(tempNode->pcKey, pcKey) == 0) {
         tempValue = tempNode->pvValue;
         tempNode->pvValue = pvValue;
         return (void *) tempValue;
      }
      tempNode = tempNode->psNextNode;
   }
   return NULL; 
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
   struct BucketNode *tempNode;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);

   tempNode = oSymTable->hashTable[hash];
   while (tempNode != NULL) {
      if(strcmp(tempNode->pcKey, pcKey) == 0) {
         return 1;
      }
      tempNode = tempNode->psNextNode;
   }
   
   return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
   struct BucketNode *tempNode;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);

   tempNode = oSymTable->hashTable[hash];
   while (tempNode != NULL) {
      if(strcmp(tempNode->pcKey, pcKey) == 0) return (void*) tempNode->pvValue;
      tempNode = tempNode->psNextNode;
   }
   return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
   struct BucketNode *tempNode_current;
   struct BucketNode *tempNode_next;
   const void *tempValue;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if (SymTable_contains(oSymTable, pcKey) != 1) return NULL;

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);

   tempNode_current = oSymTable->hashTable[hash];
   tempNode_next = tempNode_current->psNextNode;
   if (strcmp(tempNode_current->pcKey, pcKey) == 0) {
      oSymTable->hashTable[hash] = tempNode_next;
      tempValue = tempNode_current->pvValue;
      free((char*)tempNode_current->pcKey);
      free(tempNode_current);
      tempNode_current = NULL;
      oSymTable->nodeCount--;
      return (void *) tempValue;
   }
    


   while (tempNode_next != NULL) {
      if(strcmp(tempNode_next->pcKey, pcKey) == 0) {
         tempValue = tempNode_next->pvValue;
         tempNode_current->psNextNode = (struct BucketNode *) tempNode_next->psNextNode;
         free((char*)tempNode_next->pcKey);
         free(tempNode_next);
         tempNode_next = NULL;
         oSymTable->nodeCount--;
         return (void *) tempValue;
      }
      tempNode_current = tempNode_next;
      tempNode_next = tempNode_current->psNextNode;
   }

   return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
   struct BucketNode *tempNode_current;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (hash = 0; hash < oSymTable->hashTableSize; hash++) {
      for (tempNode_current = oSymTable->hashTable[hash]; 
         tempNode_current != NULL; 
         tempNode_current = tempNode_current->psNextNode) {
         (*pfApply)(tempNode_current->pcKey, (void*)tempNode_current->pvValue, (void*)pvExtra);
         }
      }
   
}