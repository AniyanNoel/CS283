1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is useful for a couple of reasons, such as reading commands line by line, having built-in buffer overflow protection, handling EOF efficiently, and preserving the newline character.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: Using malloc offers flexible buffer sizes, enables memory to be allocated only when needed, and allows for buffer resizing.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces helps with command recognition, parsing arguments, pipe processing, and user experience. Some systems can't recognize commands with spacing differences which leads to execution failures. Additionally, extra spaces can be interpreted as empty arguments. This also helps flags to be processed with or without spaces.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Three redirection examples to implement are output, input, and error redirection. Challenges that can arise from them include the need to handle creation with proper permissions for output, verifying file existence for input redirection, and needing to parse the file descriptor number for error redirection.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection connects commands with files. It is about moving data between commands and files. Pipes move data between commands, creating a producer-consumer relationship between commands. Think of pipes like Mario warp pipes—they take something in and feed it somewhere else. Another distinction is that redirection involves disk I/O, while piping happens in memory.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to separate the two for error handling, as it allows programs to distinguish output and errors. A couple of other reasons include script processing and debugging issues.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  I think that if a command fails, the command's exit status should be returned, error redirection should take place, and stderr should go into stdout while preserving the original error message from failed commands. Finally, a shell-specific message should be sent as context for the error. In terms of merging STDOUT and STDERR, separate buffers should be used for each by default, and proper ordering of output should be prioritized.