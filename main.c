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
void processTerminated();

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
			printf("Error, %s failed to execute\n", cmd[1]);
			exit(-1);
			processTerminated();
	}else{ //if the pid is bigger than 0, the current process is the parent process
		int status = 0; 
		pid_t pid2 = waitpid(pid, &status, WNOHANG);
		if(pid2 == 0){ //Child process is running so we can add it to the linked list		
			head = add_newNode(head, pid, cmd[1]); //use the createNode function made in the linked_list program
			printf("PID %d started\n", pid); //print the pid number
		}else if (pid2 == pid){ //Child process is terminated so check the status
			if(WIFEXITED(status) || WIFSIGNALED(status)){ //check the status of the child process
				printf("Child process %d terminated\n", pid); //print the terminated child process
				head = deleteNode(head,pid); //delete the terminated child process from the linked list
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
	if(head == NULL){ //check if list is empty
		printf("No Processes Running\n"); 
	}else{
		printList(head); //otherwise print the entire list
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
	int retVal = kill(pid, SIGTERM); //get the return value
	if(retVal == -1){ //if it is -1, there was an error killing the process
		printf("Error Killing Process\n");
	}else if(retVal == 0){ //if it is 0, the process was killed successfully
		head = deleteNode(head,pid); //remove the node
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
	int retVal = kill(pid,SIGSTOP); //get the return value
	if(retVal == -1){ //if it is -1, there was an error killing the process
		printf("Error Stopping Process\n");
	}else if(retVal == 0){ //if it is 0, the process was killed successfully
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
	int retVal = kill(pid,SIGCONT); //get the return value
	if(retVal == -1){ //if it is -1, there was an error killing the process
		printf("Error Starting Process");
	}else if(retVal == 0){ //if it is 0, the process was killed successfully
		printf("Started Process %d\n", pid);
	}
}

/*
* A Helper function for func_pstat as it
* is repeated to find the Uptime and stime
*/
void funcTime(char * str_pid){
	char path[256]; //create a path so we can format str_pid into a /proc/pid/status
	strcpy(path, "/proc/");
	strcat(path, str_pid);
	strcat(path, "/stat");
	FILE *stringPath = fopen(path, "r"); //open the path file
	if(stringPath == NULL){ //error handling if the path is empty
		printf("Process %s does not exist\n", str_pid);
		return;
	}
	char line[512]; //create a line to read the file line by line
	if(fgets(line, sizeof(line), stringPath)){ //read the file line by line
		char* token = strtok(line, " "); //use strtok to split the line by spaces
		int count = 0;
		while(token != NULL){ //iterate through the line
			count++;
			if(count == 14){ //The 14th token is utime
				printf("Utime: %s\n", token);
			}else if(count == 15){ //The 15th token is stime
				printf("Stime: %s\n", token);
				return;
			}
			token = strtok(NULL, " "); //traverse through the line
		}
	}
	fclose(stringPath);
}


/*
* According to the Assignment specifications,
* we need to implement the pstat function to
* print the following information about a process:
* 1. The command that started the process
* 2. The state of the process
* 3. The amount of time the process has been scheduled in user mode
* 4. The amount of time the process has been scheduled in kernel mode
* 5. The amount of memory the process is using
* 6. The number of voluntary context switches the process has made
* 7. The number of involuntary context switches the process has been involved in
*/
void func_pstat(char * str_pid){
	char path[256]; //create a path so we can format str_pid into a /proc/pid/status
	strcpy(path, "/proc/");
	strcat(path, str_pid);
	strcat(path, "/status");
	FILE *stringPath = fopen(path, "r"); //open the path file
	if(stringPath == NULL){ //error handling if the path is empty
		printf("Process %s does not exist\n", str_pid);
		return;
	}
	char line[512]; //create a line to read the file line by line
	while(fgets(line, sizeof(line), stringPath)){ //read the file line by line
		if(strstr(line, "Name:") != NULL){ //Use substring search to find the Comm
			printf("%s", line);
		}else if(strstr(line, "State:") != NULL){ //Use substring search to find the VmSize
			printf("%s", line);
		}
	}
	fclose(stringPath); //close the path file so we can print time
	funcTime(str_pid);
	stringPath = fopen(path, "r"); //open the path file again
	while(fgets(line, sizeof(line), stringPath)){
	 	if(strstr(line, "RssAnon:") != NULL){ //Use substring search to find the rss
			printf("%s", line);
		}else if(strstr(line, "voluntary_ctxt_switches:") != NULL){ //Use substring search to find the voluntary ctxt
			printf("%s", line);
		}else if(strstr(line, "nonvoluntary_ctxt_switches:") != NULL){ //Use substring search to find the nonvoluntary ctxt
			printf("%s", line);
		}
	}
	fclose(stringPath);
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
    while ((pid = waitpid(-1, &retVal, WNOHANG)) > 0) { //check if the process is terminated
        if (WIFEXITED(retVal)) { //check if the process is exited
            printf("Terminated Process %d with exit status %d\n", pid, WEXITSTATUS(retVal)); 
			head = deleteNode(head, pid); //remove the node
        } else if (WIFSIGNALED(retVal)) { //check if the process is signaled
            printf("Terminated Process %d by signal %d\n", pid, WTERMSIG(retVal));
			head = deleteNode(head, pid); //remove the node
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

