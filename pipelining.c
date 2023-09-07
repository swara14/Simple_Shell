// You'll need to rearrange your command strings into this three dimensional array
// of pointers, but by doing so you allow an arbitrary number of commands, each with
// an arbitrary number of arguments.
int executePipe(char ***commands, int inputfd)
{
    // commands is NULL terminated
    if (commands[1] == NULL)
    {
        // If we get here there's no further commands to execute, so run the 
        // current one, and send its result back.
        pid_t pid;
        int status;
        if ((pid = fork()) == 0)
        {
            // Set up stdin for this process.  Leave stdout alone so output goes to the
            // terminal.  If you want '>' / '>>' redirection to work, you'd do that here
            if (inputfd != -1)
            {
                dup2(inputfd, STDIN_FILENO);
                close(inputfd);
            }
            execvp(commands[0][0], commands[0]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        waitpid(pid, &status, 0);
        return status;
    }
    else
    {
        // Somewhat similar to the above, except we also redirect stdout for the
        // next process in the chain
        int fds[2];
        if (pipe(fds) != 0)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid;
        int status;
        if ((pid = fork()) == 0)
        {
            // Redirect stdin if needed
            if (inputfd != -1)
            {
                dup2(inputfd, STDIN_FILENO);
                close(inputfd);
            }
            dup2(fds[1], STDOUT_FILENO);
            close(fds[1]);
            execvp(commands[0][0], commands[0]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        // This is where we handle piped commands.  We've just executed
        // commands[0], and we know there's another command in the chain.
        // We have everything needed to execute that next command, so call
        // ourselves recursively to do the heavy lifting.
        status = executePipe(++commands, fds[0]);
        // As written, this returns the exit status of the very last command
        // in the chain.  If you pass &status as the second parameter here
        // to waitpid, you'll get the exit status of the first command.
        // It is left as an exercise to the reader to figure how to get the
        // the complete list of exit statuses
        waitpid(pid, NULL, 0);
        return status;
    }
}