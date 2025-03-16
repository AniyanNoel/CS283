#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

// Add at the top of the file with other globals
static int last_return_code = 0;

int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;

    if (alloc_cmd_buff(&cmd) != OK) {
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        clear_cmd_buff(&cmd);

        rc = build_cmd_buff(cmd_line, &cmd);
        if (rc != OK) {
            if (rc == WARN_NO_CMDS) {
                printf(CMD_WARN_NO_CMD);
                continue;
            }
            break;
        }

        Built_In_Cmds bi_rc = exec_built_in_cmd(&cmd);
        if (bi_rc == BI_CMD_EXIT) {
            break;
        }
        if (bi_rc != BI_NOT_BI) {
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            continue;
        }
        
        if (pid == 0) {
            execvp(cmd.argv[0], cmd.argv);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_return_code = WEXITSTATUS(status);
            } else {
                last_return_code = -1;
            }
        }
    }

    free_cmd_buff(&cmd);
    return 0;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                last_return_code = 1;
            } else {
                last_return_code = 0;
            }
        }
        return BI_EXECUTED;
    }

    if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_return_code);
        return BI_EXECUTED;
    }

    return BI_NOT_BI;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    char *str = cmd_line;
    int in_quotes = 0;
    char quote_char = '\0';
    char *token_start = NULL;
    
    while (*str && isspace(*str)) {
        str++;
    }
    
    if (*str == '\0') {
        return WARN_NO_CMDS;
    }

    cmd_buff->argc = 0;
    char *buffer_pos = cmd_buff->_cmd_buffer;
    
    while (*str && cmd_buff->argc < CMD_ARGV_MAX - 1) {
        if (*str == '"' || *str == '\'') {
            if (!in_quotes) {
                in_quotes = 1;
                quote_char = *str;
                if (!token_start) {
                    token_start = buffer_pos;
                }
                str++;
                continue;
            } else if (*str == quote_char) {
                in_quotes = 0;
                str++;
                continue;
            }
        }

        if (!in_quotes && isspace(*str)) {
            if (token_start) {
                *buffer_pos = '\0';
                cmd_buff->argv[cmd_buff->argc] = token_start;
                cmd_buff->argc++;
                buffer_pos++;
                token_start = NULL;
            }
            str++;
            continue;
        }

        if (!token_start) {
            token_start = buffer_pos;
        }
        *buffer_pos++ = *str++;
    }

    if (token_start) {
        *buffer_pos = '\0';
        cmd_buff->argv[cmd_buff->argc] = token_start;
        cmd_buff->argc++;
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    if (cmd_buff->argc > 0) {
        return 0;
    } else {
        return WARN_NO_CMDS;
    }
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = (char *)malloc(ARG_MAX);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    cmd_buff->argc = 0;
    cmd_buff->argv[0] = NULL;
    
    return 0;
}

int free_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    return 0;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    if (!cmd_buff) {
        return -1;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    
    if (cmd_buff->_cmd_buffer != NULL) {
        memset(cmd_buff->_cmd_buffer, 0, ARG_MAX);
    }
    
    return 0;
}