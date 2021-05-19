#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_FILE_LINES 11
#define MAX_CHAR 80
#define MAX_INPUT_ARGS 7

void get_lines(char *filename, char lines_dest[MAX_FILE_LINES][MAX_CHAR], int *num_lines);
void get_input_args(char user_input[MAX_CHAR], char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], int *num_args, char **command);
void find_command(char path_store[MAX_CHAR], char *command, char file_lines[MAX_FILE_LINES][MAX_CHAR], int *num_lines);
void regular_processes(int *num_input_arguments, char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], char path_name[MAX_CHAR]);
void redirected_out_processes(int *num_input_arguments, char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], char path_name[MAX_CHAR]);
void piped_processes(char path[MAX_CHAR], int *num_input_args, char input_arguments[MAX_INPUT_ARGS*2][MAX_CHAR], char lines[MAX_FILE_LINES][MAX_CHAR], int *num_lines);

int main(int argc, char *argv[]) {

    char *filename = ".sh360rc";

    //file lines storage location
    char lines[MAX_FILE_LINES][MAX_CHAR];
    //total number of lines counter
    int num_lines = 0;
    
    //get and store lines from file
    get_lines(filename, lines, &num_lines);

    //check if number of directories in file exceeds 10 (i.e if file has more than 11 lines)
    if (num_lines > MAX_FILE_LINES)
    {
        fprintf(stdout, "Error: The number of directories present (%d) in the file '%s' exceed the limit (10)\n", num_lines, filename);
        exit(1);
    }
    
    /*
    code from appendix_a.c to get repeated user input
    altered to match my code
    */

    //array storing user input string
    char input[MAX_CHAR];
    //2D array storing command arguments
    char input_arguments[MAX_INPUT_ARGS*2][MAX_CHAR];
    //argument counter
    int  num_input_args = 0;
    //store for actual command name without arguments
    char *command = NULL;

    for(;;) {
        fprintf(stdout, "%s", lines[0]);
        fflush(stdout);
        fgets(input, MAX_CHAR, stdin);

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        if(strlen(input) == 0)
        {
            fprintf(stdout, "Error: No input detected\n");
        }
        else
        {
            fflush(stdin);

            if (strcmp(input, "exit") == 0) {
                exit(0);
            }

            //get and store each argument on its own
            get_input_args(input, input_arguments, &num_input_args, &command);

            //store for full path (directory + command) if it exists
            char path[MAX_CHAR];

            //create path for command if it exists and is executable in any of the provided directories
            find_command(path, command, lines, &num_lines);

            //if the command was either not found in a directory or it was found and wasn't executable, consider it not found
            if (*path == 0)
            {
                fprintf(stdout, "Error: Command not found: %s\n", command);
            } 
            else
            {

                if (strcmp(input_arguments[0], "OR") == 0){
                    redirected_out_processes(&num_input_args, input_arguments, path);
                }
                else if(strcmp(input_arguments[0], "PP") == 0){
                    fprintf(stdout, "Sadly, this program will not able to complete this feature, please csee README for detailed information.\n");
                    //piped_processes(path, &num_input_args, input_arguments, lines, &num_lines);
                } 
                else{
                    regular_processes(&num_input_args, input_arguments, path);
                }
            }

            //reset argument count for next input
            num_input_args = 0;
        }
    }
}

/* Function that:
 * reads into specified file line by line
 * tokenizes each line and stores tokens in an array
 * "returns" a count of the number of lines in the file as well as an array containing said lines as strings
 */
void get_lines(char *filename, char lines_dest[MAX_FILE_LINES][MAX_CHAR], int *num_lines){
    //temporary storage locations for lines that are read from file
    char tmp[MAX_CHAR];
    char line[MAX_CHAR];

    FILE *dotfile;
    dotfile = fopen(filename, "r");

    //check to see if file exists in specified directory
    if (dotfile == NULL)
    {
        fprintf(stdout, "Error: Could not open source file\n");
        exit(1);
    }

    //loop reads lines, rids lines of "\n" char, stores in liines in array
    while (fgets(tmp, MAX_CHAR, dotfile) != NULL)
    {
        strncpy(line, tmp, MAX_CHAR);
        char* token = strtok(line, "\n");
        strncpy(lines_dest[*num_lines], token, MAX_CHAR);
        *num_lines+=1;
    }

    //if there are no directories in the file i.e nothing after first line; terminate
    if(*num_lines == 1)
    {
        fprintf(stdout, "Error: There are no directories specified in the file\n");
        exit(1);
    }
    
    int fclose( FILE *dotfile );
}

