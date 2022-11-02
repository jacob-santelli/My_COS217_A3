/* Interface for Symbol Table functions */
#ifndef SYM_INCLUDED
#define SYM_INCLUDED
#include <stdio.h>

/* A SymTable_T is a collection of keys (strings) bound to a set of
   values, which can be of any type. */
typedef struct SymTable *SymTable_T;

/*  Return a new SymTable_T object, or NULL if insufficient memory is
   available. */
SymTable_T SymTable_new(void);

/* Free oSymTable */
void SymTable_free(SymTable_T oSymTable);

/* Return number of bindings in oSymTable */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Add the binding pcKey-pvValue to oSymTable. Returns 1 (TRUE) 
   if successful, or 0 (FALSE) if insufficient memory is available. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* SymTable_replace replaces the pcKey's bound  value with pvValue and 
   returns the old value. Otherwise it leaves oSymTable unchanged and 
   returns NULL.*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
                       const void *pvValue);

/* SymTable_contains returns 1 (TRUE) if oSymTable contains a binding
   whose key is pcKey, and 0 (FALSE) otherwise. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Returns the value of the binding within oSymTable whose key is 
   pcKey, or NULL if no such binding exists.  */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* If oSymTable contains a binding with key pcKey, then 
   SymTable_remove removes that binding from oSymTable and returns the
   binding's value. Otherwise SymTable_remove does not change 
   oSymTable and returns NULL. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Calls (*pfApply) for all key-value bindings in oSymTable,
passes pvExtra as an extra parameter */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);
#endif
  