1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Fork creates a new process, whereas execvp replaces the current process with the new program. Fork is important because it causes the shell not to be terminated. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  -1 will be returned if fork() fails. To prevent the shell from crashing, an error message is returned and the commands continue onward.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for executables in the directories listed in PATH env variable and automatically looks through each directory in PATH until a matching executable name.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() prevents zombie processes by collecting the child's exit status. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXISTATUS() extracts exit codes from the status value returned by wait(). It is stored in last_return_code for the 'rc' command which lets us know if the command succeeded or failed

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  Since shell commaands often need to treat quoted stirngs as a single arg, preserving internal spacing is important so we want to be able to handle quoted args by tracking it using in_quotes and quote_chars.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: We were to remove pipe handling, and focus on single commands. We cahnged the parsing logic and simplified it to work with comd_buff_t instead of making a command list. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals are lightweight and don't transfer data since they're mainly for process control and notification. This is what makes them differ from IPCs.


- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  (1)SIGTERM graceful termination request, (2)SIGKILL immediate termination...can not be caught, (3)SIGNT interactive interupt...same as Ctrl C.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP suspends process execution until SIGCONT is recieved. SIGSTOP ensures that processes can always be stopped by the system.