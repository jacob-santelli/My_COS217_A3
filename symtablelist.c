#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

struct SymTableNode
{
   /* The item and key. */
   const char *pcKey;
   const void *pvValue;

   /* The address of the next StackNode. */
   struct SymTableNode *psNextNode;
};

struct SymTable
{
   /* The address of the first StackNode. */
   struct SymTableNode *psFirstNode;
   size_t nodeCount;
};

/* SymTable_new must return a new SymTable object that contains no bindings, or NULL if insufficient memory is available. */
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->nodeCount = 0;
   return oSymTable;
}

/* SymTable_free must free all memory occupied by oSymTable. */
void SymTable_free(SymTable_T oSymTable) {
   struct SymTableNode *tempNode_1;
   struct SymTableNode *tempNode_2;
   assert(oSymTable != NULL);
   tempNode_1 = oSymTable->psFirstNode;
   while (tempNode_2 != NULL) {
      tempNode_2 = tempNode_2->psNextNode;
      free((char*)(tempNode_1->pcKey));
      free(tempNode_1);
      tempNode_1 = tempNode_2;
   }
   free(oSymTable);
}

/* SymTable_getLength must return the number of bindings in oSymTable. */
size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);
   return oSymTable->nodeCount;
}

   /*
   struct SymTableNode *tempNode;
   size_t length;
   

   tempNode = oSymTable->psFirstNode;
   while (tempNode != NULL) {
      length++;
      tempNode = tempNode->psNextNode;
   }
   return length;
   */

/* If oSymTable does not contain a binding with key pcKey, then SymTable_put must add a new binding 
to oSymTable consisting of key pcKey and value pvValue and return 1 (TRUE). Otherwise the function must 
leave oSymTable unchanged and return 0 (FALSE). If insufficient memory is available, then the function 
must leave oSymTable unchanged and return 0 (FALSE). 

A SymTable object is responsible for allocating the memory in which its keys reside. Specifically, SymTable_put must 
not simply store the value of pcKey within the binding that it creates. Instead SymTable_put must make a defensive copy 
of the string to which pcKey points, and store the address of that copy within the new binding. You will find the 
standard C functions strlen, malloc, and strcpy useful for making the copy. Thereafter a SymTable object must own its 
keys. That is, a SymTable object must free the memory in which its keys reside when that memory is no longer required.

PAGE 40 OF LECTURE 10 EXPLAINS DEFENSIVE COPIES

Conversely, a SymTable object is not responsible for allocating the memory in which its values reside. Specifically, 
SymTable_put must simply store the value of pvValue within the binding that it creates. SymTable_put must not make a 
defensive copy of the object to which pvValue points. In fact SymTable_put cannot make a copy of the object pointed to 
by pvValue; since SymTable_put cannot determine the type of that object, SymTable_put cannot make a copy of that object. 
Thus a SymTable object must not own its values; it must not free the memory in which its values reside. (That memory might 
not even be in the heap! Freeing it could be a disaster!) */


int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
   struct SymTableNode *psNewNode;
   char *pcTempKey;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
   if (psNewNode == NULL) 
      return 0;

   if (SymTable_contains(oSymTable, pcKey)) 
      return 0;

   pcTempKey = malloc(strlen(pcKey) + 1);
   if (pcTempKey == NULL)
      return 0;

   strcpy(pcTempKey, pcKey);

   oSymTable->nodeCount++;
   psNewNode->pcKey = pcTempKey;
   psNewNode->pvValue = pvValue;
   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;
   return 1;
}

/* If oSymTable contains a binding with key pcKey, then SymTable_replace must replace the binding's 
value with pvValue and return the old value. Otherwise it must leave oSymTable unchanged and return NULL. */
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

/* SymTable_contains must return 1 (TRUE) if oSymTable contains a binding whose key is pcKey, and 0 (FALSE) otherwise. */
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


/* SymTable_get must return the value of the binding within oSymTable whose key is pcKey, or NULL if no such binding exists.*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
   struct SymTableNode *tempNode;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   tempNode = oSymTable->psFirstNode;
   while (tempNode != NULL) {
      /* not sure about this */
      if(strcmp(tempNode->pcKey, pcKey) == 0) return (void*) tempNode->pvValue;
      tempNode = tempNode->psNextNode;
   }
   return 0;
}

/* If oSymTable contains a binding with key pcKey, then SymTable_remove must remove that binding from oSymTable and 
return the binding's value. Otherwise the function must not change oSymTable and return NULL. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

   struct SymTableNode *tempNode_current;
   struct SymTableNode *tempNode_next;
   const void *tempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);


   tempNode_current = oSymTable->psFirstNode;
   tempNode_next = tempNode_current->psNextNode;
   if (strcmp(tempNode_current->pcKey, pcKey) == 0) {
      oSymTable->psFirstNode = tempNode_next;
      tempValue = tempNode_current->pvValue;
      free((char*)tempNode_current->pcKey);
      free(tempNode_current);
      oSymTable->nodeCount--;
      return (void *) tempValue;
   }
    


   while (tempNode_next != NULL) {
      if(strcmp(tempNode_next->pcKey, pcKey) == 0) {
         tempValue = tempNode_next->pvValue;
         tempNode_current->psNextNode = (struct SymTableNode *) tempNode_next->psNextNode;
         free((char*)tempNode_next->pcKey);
         free(tempNode_next);
         oSymTable->nodeCount--;
         return (void *) tempValue;
      }
      tempNode_current = tempNode_next;
      tempNode_next = tempNode_current->psNextNode;
   }


   return NULL;
}

/* SymTable_map must apply function *pfApply to each binding in oSymTable, passing pvExtra as an extra parameter. 
That is, the function must call (*pfApply)(pcKey, pvValue, pvExtra) for each pcKey/pvValue binding in oSymTable. */
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