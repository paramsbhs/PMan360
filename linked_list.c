#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	Node* newNode = (Node*)malloc(sizeof(Node)); //Allocate memory for a new Node
	if(newNode == NULL){
		perror("Memory Allocation Failed (Node)"); //Check to see if memory allocation is successful
		return head;
	}

	newNode->path = (char*)malloc(strlen(new_path)+1); //Allocate memory for the path string
	if(newNode->path == NULL){
		perror("Memory Allocation Failed (path)"); //Check to see if memory allocation is successful
		free(newNode); //if memory allocation fails, free the previous allocated memory for the node
		return head;	
	}
	strcpy(newNode->path, new_path); //Copy the new desired path onto the node
	newNode->pid = new_pid; //Set the new pid to the new node
	newNode->next = head; //Set the next pointer of the new node to the head
	
	return newNode; 
}


Node * deleteNode(Node* head, pid_t pid){
	if(head == NULL){ //Check if the list is empty, return NULL if it is
		return NULL;
	}

	if(head->pid == pid){ //If the head is matching the pid number,
		Node* temporaryNode = head; //Delete the head
		head = temporaryNode->next; //Set the head to the next node
		free(temporaryNode); //free the temporary node created as it is not required anymore
		return head; //return the node deleted
	}

	Node* currentNode = head; //create a current node as the head
	while(currentNode->next != NULL){ //Iterate until the end of the list
		if(currentNode->next->pid == pid){ //if the next nodes pid is matching, delete it
			Node* temporaryNode = currentNode->next; //store reference for the next node
			currentNode->next = currentNode->next->next; //set the next node to the node after
			free(temporaryNode); //free temporary node as it is not required anymore
			return head;
		}
		currentNode = currentNode->next; //Iterate through the list
	}
	
	return head;
}

void printList(Node *node){
	while(node != NULL){ //loop through the linked list
		printf("PID %d, Path %s\n", node->pid, node->path); //print the pid and path of the current node
		node = node->next; //traverse through the list
	}
}


int PifExist(Node *node, pid_t pid){
	while(node != NULL){ //traverse to the end of the list
		if(node->pid == pid){ //check the pid of the current node and if it is equal to the pid desired
			return 1; //return 1 if it exists or is true
		}
		node = node->next; //traverse through the list
	}

  	return 0; //if pid not found, return 0 as it is false
}

