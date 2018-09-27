#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// parse the command line and return in an array of tokens with a trailing (char*)0
// example
//   line="ls -al foo"
//   return={"ls", "-al", "foo", (char*)0}
char** getCommandLine(const char *line)
{
    char **ppCmd = NULL;
    int cTokens = 0;
    const char delims[] = " \t\n";

    // first count the number of tokens
    // note that strtok will write on your original line buffer, 
    // so we use a temp duplicated copy, don't forget to free it
    char *dupLine = strdup(line);
    if (dupLine)
    {
        char *pToken = strtok(dupLine, delims);
        while (pToken != NULL)
        {
            cTokens++;
            pToken = strtok(NULL, delims);
        }

        // free the temp copy
        free(dupLine);
    }

    // allocate memory, add one token for the trailing '(char*)0'
    ppCmd = malloc((cTokens+1) * sizeof(char*));
    if (ppCmd)
    {
        int iToken = 0;
        char *pToken = strtok(line, delims);
        while (pToken != NULL)
        {
            ppCmd[iToken] = pToken;
            iToken++;
            pToken = strtok(NULL, delims);
        }

        // set the trailing '(char*)0'
        ppCmd[iToken] = (char*)0;
    }

    return ppCmd;
}

int doCommand(char **ppCmd)
{
    pid_t pID;
            
    // is it an internal command "exit" or "cd"?
    if (!strcmp(ppCmd[0], "exit"))
    {
        printf("Exiting TheOneShell...\n\n");
        exit(0);
    }
    else if (!strcmp(ppCmd[0], "cd"))
    {
		char cwd[256];
        getcwd(cwd, sizeof(cwd));
		if (ppCmd[1]){
			char *cmd = ppCmd[1];
			if (cmd[0] != '/'){
				char *dir = strcat(cwd, "/");
				dir = strcat(dir, ppCmd[1]);
				chdir(dir);
			} else {
				chdir(ppCmd[1]);
			}
		} else {
			chdir(getenv("HOME"));        
		}
        return 0;
    }
    else
    {
        // execute the external command
        pID = fork();
        if (pID == 0)
        {
            // child process
            // add code here: change execlp to execvp to include command line arguments
            execvp(ppCmd[0], ppCmd);

            // this following instruction will only happens if the exec failed
            printf(ANSI_COLOR_RED "exec failed\n");
            
            // add code here: what do you do if the child process failed to launch the command?
            //      for example, user types "lkasjdhflakjh" which is an invalid command
			printf(ANSI_COLOR_RED "%s is not a valid command\n" ANSI_COLOR_RESET, ppCmd[0]);
			return 1;
        }
        else if (pID < 0)
        {
            // fork failed
            printf(ANSI_COLOR_RED "fork failed\n" ANSI_COLOR_RESET);
            return 2;
        }
        else
        {
            // this is the parent process
            // wait for child process to finish
            // add code here: use waitpid system call
            waitpid(pID,0,0);
        }
    }

    return 0;
}

// main function 
int main(int argc, char* argv[])
{
    size_t n;
    char *line = NULL;
    int i;
    int result = 0;

    while (result == 0){
    
		char *dupLine = NULL;
		char cwd[256];
        getcwd(cwd, sizeof(cwd));
        char *home = strstr(cwd, getenv("HOME"));
        int size = strlen(cwd) - strlen(getenv("HOME"));
        if (home){
			printf(ANSI_COLOR_CYAN "\nTheOneShell" ANSI_COLOR_GREEN ":~%s> " ANSI_COLOR_RESET, home + strlen(home) - size);
		} else {
			printf(ANSI_COLOR_CYAN "\nTheOneShell" ANSI_COLOR_GREEN ":%s> " ANSI_COLOR_RESET, cwd);
		}
		ssize_t cc = (ssize_t)getline(&line, &n, stdin);
		if (line)
		{
			char **ppCmd = getCommandLine(line);
			if (!ppCmd)
			{
				// getCommandLine failed to allocate memory
				// add code here: exit the loop
				result = 2;
				continue;
			}
			
			if (ppCmd[0] == '\0') { 
				continue; 
			}

			result = doCommand(ppCmd);
			free(ppCmd);
		} // end if
		
	}
	
	if (line)
		free(line);

    return result;
}