/* Function that:
 * tokenizes user input string so as to store the input arguments as separate strings
 * singles out the command itself and stores a separate copy of it away from the collection of input arguments
 * "returns" an array that stores each argument from user input on its own, the argument count as well as the actaul command stored as a string
 */
void get_input_args(char user_input[MAX_CHAR], char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], int *num_args, char **command){
    //storage for copy of user input string
   char tmp[MAX_CHAR];
   //delimeters for tokenization
   char delim[] = " \n\0";

   strncpy(tmp, user_input, MAX_CHAR);
   char *token = strtok(tmp, delim);
   while(token != NULL)
   {
       //store each argument as its own separate string
       strncpy(input_args[*num_args], token, MAX_CHAR);
       token = strtok(NULL, delim);
       *num_args+=1;
    }
    
    //store separate copy of the actual command itself
    if (strcmp(input_args[0], "OR") == 0 || strcmp(input_args[0], "PP") == 0)
    {
        *command = input_args[1];
    }
    else
    {
        *command = input_args[0];
    }

    //check if number of arguments exceeds assignment specifications
    if(*num_args > MAX_INPUT_ARGS)
    {
        fprintf(stdout, "Error: The number of arguments provided (%d) exceeds the limit (7)\n", *num_args);
    }
}

/* Function that
 * checks to see if the command given in user input exists in any of the directories provided from the file
 * and if it does exist, checks whether or not it is an executable
 * "returns" full path (directory + command) if command satisfies the above
 */
void find_command(char path_store[MAX_CHAR], char *command, char file_lines[MAX_FILE_LINES][MAX_CHAR], int *num_lines){
    //struct that stores command file information
    struct stat stats;
    //counter that keeps track of how many directories the command is found in
    int num_paths_found = 0;
    //as command can be present in multiple directories, store each of them where command exists
    char paths_found[MAX_FILE_LINES][MAX_CHAR];

    /* 
     * for each directory in the file
     * -append command to directory to create full path
     * -use stat() to see if path exits
     * -if it exists, check file information (struct stats) to see if command file is an executable
     * -if it is an executable, store full path
     */
    for (int i = 1; i < *num_lines; i++)
    {
        //temp store for directory
        char temp[MAX_CHAR];
        strncpy(temp, file_lines[i], MAX_CHAR);
        strncat(temp, "/", 1);
        strncat(temp, command, strlen(command));

        if (stat(temp, &stats) == 0)
        {
            if (stats.st_mode & S_IXGRP)
           {
                strncpy(paths_found[num_paths_found], temp, MAX_CHAR);
                num_paths_found+=1;
            }
        }
    }

    if(num_paths_found == 0){
        *path_store = 0;
    }else{
        //for assignment purposes, only store the first full path
        strncpy(path_store, paths_found[0], MAX_CHAR);
    }
}

/* Function that
 * creates child processes that emulates the functioning of a unix shell upon receiving some user input
 */
void regular_processes(int *num_input_arguments, char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], char path_name[MAX_CHAR]){
    /*
    code from appendix_b.c to creates child processes
    altered to match my code
    */
    char *args[*num_input_arguments+1];
    char *envp[] = { 0 };

    args[0] = path_name;
    for (int i = 1; i < *num_input_arguments; i++)
    {
        args[i] = input_args[i];
    }
    args[*num_input_arguments] = 0;
        
    int pid;
    int status;

    if ((pid = fork()) == 0) {
        printf("child: about to start...\n");
        execve(args[0], args, envp);
        printf("child: SHOULDN'T BE HERE.\n");
    }
    
    printf("parent: waiting for child to finish...\n");
    while (wait(&status) > 0) {
        printf("parent: child is finished.\n");
    }
}

