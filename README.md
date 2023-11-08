# Linux-shell-in-c

## Installation
* Open Linux terminal
* Go to the directory where myshell.c is present
* Run gcc myshell.c -o myshell
* Finally execute ./myshell

## Features

- ### Basic Stuff
    The shell run an infinite loop (which will only exit with 
    the ‘exit’ command) and interactively process user commands. 
    The shell print a prompt that indicate the current working 
    directory followed by ‘$’ character.

- ### Changing directory
    The cd command is supported

- ### Incorrect command
    An incorrect command format (which shell is unable to process)
    prints an error message ‘Shell: Incorrect command’.
    If shell is able to execute the command, but the execution 
    results in error messages generation, those error messages 
    will be displayed to the terminal.

- ### Signal handling
    The behaviour of both the signals (Ctrl C, Ctrl Z) are same
    i.e. both kills the process if running in the terminal else
    it does not have any effect in myshell.c

- ### Executing multiple commands
    Shell supports multiple command execution for sequential 
    execution as well as for parallel execution. The commands 
    separated by `‘&&’` are executed in parallel and the commands 
    separated by `‘##’` are executed sequentially. Also shell waits 
    for all the commands to be terminated (for parallel and 
    sequential executions, both) before accepting further inputs. 
    Simultaneous use of `‘&&’` and `‘##’` is **not** supported.

- ### Output redirection
    Shell redirects STDOUT for the commands using `‘>’` symbol. 
    For example, ‘ls > info.out’ should write the output of ‘ls’ 
    command to ‘info.out’ file instead of writing it on screen. 
    Again, simultaneous use of multiple commands and output 
    redirection is **not** supported.


## Development Responsibility
This project is developed and maintained by 
[@Hasan Koser](https://www.github.com/HASH-002).