/* ADD COMMENT HERE */

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

/* ADD COMMENT HERE */
struct BucketNode
{
   /* The item and key. */
   const char *pcKey;

   /* ADD COMMENT HERE */
   const void *pvValue;

   /* The address of the next StackNode. */
   struct BucketNode *psNextNode;
};

/* ADD COMMENT HERE */
struct SymTable
{
   /* ADD COMMENT HERE */
   struct BucketNode **hashTable;
   
   /* ADD COMMENT HERE */
   size_t nodeCount;

   /* ADD COMMENT HERE */
   size_t hashTableSize;

   /* ADD COMMENT HERE */
   int *resized;
};

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

/* ADD COMMENT HERE */
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   int i;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->hashTable = malloc(sizeof(struct BucketNode*)*509);
   if (oSymTable->hashTable == NULL) return NULL;

   oSymTable->resized = malloc(sizeof(int*)*7);
   if (oSymTable->resized == NULL) return NULL;
   for (i = 0; i < 7; i++) {
      oSymTable->resized[i] = 0;
   }

   oSymTable->nodeCount = 0;
   oSymTable->hashTableSize = 509;
   return oSymTable;
}

SymTable_T SymTable_resize(SymTable_T oSymTable, size_t newSize) {
   SymTable_T oSymTableNew;
   struct BucketNode *psCurrentNode;
   struct BucketNode *psNextNode;
   size_t hash;
   size_t hashNew;

   oSymTableNew = oSymTable;

   oSymTableNew->hashTable = malloc(sizeof(struct BucketNode*)*newSize);
   if (oSymTableNew->hashTable == NULL) return oSymTable;

   if (newSize == 1021) oSymTableNew->resized[0] = 1;
   else if (newSize == 2039) oSymTableNew->resized[1] = 1;
   else if (newSize == 4093) oSymTableNew->resized[2] = 1;
   else if (newSize == 8191) oSymTableNew->resized[3] = 1;
   else if (newSize == 16381) oSymTableNew->resized[4] = 1;
   else if (newSize == 32749) oSymTableNew->resized[5] = 1;
   else if (newSize == 65521) oSymTableNew->resized[6] = 1;
   
   oSymTableNew->nodeCount = 0;
   oSymTableNew->hashTableSize = newSize;

   for(hash = 0; hash < oSymTable->hashTableSize; hash++) {
      if(oSymTable->hashTable[hash] != NULL) {
         psCurrentNode = oSymTable->hashTable[hash];
         while(psCurrentNode != NULL) {
            psNextNode = psCurrentNode->psNextNode;

            hashNew = SymTable_hash(psCurrentNode->pcKey, newSize);
            
            psCurrentNode->psNextNode = oSymTableNew->hashTable[hashNew];
            oSymTableNew->hashTable[hash] = psCurrentNode;
            oSymTableNew->nodeCount++;

            psCurrentNode = psNextNode;
         }
      }
   }
   return oSymTableNew;
}

/* ADD COMMENT HERE */
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
   free(oSymTable->resized);
   free(oSymTable);
}

/* ADD COMMENT HERE */
size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   return oSymTable->nodeCount;
}


/* ADD COMMENT HERE */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct BucketNode *psNewNode;
   char *pcTempKey;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);
  
   psNewNode = (struct BucketNode*)malloc(sizeof(struct BucketNode));
   if (psNewNode == NULL) 
      return 0;

   if (SymTable_contains(oSymTable, pcKey)) 
      return 0;

   pcTempKey = malloc(strlen(pcKey) + 1);
   if (pcTempKey == NULL)
      return 0;

   strcpy(pcTempKey, pcKey);

   psNewNode->pcKey = pcTempKey;
   psNewNode->pvValue = pvValue;
   psNewNode->psNextNode = oSymTable->hashTable[hash];
   oSymTable->hashTable[hash] = psNewNode;
   oSymTable->nodeCount++;

   if (oSymTable->nodeCount == oSymTable->hashTableSize + 1) {
      if (oSymTable->resized[0] == 0) {
         oSymTable = SymTable_resize(oSymTable, 1021);
      }
      else if (oSymTable->resized[1] == 0) {
         oSymTable = SymTable_resize(oSymTable, 2039);
      }
      else if (oSymTable->resized[2] == 0) {
         oSymTable = SymTable_resize(oSymTable, 4093);
      }
      else if (oSymTable->resized[3] == 0) {
         oSymTable = SymTable_resize(oSymTable, 8191);
      }
      else if (oSymTable->resized[4] == 0) {
         oSymTable = SymTable_resize(oSymTable, 16381);
      }
      else if (oSymTable->resized[5] == 0) {
         oSymTable = SymTable_resize(oSymTable, 32749);
      }
      else if (oSymTable->resized[6] == 0) {
         oSymTable = SymTable_resize(oSymTable, 65521);
      }
   }

   return 1;
}

/* ADD COMMENT HERE */
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

/* ADD COMMENT HERE */
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


/* ADD COMMENT HERE */
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
   return 0;
}

/* ADD COMMENT HERE */
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

/* ADD COMMENT HERE */
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