/* Function that
 * creates child processes that emulates the functioning of a unix shell upon receiving some user input
 */
void redirected_out_processes(int *num_input_arguments, char input_args[MAX_INPUT_ARGS*2][MAX_CHAR], char path_name[MAX_CHAR]){
    /*
    code from appendix_c.c to creates child processes that redirect output to a specified file
    altered to match my code
    */

    //num_input_args-3 because of the OR, the -> and the output file
    char *args[*num_input_arguments-3];

    char out_dest[MAX_CHAR];

    args[0] = path_name;
    for (int i = 2; i < *num_input_arguments-1; i++)
    {
        if(strcmp(input_args[i], "->") != 0){
            args[i-1] = input_args[i];
        } else {
            strncpy(out_dest, input_args[i+1], MAX_CHAR);
        }
    }
    args[*num_input_arguments-3] = 0;
    
    char *envp[] = { 0 };
    int pid, fd;
    int status;

    if ((pid = fork()) == 0) {
        printf("child: about to start...\n");

        fd = open(out_dest, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd == -1) {
            fprintf(stderr, "cannot open output.txt for writing\n");
            exit(1);
        }
        dup2(fd, 1);
        dup2(fd, 2); 
        execve(args[0], args, envp);
        printf("child: SHOULDN'T BE HERE.\n");
    }

    printf("parent: waiting for child to finish...\n");
    waitpid(pid, &status, 0);
    printf("parent: child is finished.\n");
}

/* Function that
 * creates child processes that emulates the functioning of a unix shell upon receiving some user input
 */
void piped_processes(char path[MAX_CHAR], int *num_input_args, char input_arguments[MAX_INPUT_ARGS*2][MAX_CHAR], char lines[MAX_FILE_LINES][MAX_CHAR], int *num_lines){
    //store for all command paths present in full pipe command
    char pipe_command_paths[5][MAX_CHAR]; //set max number of commands that can be piped to 5
    strncpy(pipe_command_paths[0], path, MAX_CHAR); //store path of first command in pipe

    //store for each command's arguments present in user input
    char command_args[5][MAX_INPUT_ARGS][MAX_CHAR]; //for each command store its arguments as separate strings

    //temp store for a command so as to perform a check on whether it exists in provided
    //directories or not
    char *temp_command = NULL;

    //temp store for full command path
    char temp_path[MAX_CHAR];

    //command counter
    int num_commands = 1; //i.e command already present that brought us to this state of the code
    int num_arrows = 0; //number of commands will always be 1 more than number of arrows
    int arg_pos = 0; //argument index


    for (int i = 1; i < *num_input_args; i++)
    {
        if(strcmp(input_arguments[i], "->") == 0){
            num_arrows+=1;
        }


        if(strcmp(input_arguments[i], "->") == 0 && num_commands == num_arrows){
            //reset argument position
            arg_pos = 0;

            num_commands+=1;

            //next index after encountering an arrow will contain a command
            temp_command = input_arguments[i+1];

            //check to see if command exists and is executable
            find_command(temp_path, temp_command, lines, num_lines);
            if(*temp_path == 0){
                fprintf(stdout, "Error: Command not found: %s\n", temp_command);
            }else{
                strncpy(pipe_command_paths[i], temp_path, MAX_CHAR);
            }

        }else{
            //at num_commands, store the args
            strncpy(command_args[num_commands-1][arg_pos], input_arguments[i], MAX_CHAR);
            arg_pos+=1;
        }
    }

    char *cmd_head[MAX_INPUT_ARGS];
    cmd_head[0] = pipe_command_paths[0];

    for (int k = 1; k < MAX_INPUT_ARGS; k++)
    {
        if(strcmp(command_args[0][k], "") != 0){
            cmd_head[k] = command_args[0][k];
        }else{
            cmd_head[k] = '\0';
        }
    }
    
    char *cmd_tail[MAX_INPUT_ARGS];
    cmd_tail[0] = pipe_command_paths[1];

}