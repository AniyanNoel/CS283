1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    The child processes would become zombie processes and consume resources until the parent kills them. The system could also run out of process slots.


2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    If pipes remain open, the processes may not receive an EOF signal. They also waste pipe descriptors and can lead to data corruption.


3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    The working directory change would only occur within the child process and not affect the shell's environment. Since the shell would remain in the same directory when the child process terminates, the cd command would be ineffective as well.


4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    In terms of modification, I would try to dynamically allocate memory and use realloc to allocate and free memory. Some trade-offs would include an increase in complexity due to edge cases and potential inefficiencies in memory management.

