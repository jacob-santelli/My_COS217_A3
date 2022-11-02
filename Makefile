# Macros
CC = gcc217
CC = gcc217m
CFLAGS =
# CFLAGS = -g
# CFLAGS = -D NDEBUG
# CFLAGS = -D NDEBUG -O
# Dependency rules for non-file targets
all: testsymtablelist testsymtablehash
clobber: clean
	rm -f *~ \#*\#
clean:
	rm -f testsymtablelist *.o
	rm -f testsymtablehash *.o

# Dependency rules for file targets

testsymtablelist: symtablelist.o testsymtable.o
	$(CC) $(CFLAGS) symtablelist.o testsymtable.o -o testsymtablelist

testsymtablehash: symtablehash.o testsymtable.o
	$(CC) $(CFLAGS) symtablehash.o testsymtable.o -o testsymtablehash

testsymtable.o: testsymtable.c symtable.h
	$(CC) $(CFLAGS) -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
	$(CC) $(CFLAGS) -c symtablelist.c

symtablehash.o: symtablehash.c symtable.h
	$(CC) $(CFLAGS) -c symtablehash.c