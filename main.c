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

Node* head = NULL;

/* 
 * According to Tutorial 2, for fork(),
 * if pid value is less than 0, fork
 * operation failed. If pid is equal to 0,
 * the current process is the child process.
 * If pid is bigger than 0, the fork succeeded
 * and the current process is the parent process.
 */
void func_BG(char **cmd){
	pid_t pid = fork();
	if(pid < 0){ //if the pid is less than 0, fork failed
		printf("fork() operation failed\n");
		return;
	}else if(pid == 0){ //if  the pid is equal to 0, the current process is the child process
		execvp(cmd[1], &cmd[1]);
		if (execvp(cmd[1], &cmd[1]) < 0) { //From Q&A, if execvp returns -1, it failed
			printf("Error, %s failed to execute\n", cmd[1]);
			exit(-1);
		}
	}else{ //if the pid is bigger than 0, the current process is the parent process
		int status = 0; 
		pid_t pid2 = waitpid(pid, &status, WNOHANG);
		if(pid2 == 0){ //Child process is running so we can add it to the linked list		
			head = add_newNode(head, pid, cmd[1]); //use the createNode function made in the linked_list program
			printf("PID %d started\n", pid); //print the pid number
		}else if (pid2 == pid){ //Child process is terminated so check the status
			if(WIFEXITED(status) || WIFSIGNALED(status)){
				printf("Child process %d terminated\n", pid);
			}
		}
	}
}

/*
 * BGlist can use the printList function
 * created in the linked_list.c file, as
 * it iterates through the file and prints
 * the PID and the Path
 */
void func_BGlist(char **cmd){
	if(head == NULL){
		printf("No Processes Running\n");
	}else{
		printList(head);
	}
}

/*
 * Referring to Tutorial 2,
 * Given the pid in string form,
 * we need to convert this to integer
 * form and use the kill() function
 * to kill the process. If kill()
 * outputs -1, there was an error
 * killing the process, otherwise,
 * we can use the deleteNode function
 * we created in linked_list.c to
 * kill the background process
 */
void func_BGkill(char * str_pid){
	pid_t pid = atoi(str_pid);
	if(pid == 0){ //atoi error handling
		printf("PID INVALID\n");
		return;
	}
	int retVal = kill(pid, SIGTERM);
	if(retVal == -1){
		printf("Error Killing Process\n");
	}else if(retVal == 0){
		head = deleteNode(head,pid);
		printf("Killed Process %d\n", pid);	
	}
}

/* 
 * Referring to Tutorial 2,
 * Similar to BGkill but instead we
 * are using SIGSTOP for kill(pid,SIGSTOP)
 * and if it equals -1 then there is a
 * error stopping the process.
 */
void func_BGstop(char * str_pid){
	pid_t pid = atoi(str_pid);
	if(pid == 0){ //atoi error handling
		printf("PID INVALID\n");
		return;
	}
	int retVal = kill(pid,SIGSTOP);
	if(retVal == -1){
		printf("Error Stopping Process\n");
	}else if(retVal == 0){
		printf("Stopped Process %d\n", pid);
	}
}

/*
 * Referring to Tutorial 2,
 * Similar to BGkill and BGstop, but
 * instead we are using SIGCONT
 */
void func_BGstart(char * str_pid){
	pid_t pid = atoi(str_pid);
	if(pid == 0){ //atoi error handling
		printf("PID INVALID\n");
		return;
	}
	int retVal = kill(pid,SIGCONT);
	if(retVal == -1){
		printf("Error Starting Process");
	}else if(retVal == 0){
		printf("Started Process %d\n", pid);
	}
}


void func_pstat(char * str_pid){
	char path[50];
	sprintf(path, "/proc/%s/status", str_pid);
	FILE *file = fopen(path, "r");
	if(file == NULL){
		printf("Process %s does not exist\n", str_pid);
		return;
	}
	char line[256];
	while(fgets(line, sizeof(line), file)){
		if(strstr(line, "State:") != NULL){
			printf("%s", line);
		}else if(strstr(line, "VmSize:") != NULL){
			printf("%s", line);
		}else if(strstr(line, "Threads:") != NULL){
			printf("%s", line);
		}
	}
	fclose(file);
}

/*
* According to Q&A Q6, we need to implement
* the following, so it indicates if a process
* is terminated outside of PMan. It uses waitpid
* and NOHANG to check if the process is terminated.
*/
void processTerminated() {
    int retVal;
    pid_t pid;
    while ((pid = waitpid(-1, &retVal, WNOHANG)) > 0) {
        if (WIFEXITED(retVal)) {
            printf("Terminated Process %d with exit status %d\n", pid, WEXITSTATUS(retVal));
			head = deleteNode(head, pid);
        } else if (WIFSIGNALED(retVal)) {
            printf("Terminated Process %d by signal %d\n", pid, WTERMSIG(retVal));
			head = deleteNode(head, pid);
        }
    }
}

 
int main(){
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
    }

  return 0;
}

