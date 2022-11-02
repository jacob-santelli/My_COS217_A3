/* Module defining a number of symbol table functions with using a
   hash linked list implementation. */

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

/* Each key-value binding is stored in a SymTableNode. SymTableNodes
   are placed in sequence to form lists. */
struct SymTableNode
{
   /* The binding's key. */
   const char *pcKey;
   
   /* The binding's value. */
   const void *pvValue;

   /* The address of the next SymTableNode. */
   struct SymTableNode *psNextNode;
};

/* ADD COMMENT HERE */
struct SymTable
{
   /* The address of the first StackNode. */
   struct SymTableNode *psFirstNode;

   /* Number of elements in SymTable */
   size_t nodeCount;
};

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->nodeCount = 0;
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
   struct SymTableNode *psCurrentNode;
   struct SymTableNode *psNextNode;

   assert(oSymTable != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
   {
      psNextNode = psCurrentNode->psNextNode;
      free((char*)(psCurrentNode->pcKey));
      free(psCurrentNode);
      psCurrentNode = NULL;
   }
   free(oSymTable);
}


size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   return oSymTable->nodeCount;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct SymTableNode *psNewNode;
   char *pcTempKey;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
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
   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;

   oSymTable->nodeCount++;
   return 1;
}

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

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
   struct SymTableNode *tempNode;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   tempNode = oSymTable->psFirstNode;
   while (tempNode != NULL) {
      if(strcmp(tempNode->pcKey, pcKey) == 0) return (void*) tempNode->pvValue;
      tempNode = tempNode->psNextNode;
   }
   return NULL;
}

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