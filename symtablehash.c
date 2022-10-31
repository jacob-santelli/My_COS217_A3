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
struct Bucket
{
   /* The address of the first StackNode. */
   struct BucketNode *psFirstNode;
};

/* ADD COMMENT HERE */
struct SymTable
{
   /* ADD COMMENT HERE */
   struct Bucket *hashTable[509];
   
   /* ADD COMMENT HERE */
   size_t nodeCount;

   /* ADD COMMENT HERE */
   size_t hashTableSize;
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

static int SymTable_hasBucket(SymTable_T oSymTable, const char *pcKey) {
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if (oSymTable->hashTable
      [SymTable_hash(pcKey, oSymTable->hashTableSize)] == NULL) {
         return 0;
      }
   return 1;
}

/* ADD COMMENT HERE */
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->nodeCount = 0;
   oSymTable->hashTableSize = 509;
   return oSymTable;
}

/* ADD COMMENT HERE */
void SymTable_free(SymTable_T oSymTable) {
   struct BucketNode *psCurrentNode;
   struct BucketNode *psNextNode;
   struct Bucket *tempBucket;

   assert(oSymTable != NULL);

   for(int i = 0; i < 509; i++) {
      if (oSymTable->hashTable[i] != NULL) {
         tempBucket = oSymTable->hashTable[i];
         for (psCurrentNode = tempBucket->psFirstNode;
         psCurrentNode != NULL;
         psCurrentNode = psNextNode) 
            {
            psNextNode = psCurrentNode->psNextNode;
            free((char*)(psCurrentNode->pcKey));
            free(psCurrentNode);
            psCurrentNode = NULL;
            }
         free(tempBucket);
      }
   }
   free(oSymTable);
}

/* ADD COMMENT HERE */
size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   return oSymTable->nodeCount;
}

/* ADD COMMENT HERE */
static struct Bucket SymTable_newBucket(void) {
   struct Bucket bucket;

   bucket = malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->nodeCount = 0;
   oSymTable->hashTableSize = 509;
   return oSymTable;
}

/* ADD COMMENT HERE */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct Bucket *psNewNode;
   char *pcTempKey;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->hashTableSize);

   if (oSymTable->hashTable[hash] == NULL)

  
   psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
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
   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;

   oSymTable->nodeCount++;
   return 1;
}

/* ADD COMMENT HERE */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
 
   struct SymTableNode *tempNode;
   const void *tempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   tempNode = oSymTable->psFirstNode;
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
   struct SymTableNode *tempNode;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   tempNode = oSymTable->psFirstNode;
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
   struct SymTableNode *tempNode;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   tempNode = oSymTable->psFirstNode;
   while (tempNode != NULL) {
      if(strcmp(tempNode->pcKey, pcKey) == 0) return (void*) tempNode->pvValue;
      tempNode = tempNode->psNextNode;
   }
   return 0;
}

/* ADD COMMENT HERE */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

   struct SymTableNode *tempNode_current;
   struct SymTableNode *tempNode_next;
   const void *tempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if (SymTable_contains(oSymTable, pcKey) != 1) return NULL;

   tempNode_current = oSymTable->psFirstNode;
   tempNode_next = tempNode_current->psNextNode;
   if (strcmp(tempNode_current->pcKey, pcKey) == 0) {
      oSymTable->psFirstNode = tempNode_next;
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
         tempNode_current->psNextNode = (struct SymTableNode *) tempNode_next->psNextNode;
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
   struct SymTableNode *tempNode_current;
   assert(oSymTable != NULL);
   assert(pfApply != NULL);

  
    for (tempNode_current = oSymTable->psFirstNode; 
         tempNode_current != NULL; 
         tempNode_current = tempNode_current->psNextNode) {
      (*pfApply)(tempNode_current->pcKey, (void*)tempNode_current->pvValue, (void*)pvExtra);
   }
}