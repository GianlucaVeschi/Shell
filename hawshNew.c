// C Program to design a shell in Linux 
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 
#include <stdbool.h>
  
#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 
  
 
// Greeting shell during startup 
void init_shell_help() 
{ 
    puts("\n\n********************************************************"
        "\n WELCOME TO THE HAW SHELL                             "
        "\n List of Commands supported:                          "
        "\n >/  genau wie (cd)                                   "                                              
        "\n >ls                                                  "
        "\n >version                                             "
        "\n >exit                                                " 
        "\n >all other general commands available in UNIX shell  "
        "\n********************************************************\n");    
}
// Version and Autor
void version_and_autor()
{
	char* username = getenv("USER"); 
    printf("\n\nUSER is: @%s", username); 
    printf("\n");

	puts("\n"
		 "HAW-Shell Version 1.0 Autors : Gianluca , Jasmin "	
		 "\n");
}
// Function to take input 
int takeInput(char* str) 
{ 
    char* buf; 
  
    buf = readline("\n>>> "); 
    if (strlen(buf) != 0) { 
        add_history(buf); 
        strcpy(str, buf); 
        return 0; 
    } else {
        printf("\n nothing has been entered"); 
        return 1; 
    } 
} 
  
// Function to print Current Directory. 
void printDir() 
{ 
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("\nDir: %s", cwd); 
} 
//check that input is a path
int isAbsolute(const char *str)
{
    return (str[1] == '/');
}

//change directory
void changeDirectory(char* path){
	printf("\nAccessing: %s",path); //accessing the given path
	/*
	The chdir() function causes the path argument to become the 
	current working directory; that is, the starting point 
	for path searches for pathnames not beginning with '/' .
	*/
	if(chdir(path)== 0){//succesfull completion
		printDir(); // prove that we accessed the path correctly
	}
	else{
		/*EACCES error Search permission is denied for any component 
		of the pathname. */
		printf("\n error accessing the directory %s",path);
	}
}

bool endsWith(char* base, char* str)
{
	int blen = strlen(base);
	int slen = strlen(str);
	if (slen <= blen)	{
		return (0 == strcmp(base + blen - slen, str));
	}
	return false;
}

bool isBackround(char* inputString){
	return endsWith(inputString,"&");
}
  
// Function where the system command is executed 
void execArgs(char* command,int isForeground) {   
	//variables
    pid_t pid;
    pid_t wpid;
	int status;

    if(isForeground){
		pid = fork();  //duplicate process 
		if(pid == 0){
			//child process
            status = execlp(command,command,NULL);//replace child process with command
			if(status == -1){
				perror("couldn't run command");
			}
			exit(EXIT_FAILURE);
		} else if (pid < 0){
			perror("error forking");
		} else {
			//parent process
			/*anstsRussia
			int stat 
			waitpid(pid,&stat,0);
			*/
			waitpid(pid,&status,0);
		
		}
	}
    else{//if it's background
		pid = fork();  
		if(pid == 0){
			//child process
			status = execlp(command,command,NULL);
			if( status == -1){
				perror("couldn't run command");
				exit(0);
			}
			exit(0);
		} else if (pid < 0){
			perror("error forking");
		} else {
			//parent process
				printf("Executing %s in background",command);
		}
	}
    
} 
  
int ownCmdHandler(char* command) { 
    int NoOfOwnCmds = 5;
    int switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  	
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "/"; // genau wie cd 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "hello"; 
    ListOfOwnCmds[4] = "version";
  
    for (int i = 0; i < NoOfOwnCmds; i++) {
		if(isAbsolute(command)){
			switchOwnArg = 2; // the command to be executed is cd
			break;
		}
        if (strcmp(command, ListOfOwnCmds[i]) == 0) { //compare the strings
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        printf("\nGoodbye\n"); 
        exit(0); 
    case 2: 
		changeDirectory(command); 
        return 1; 
    case 3: 
        init_shell_help(); 
        return 1; 
    case 4: 
        username = getenv("USER"); 
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n", 
            username); 
        return 1; 
    case 5:
    	version_and_autor();
    	return 1;   
    default: //not my own command
   		return 0;
        break; 
    } 
    return 0; 
} 

int main() { 
    char inputString[MAXCOM];   
    int execFlag = 0;
    int executeInForeground;  
    init_shell_help(); 
  	
    while (1) {             
        // take input 
        if (takeInput(inputString)) 
            continue; 
 		executeInForeground = 1;   
        if (isBackround(inputString)){
        	//remove last character
            size_t len = strlen(inputString);
            inputString[len-1] = '\0'; 
        	execFlag = -1;   
            executeInForeground = 0; // the process has to be run in background  
        }
        
       if (!ownCmdHandler(inputString)) {
        	//system(inputString); //how can I replace this?
       		execArgs(inputString, executeInForeground);
       } 
    } 
    return 0; 
}