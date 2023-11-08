# Linux-shell-in-c

## Installation
* Open Linux terminal
* Go to the directory where myshell.c is present
* Run gcc myshell.c -o myshell
* Finally execute ./myshell

## Features

- ### Basic Stuff
    The shell runs an infinite loop (which will only exit with 
    the ‘exit’ command) and interactively process user commands. 
    The shell prints a prompt that indicates the current working 
    directory followed by a ‘$’ character.

- ### Changing directory
    The cd command is supported

- ### Incorrect command
    An incorrect command format (which the shell is unable to process)
    prints an error message ‘Shell: Incorrect command’.
    If the shell is able to execute the command, but the execution 
    results in error messages generation, those error messages 
    will be displayed on the terminal.

- ### Signal handling
    The behavior of both the signals (Ctrl C, Ctrl Z) is the same
    i.e. both kill the process if running in the terminal 
    does not have any effect in myshell.c

- ### Executing multiple commands
    Shell supports multiple command execution for sequential 
    execution as well as for parallel execution. The commands 
    separated by `‘&&’` are executed in parallel and those 
    separated by `‘##’` are executed sequentially. Also, the shell waits 
    for all the commands to be terminated (for parallel and 
    sequential executions,) before accepting further inputs. 
    Simultaneous use of `‘&&’` and `‘##’` is **not** supported.

- ### Output redirection
    Shell redirects STDOUT for the commands using the `‘>’` symbol. 
    For example, ‘ls > info.out’ should write the output of ‘ls’ 
    command to ‘info.out’ file instead of writing it on screen. 
    Again, simultaneous use of multiple commands and output 
    redirection is **not** supported.


## Development Responsibility
This project is developed and maintained by 
[@Afsah Khan](https://github.com/Afsah-Adeeb).
