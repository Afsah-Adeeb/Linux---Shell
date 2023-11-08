#include <stdio.h>
#include <string.h>
#include <stdlib.h>	  // exit()
#include <unistd.h>	  // fork(), getpid(), exec()
#include <sys/wait.h> // wait()
#include <signal.h>	  // signal()
#include <fcntl.h>	  // close(), open()

#define PATHLENGTH 10000 // Max size of user command
#define MAXSIZE 1000	 // Max size of command after applying delimiter

// GLOBALS
int cur_size;		// size of commands string returned from parseInput function
pid_t forkChildPID; // Child id used in signal Handling

// This function terminated the use of Ctrl+C and Ctrl+Z in parent process and uses them in Child process
void sighandler(int sig_num)
{
	if (forkChildPID != 0)
	{
		kill(forkChildPID, SIGKILL);
	}
}

/* This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter 
(&&, ##, >, or spaces). */
int parseInput(char *original, char **commands)
{
	int i, ret_val = 0;
	if (strstr(original, "&&") != NULL)
	{
		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, "&&");

			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		ret_val = 1;
		cur_size = i;
	}
	else if (strstr(original, "##") != NULL)
	{
		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, "##");

			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		ret_val = 2;
		cur_size = i;
	}
	else if (strstr(original, ">") != NULL)
	{
		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, ">");

			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		ret_val = 3;
		cur_size = i;
	}
	else
	{
		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, " ");

			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		ret_val = 4;
	}

	if (i == 0)
		return 0;

	return ret_val;
}

// This function will fork a new process to execute a command except cd and exit command
void executeCommand(char **str)
{
	if (strcmp(str[0], "exit") == 0)
	{
		printf("Exiting shell...\n");
		exit(0);
	}
	else if (strcmp(str[0], "cd") == 0)
	{
		int result;

		if (str[1] == NULL)
			result = chdir(getenv("HOME"));
		else
			result = chdir(str[1]);

		if (result != 0 && strcmp(str[1], ".") != 0)
			printf("Shell: Incorrect command\n");
	}
	else
	{
		forkChildPID = fork();

		if (forkChildPID < 0)
		{
			printf("Shell: Incorrect command\n"); //fork unsuccessful
			exit(0);
		}
		else if (forkChildPID == 0)
		{
			execvp(str[0], str);

			// If execvp fails exit child process
			printf("Shell: Incorrect command\n");
			exit(0);
		}
		else
		{
			wait(NULL);
		}
	}
}

// This function will run multiple commands in parallel
void executeParallelCommands(char **str)
{
	int total_waits = 0;

	for (int k = 0; k < cur_size; k++)
	{
		char *commands[MAXSIZE];
		char *original = strdup(str[k]);
		int i;

		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, " ");
			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		if (i == 0)
			continue;

		total_waits++;

		// When user uses exit command.
		if (strcmp(commands[0], "exit") == 0)
		{
			printf("Exiting shell...\n");
			exit(0);
		}
		else if (strcmp(commands[0], "cd") == 0)
		{
			int result;

			if (commands[1] == NULL)
				result = chdir(getenv("HOME"));
			else
				result = chdir(commands[1]);

			if (result != 0 && strcmp(commands[1], ".") != 0)
				printf("Shell: Incorrect command\n");
		}
		else
		{
			forkChildPID = fork();

			if (forkChildPID < 0)
			{
				printf("Shell: Incorrect command\n");
				exit(0);
			}
			else if (forkChildPID == 0)
			{
				execvp(commands[0], commands);

				printf("Shell: Incorrect command\n");
				exit(0);
			}
		}
	}

	for (int i = 0; i < total_waits; i++)
		wait(NULL); // waiting for every process to terminated; wait will return random child id after there termination
}

// This function will run multiple commands sequentially
void executeSequentialCommands(char **str)
{
	for (int k = 0; k < cur_size; k++)
	{
		char *commands[MAXSIZE];
		char *original = strdup(str[k]);
		int i;

		for (i = 0; i < MAXSIZE; i++)
		{
			commands[i] = strsep(&original, " ");

			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) == 0)
				i--;
		}

		if (i == 0)
		{
			continue;
		}

		executeCommand(commands);
	}
}

// This function will run a single command with output redirected to an output file specificed by user
void executeCommandRedirection(char **str)
{
	int i = 0;
	char *commands[MAXSIZE];
	for (int k = 0; k < cur_size; k++)
	{
		char *original = strdup(str[k]);

		while (i < MAXSIZE)
		{
			commands[i] = strsep(&original, " ");
			if (commands[i] == NULL)
				break;
			if (strlen(commands[i]) != 0)
				i++;
		}
	}

	int n = i;

	forkChildPID = fork();

	if (forkChildPID < 0)
	{
		printf("Shell: Incorrect command\n");
		exit(0);
	}
	else if (forkChildPID == 0)
	{
		close(STDOUT_FILENO); // Redirecting STDOUT
		open(commands[n - 1], O_CREAT | O_RDWR | O_APPEND);

		commands[n - 1] = NULL; // making name null as it is not required
		execvp(commands[0], commands);

		printf("Shell: Incorrect command\n");
		exit(0);
	}
	else
	{
		wait(NULL);
	}
}

int main()
{
	// Ignore SIGINT signal (CTRL+C) and SIGTSTP signal (CTRL+Z) in parent
	signal(SIGINT, sighandler);
	signal(SIGTSTP, sighandler);

	// Initial declarations
	char currentWorkingDirectory[PATHLENGTH];
	char *check = NULL;

	char *input = NULL; // string recieved from user
	char *retval = NULL;
	size_t size = 0;

	char *original;
	char *commands[MAXSIZE];

	int flag;
	while (1) // This loop will keep your shell running until user exits.
	{
		// getcwd() return the path of current working directory defined in unistd.h library
		check = getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)); 
		
		if (check != NULL)
		{
			printf("%s$",currentWorkingDirectory); // Print the prompt in format - currentWorkingDirectory$
		}
		else
		{
			printf("Shell: Incorrect command\n"); // Too long path cannot be displayed...
		}

		// accept input with 'getline()'
		getline(&input, &size, stdin); // input string contain delimiter also
		input = strsep(&input, "\n");  // Remove delimiter using strsep in

		if (strlen(input) == 0)
		{
			continue;
		}

		original = strdup(input); // copies in heap and give me the string

		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		flag = parseInput(original, commands);

		if (flag == 0) // Handling case of spaces
		{
			continue;
		}

		if (flag == 1)
		{
			executeParallelCommands(commands); // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		}
		else if (flag == 2)
		{
			executeSequentialCommands(commands); // This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		}
		else if (flag == 3)
		{
			executeCommandRedirection(commands); // This function is invoked when user wants redirect output of a single command to and output file specificed by user
		}
		else
		{
			executeCommand(commands); // This function is invoked when user wants to run a single commands
		}
	}

	return 0;
}