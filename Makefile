.phony all:
all: pman

pman: main.c linked_list.c
	gcc -Wall main.c linked_list.c -o PMan
	gcc -Wall inf.c -o inf
	gcc -Wall args.c -o args
	
.PHONY clean:
clean:
	-rm -rf *.o *.exe
