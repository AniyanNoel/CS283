#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"
 
static int last_return_code = 0; // store the exit code from the last child process

int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;

    // 1) Allocate command buffer
    if (alloc_cmd_buff(&cmd) != OK) {
        return ERR_MEMORY;
    }

    while (1) {
        // 2) Prompt and read a line from user
        printf("%s", SH_PROMPT);       
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            // We got EOF (Ctrl+D) or an input error
            printf("\n");
            break;
        }

        // 3) Strip trailing newline
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        // 4) Clear old command state
        clear_cmd_buff(&cmd);

        // 5) Parse input into cmd_buff
        rc = build_cmd_buff(cmd_line, &cmd);
        if (rc != OK) {
            if (rc == WARN_NO_CMDS) {
                printf(CMD_WARN_NO_CMD);
                continue;
            }
            // any other error breaks out
            break;
        }

        // 6) Check built-in commands: exit, cd, rc, etc.
        Built_In_Cmds bi_rc = exec_built_in_cmd(&cmd);
        if (bi_rc == BI_CMD_EXIT) {
            // exit command => exit the while loop
            break;
        }
        if (bi_rc != BI_NOT_BI) {
            // If a built-in was executed (like `cd` or `rc`), skip fork/exec
            continue;
        }

        // 7) Not built in => fork & execvp
        pid_t pid = fork();
        
        if (pid < 0) {
            // Fork failed
            perror("fork");
            continue;
        }
        
        if (pid == 0) {
            // CHILD process
            execvp(cmd.argv[0], cmd.argv);
            // If we get here, execvp failed:
            fprintf(stderr, "Failed to execute '%s': ", cmd.argv[0]);
            if (errno == ENOENT) {
                fprintf(stderr, "Command not found in PATH (PATH=%s)\n", getenv("PATH"));
            } else if (errno == EACCES) {
                fprintf(stderr, "Permission denied\n");
            } else {
                perror("execvp");
            }
            exit(errno);
        } else {
            // PARENT process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_return_code = WEXITSTATUS(status);
            } else {
                last_return_code = -1; 
            }
        }
    }

    // 8) Free the command buffer and exit
    free_cmd_buff(&cmd);
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    // Check for "exit"
    if (strcmp(cmd->argv[0], EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }

    // Check for "cd"
    if (strcmp(cmd->argv[0], "cd") == 0) {
        // cd with no arguments => do nothing
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                last_return_code = errno;
            } else {
                last_return_code = 0;
            }
        }
        return BI_EXECUTED;
    }

    // Check for "rc" (extra credit)
    if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_return_code);
        return BI_EXECUTED;
    }

    // If we reach here, it's not a built-in
    return BI_NOT_BI;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    /*
     * This approach handles spaces, single quotes, and double quotes (somewhat).
     * It stores tokens in cmd_buff->_cmd_buffer, then sets cmd_buff->argv[] pointers.
     */
    char *token;
    char *str = cmd_line;
    int in_quotes = 0;
    char quote_char = '\0';
    
    // Skip leading whitespace
    while (*str && isspace((unsigned char)*str)) str++;
    if (*str == '\0') {
        return WARN_NO_CMDS;  // empty line
    }

    cmd_buff->argc = 0;
    token = cmd_buff->_cmd_buffer;

    while (*str && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        // Check if we’re starting or ending a quoted section
        if (*str == '"' || *str == '\'') {
            if (!in_quotes) {
                // Beginning quotes
                in_quotes = 1;
                quote_char = *str;
                str++;
                continue;
            } else if (*str == quote_char) {
                // Ending the same type of quote
                in_quotes = 0;
                str++;
                continue;
            }
        }

        // If we’re not in quotes, and we see a space => end of a token
        if (!in_quotes && isspace((unsigned char)*str)) {
            // Mark the end of this token with a null terminator
            *token = '\0';
            cmd_buff->argv[cmd_buff->argc++] = cmd_buff->_cmd_buffer +
                (token - cmd_buff->_cmd_buffer - strlen(token));
            
            // Advance token pointer
            token++;
            // Skip the trailing spaces before next token
            while (isspace((unsigned char)*str)) str++;
            continue;
        }

        // Otherwise, copy character into token buffer
        *token++ = *str++;
    }

    // If we have leftover characters in token buffer, we close out one last token
    if (token != cmd_buff->_cmd_buffer) {
        *token = '\0';
        cmd_buff->argv[cmd_buff->argc++] = cmd_buff->_cmd_buffer + 
            (token - cmd_buff->_cmd_buffer - strlen(token));
    }

    // Null-terminate argv
    cmd_buff->argv[cmd_buff->argc] = NULL;

    // If we ended up with no tokens, treat it as WARN_NO_CMDS
    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    // Allocate a buffer for building tokens
    cmd_buff->_cmd_buffer = (char *)malloc(ARG_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    
    cmd_buff->argc = 0;
    cmd_buff->argv[0] = NULL;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    // Reset argc and clear argv pointers
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    
    // Clear the memory to avoid stale data
    if (cmd_buff->_cmd_buffer != NULL) {
        memset(cmd_buff->_cmd_buffer, 0, ARG_MAX);
    }
    return OK;
}
