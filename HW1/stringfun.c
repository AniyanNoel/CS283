#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int print_words(char *, int, int);
int reverse_words(char *, int, int);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *buff_ptr = buff;
    char *str_ptr = user_str;
    int count = 0;
    int last_was_space = 1;  
    
    while (*str_ptr != '\0') {
        count++;
        if (count > len) return -1;  
        str_ptr++;
    }
    
    str_ptr = user_str;
    while (*str_ptr != '\0') {
        if (*str_ptr == ' ' || *str_ptr == '\t') {
            if (!last_was_space) {
                *buff_ptr = ' ';
                buff_ptr++;
                last_was_space = 1;
            }
        } else {
            *buff_ptr = *str_ptr;
            buff_ptr++;
            last_was_space = 0;
        }
        str_ptr++;
    }
    
    while (buff_ptr < buff + len) {
        *buff_ptr = '.';
        buff_ptr++;
    }
    
    return count; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
    int count = 0;
    int in_word = 0;
    char *ptr = buff;
    char *end = buff + str_len;
    
    while (ptr < end) {
        if (*ptr == ' ') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        ptr++;
    }
    return count;
}
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int reverse_words(char *buff, int len, int str_len) {
    if (!buff || len <= 0 || str_len <= 0) {
        return -1;
    }
    
    char *left = buff;
    char *right = buff + str_len - 1;
    char temp;
    
    while (left < right) {
        temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
    
    printf("Reversed String: ");
    char *ptr = buff;
    while (ptr < buff + str_len) {
        putchar(*ptr);
        ptr++;
    }
    printf("\n");
    
    return 0;
}

int print_words(char *buff, int len, int str_len) {
    if (!buff || len <= 0 || str_len <= 0) {
        return -1;
    }
    
    char *ptr = buff;
    char *end = buff + str_len;
    char *word_start = buff;
    int in_word = 0;
    int word_count = 0;
    
    printf("Word Print\n----------\n");
    while (ptr <= end) {
        if (ptr == end || *ptr == ' ') {
            if (in_word) {
                word_count++;
                printf("%d. ", word_count);
                char *temp = word_start;
                while (temp < ptr) {
                    putchar(*temp);
                    temp++;
                }
                printf(" (%ld)\n", ptr - word_start);
                in_word = 0;
            }
        } else if (!in_word) {
            word_start = ptr;
            in_word = 1;
        }
        ptr++;
    }
    
    return 0;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /* This if statement accounts for the number of arugments and ensures
    that the argv[1] is an option flag. The first condition ensures that argv[1] exists before moving on. As taught in 270, the logical operator OR only holds true when all conditions are true. This is a safe practice to ensure both conditions are met without throwing any types of errors or dereferences.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /* The purpose of this if statement is to ensure three arguemnts have been given. Similar to TODO #1 we want to ensure no errors are thrown, no out of bounds memory is accessed or dereferecing occurs which can all lead to crashes. 
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        printf("Error allocating memory for buffer\n");
        exit(2);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ); //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(99);
    }

    switch (opt){
    case 'c':
        rc = count_words(buff, BUFFER_SZ, user_str_len);
        if (rc < 0){
            printf("Error counting words, rc = %d", rc);
            exit(2);
        }
        printf("Word Count: %d\n", rc);
        break;
    //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
    //       the case statement options
    case 'r':
        rc = reverse_words(buff, BUFFER_SZ, user_str_len);
        if (rc < 0) {
            printf("Error reversing string, rc = %d\n", rc);
            exit(2);
        }
        break;

    case 'w':
        rc = print_words(buff, BUFFER_SZ, user_str_len);
        if (rc < 0) {
            printf("Error printing words, rc = %d\n", rc);
            exit(2);
        }
        break;

    case 'x':
        printf("Not Implemented!\n");
        exit(3);
        break;

    default:
        usage(argv[0]);
        exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE: Our buffer is 50 now, but what if it changes? Providing 
//                                  both of these allows us to reuse functions later on.