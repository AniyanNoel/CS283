#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    char *remaining = cmd_line;
    char *token = remaining;
    int counter = 0;

    memset(clist, 0, sizeof(command_list_t));
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    
    while (token != NULL) {
        char *pipe = NULL;        
        if (remaining) {
            pipe = strchr(remaining, PIPE_CHAR);
            if (pipe) {
                *pipe = '\0';
            }
        }

        while (*token == SPACE_CHAR) token++;
        
        int len = strlen(token);
        while (len > 0 && token[len-1] == SPACE_CHAR) {
            token[len-1] = '\0';
            len--;
        }

        if (counter >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        char *current = token;
        char *cmd_start = token;

        while (*current == SPACE_CHAR) current++;
        cmd_start = current;

        while (*current && *current != SPACE_CHAR) current++;
        if (*current) {
            *current = '\0';
            current++;
        }

        strncpy(clist->commands[counter].exe, cmd_start, EXE_MAX - 1);
        while (*current == SPACE_CHAR) current++;
        if (*current) {
            strncpy(clist->commands[counter].args, current, ARG_MAX - 1);
        }
        counter++;

        if (pipe) {
            token = pipe + 1;
            remaining = token;
        } else {
            token = NULL;
        }
    }
 
    clist->num = counter;
    return OK;
}