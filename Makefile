.phony all:
all: pman

pman: main.c linked_list.c
	gcc -Wall main.c linked_list.c -o PMan

.PHONY clean:
clean:
	-rm -rf *.o *.exe
