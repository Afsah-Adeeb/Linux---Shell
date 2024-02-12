#include <stdio.h>
#include <string.h>
#include <stdlib.h>   // exit()
#include <unistd.h>   // fork(), getpid(), exec()
#include <sys/wait.h> // wait()
#include <signal.h>   // signal()
#include <fcntl.h>    // close(), open()

#define BUFSIZE 100  // Define buffer size for input
#define CWD_SIZE 100 // Define buffer size for current working directory
#define PATH_MAX 100 // Define maximum path length

void changeDirectory(char **tokens)
{
    if (tokens[1] != NULL)
    {
        // Change directory using chdir() system call
        if (chdir(tokens[1]) != 0)
        {
            // Print error message if directory change fails
            printf("Shell: Incorrect command\n");
        }
    }
}

char **parseInput(char *input)
{
    int pos = 0;
    int buf_size = BUFSIZE;

    // Allocate memory for array of tokens
    char **tokens = malloc(buf_size * sizeof(char *));
    char *tok;
    while ((tok = strsep(&input, " ")) != NULL)
    {
        if (strlen(tok) == 0)
        {
            continue;
        }
        tokens[pos++] = tok; // Store token in array
    }

    tokens[pos] = NULL; // Mark end of array with NULL
    return tokens;      // Return array of tokens
}

void executeCommand(char **tokens)
{
    // This function will fork a new process to execute a command
    if (strcmp(tokens[0], "cd") == 0)
    {
        // Handle change directory command
        changeDirectory(tokens);
    }
    else
    {
        int rc = fork(); // Fork a new process

        if (rc < 0)
        {
            // Forking failed
            exit(1);
        }
        else if (rc == 0)
        {
            // Child process

            // Restore default signal handling for Ctrl+C and Ctrl+Z
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            // Execute command in child process
            if (execvp(tokens[0], tokens) == -1)
            {
                // Print error message if command execution fails
                printf("Shell: Incorrect command\n");
                exit(1);
            }
        }
        else
        {
            // Parent process
            int rc_wait = wait(NULL); // Wait for child process to finish
        }
    }
}

void executeSequentialCommands(char **tokens)
{
    // This function will run multiple commands in sequence
    int start = 0;
    int i = 0;
    while (tokens[i])
    {
        while (tokens[i] && strcmp(tokens[i], "##") != 0)
        {
            i++;
        }

        tokens[i] = NULL;
        executeCommand(&tokens[start]); // Execute command
        i++;
        start = i;
    }
}

void executeParallelCommands(char **tokens)
{
    // This function will run multiple commands in parallel
    int r;
    int i = 0;
    int start = 0;
    int process_count = 0;

    while (tokens[i])
    {
        while (tokens[i] && strcmp(tokens[i], "&&") != 0)
        {
            i++;
        }

        process_count++;
        tokens[i] = NULL;
        if (strcmp(tokens[start], "cd") == 0)
        {
            // Handle change directory command
            changeDirectory(&tokens[start]);
        }
        else
        {
            r = fork(); // Fork a new process
            if (r < 0)
            {
                // Forking failed
                exit(1);
            }
            else if (r == 0)
            {
                // Child process

                // Restore default signal handling for Ctrl+C and Ctrl+Z
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);

                // Execute command in child process
                if (execvp(tokens[start], &tokens[start]) == -1)
                {
                    // Print error message if command execution fails
                    printf("Shell: Incorrect command\n");
                    process_count--;
                    exit(0);
                }
            }
        }
        i++;
        start = i;
    }

    // Parent process
    while (process_count)
    {
        int ret = wait(NULL); // Wait for child processes to finish
        process_count--;
    }
}

void executeCommandRedirection(char **tokens)
{
    // This function will run a single command with output redirected to an output file specified by user
    int total_tok = 0;

    // Count total number of tokens
    for (int i = 0; tokens[i] != NULL; i++)
    {
        total_tok++;
    }

    int r = fork(); // Fork a new process

    if (r < 0)
    {
        // Forking failed
        exit(1);
    }
    else if (r == 0)
    {
        // Child process

        // Restore default signal handling for Ctrl+C and Ctrl+Z
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // Create or empty file
        fclose(fopen(tokens[total_tok - 1], "w"));

        // Redirect STDOUT to output file
        close(STDOUT_FILENO);
        int f = open(tokens[total_tok - 1], O_CREAT | O_WRONLY | O_APPEND);

        tokens[total_tok - 1] = tokens[total_tok - 2] = NULL;

        // Execute command in child process
        if (execvp(tokens[0], tokens) == -1)
        {
            // Print error message if command execution fails
            printf("Shell: Incorrect command\n");
            exit(1);
        }

        // Close file descriptor
        fflush(stdout);
        close(f);
    }
    else
    {
        // Parent process
        int rc_wait = wait(NULL); // Wait for child process to finish
    }
}

char cwd[PATH_MAX]; // stored the location of current directory

void sigHandler(int sig)
{
    printf("\n");
    printf("%s$", cwd); // Print current working directory
    fflush(stdout);
    return;
}

int main()
{

    // Initial declarations
    signal(SIGTSTP, &sigHandler); // Set signal handler for Ctrl+Z
    signal(SIGINT, &sigHandler);  // Set signal handler for Ctrl+C

    while (1) // This loop will keep your shell running until user exits.
    {
        // Print the prompt in format - currentWorkingDirectory$
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s$", cwd); // Print current working directory
        }

        // Accept input with 'getline()'
        char *input = NULL;
        size_t size = 0;

        int byte_read = getline(&input, &size, stdin); // Read input
        int len = strlen(input);
        input[len - 1] = '\0'; // Replace newline character with null terminator

        // Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
        char **args = parseInput(input); // Parse input into tokens

        if (strcmp(args[0], "exit") == 0)
        {
            // When user uses exit command.
            printf("Exiting shell...\n");
            break; // Exit the while loop and terminate the shell
        }

        int c = 0;
        for (int i = 0; args[i] != NULL; i++)
        {

            if (strcmp(args[i], "&&") == 0)
            {
                // Parallel execution
                c = 1;
                break;
            }
            else if (strcmp(args[i], "##") == 0)
            {
                // Sequential execution
                c = 2;
                break;
            }
            else if (strcmp(args[i], ">") == 0)
            {
                // Redirection execution
                c = 3;
                break;
            }
        }
        if (c == 1)
        {
            // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
            executeParallelCommands(args);
        }
        else if (c == 2)
        {
            // This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
            executeSequentialCommands(args);
        }
        else if (c == 3)
        {
            // This function is invoked when user wants redirect output of a single command to an output file specified by user
            executeCommandRedirection(args);
        }
        else
        {
            // Simple command
            executeCommand(args);
        }
    }
    return 0;
}
