#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

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
int exec_local_cmd_loop() {
    char line[SH_CMD_MAX];
    command_list_t clist;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) {
            fprintf(stderr, CMD_WARN_NO_CMD);
            continue;
        }

        memset(&clist, 0, sizeof(clist));
        int rc = build_cmd_list(line, &clist);
        if (rc == WARN_NO_CMDS) {
            fprintf(stderr, CMD_WARN_NO_CMD);
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        } else if (rc != OK) {
            continue;
        }
        if (clist.num == 1) {
            Built_In_Cmds bic = exec_built_in_cmd(&clist.commands[0]);
            if (bic == BI_CMD_EXIT) {
                free_cmd_list(&clist);
                break;
            } else if (bic != BI_NOT_BI) {
                free_cmd_list(&clist);
                continue;
            }
        }
        rc = execute_pipeline(&clist);
    }
    free_cmd_list(&clist);
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
            chdir(cmd->argv[1]);
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    while (*cmd_line && isspace(*cmd_line)) {
        cmd_line++;
    }

    if (*cmd_line == '\0') {
        return WARN_NO_CMDS;
    }

    cmd_buff->argc = 0;
    char *dst = cmd_buff->_cmd_buffer;
    char *token_start = dst;
    int in_quotes = 0;
    char quote_char = '\0';
    long remaining_space = ARG_MAX - 1;

    while (*cmd_line && (cmd_buff->argc < CMD_ARGV_MAX - 1) && remaining_space > 0) {
        if (!in_quotes && (*cmd_line == '"' || *cmd_line == '\'')) {
            in_quotes = 1;
            quote_char = *cmd_line++;
            continue;
        }
        if (in_quotes && *cmd_line == quote_char) {
            in_quotes = 0;
            cmd_line++;
            continue;
        }
        if (!in_quotes && isspace(*cmd_line)) {
            if (dst > token_start && remaining_space > 0) {
                *dst = '\0';
                cmd_buff->argv[cmd_buff->argc] = token_start;
                cmd_buff->argc++;
                dst++;
                remaining_space--;
                token_start = dst;
            }
            while (*cmd_line && isspace(*cmd_line)) {
                cmd_line++;
            }
            continue;
        }
        *dst++ = *cmd_line++;
        remaining_space--;
    }

    if (in_quotes) {
        return ERR_CMD_ARGS_BAD;
    }

    if (dst > token_start && remaining_space > 0) {
        *dst = '\0';
        cmd_buff->argv[cmd_buff->argc] = token_start;
        cmd_buff->argc++;
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;

    if (cmd_buff->argc > 0) {
        return OK;
    } else {
        return WARN_NO_CMDS;
    }
}
int build_cmd_list(char *cmd_line, command_list_t *clist){
    while (isspace(*cmd_line)) {
        cmd_line++;
    }

    if (*cmd_line == '\0') {
        return WARN_NO_CMDS;
    }

    clist->num = 0;
    char *saveptr = NULL;
    char *segment = strtok_r(cmd_line, "|", &saveptr);
    while (segment != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        cmd_buff_t *cmd = &clist->commands[clist->num];
        memset(cmd, 0, sizeof(cmd_buff_t));

        cmd->_cmd_buffer = malloc(ARG_MAX);
        if (!cmd->_cmd_buffer) {
            return ERR_MEMORY;
        }
        strncpy(cmd->_cmd_buffer, segment, ARG_MAX - 1);
        cmd->_cmd_buffer[ARG_MAX - 1] = '\0';
        int rc = build_cmd_buff(cmd->_cmd_buffer, cmd);
        if (rc == OK && cmd->argc > 0) {
            clist->num++;
        } else {
            free(cmd->_cmd_buffer);
            cmd->_cmd_buffer = NULL;
        }
        segment = strtok_r(NULL, "|", &saveptr);
    }
    
    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    return 0;
}
int execute_pipeline(command_list_t *clist) {
    int n = clist->num;
    if (n < 1) return WARN_NO_CMDS;
    pid_t pids[CMD_MAX] = {0};
    int pipes[CMD_MAX - 1][2];
    int num_pipes_created = 0;
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            for (int j = 0; j < num_pipes_created; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return -1;
        }
        num_pipes_created++;
    }
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            for (int j = 0; j < num_pipes_created; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            for (int j = 0; j < i; j++) {
                if (pids[j] > 0) {
                    waitpid(pids[j], NULL, 0);
                }
            }
            return -1;
        }
        else if (pid == 0) {

            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < n - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < num_pipes_created; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
        } else {
            pids[i] = pid;
        }
    }
    for (int i = 0; i < num_pipes_created; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < n; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            continue;
        }
    }
    return 0;
}
int free_cmd_list(command_list_t *cmd_lst) {
    if (cmd_lst == NULL) return OK;
    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}
int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }

    cmd_buff->argc = 0;

    for (int i=0; i<CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return 0;
}
int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return -1;
    cmd_buff->argc = 0;

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    
    if (cmd_buff->_cmd_buffer) {
        memset(cmd_buff->_cmd_buffer, 0, ARG_MAX);
    }
    return 0;
}
int close_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return -1;

    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    
    cmd_buff->argc = 0;

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    
    return 0;
}
int exec_cmd(cmd_buff_t *cmd) {
    if (!cmd || cmd->argc == 0) return -1;
    Built_In_Cmds bic = exec_built_in_cmd(cmd);

    if (bic != BI_NOT_BI) {
        if (bic == BI_CMD_EXIT) {
            return -1;
        } else {
            return 0;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
    }
    
    int status;
    waitpid(pid, &status, 0);
    return 0;
}
