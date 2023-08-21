#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include "parser.h"
#include <fcntl.h>
#include  <assert.h>
#pragma  set follow-fork-mode child/parent
#pragma  detach detach-on-fork off
#pragma  info inferiors

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;

class Bundle{
public:
    char* name = new char[256];
    char* args[50][256];
    int command_count;

    void printBundle() {
        //printf("BUNDLE NAME: %s \n", name);

        for (int i = 0; args[i]; i++) {
            //printf("ARG[%d]: %s \n", i, args[i]);
        }
    }
};

int main()
{
    int is_bundle_creation=0, parse_return=0;
    union parsed_input *parsedInput = new parsed_input;
    char line[256];
    Bundle *storedBundleList[256];
    int arg_index=0, bundle_index=0, command_index=0;

    while(true){
        string line_str;
        int line_no=0;
        getline(cin, line_str);

        char input_filename[256], output_filename[256];
        bool has_input_file= false,has_output_file= false;

        int len = line_str.length();
        line_str[len]='\n';
        line_str[len+1]='\0';
        const char * line_ptr = line_str.c_str();
        parse_return = parse((char*)  line_ptr, is_bundle_creation, parsedInput);
        line_no++;

        //inside bundle creation, take regular commands
        if(is_bundle_creation){
            if(parsedInput->command.type==PROCESS_BUNDLE_STOP){
                is_bundle_creation=0;
                command_index=0;
                //printf("%s -CREATED \n", storedBundleList[bundle_index]->name);

                bundle_index++;
                storedBundleList[bundle_index] = NULL;
            }
            else{
                int argument_counter=0;
                for(; parsedInput->argv[argument_counter]; argument_counter++);

                int argument_index=0;
                for(; argument_index < argument_counter; argument_index++){
                    storedBundleList[bundle_index]->args[command_index][argument_index] = parsedInput->argv[argument_index];
                }
                storedBundleList[bundle_index]->args[command_index][argument_index] = NULL;

                for(int i=0; i<argument_index;i++){
                    //printf("ARG[%d]: %s \n", i, storedBundleList[bundle_index]->args[command_index][i]);
                }

                command_index++;
                storedBundleList[bundle_index]->args[command_index][0] = NULL;
                storedBundleList[bundle_index]->command_count = command_index;
            }
        }
        else
        {
            if(parsedInput->command.type==QUIT){
                //quit section
                return 0;
            }
            else if(parsedInput->command.type==PROCESS_BUNDLE_CREATE){
                Bundle *newBundle = new Bundle;
                storedBundleList[bundle_index] = newBundle;
                is_bundle_creation=1;
                strcpy(storedBundleList[bundle_index]->name, parsedInput->command.bundle_name);
                //printf("%s -CREATING \n", storedBundleList[bundle_index]->name);

            }
            else if(parsedInput->command.type==PROCESS_BUNDLE_EXECUTION){
                int executing_bundle_count = parsedInput->command.bundle_count;
                int pid_count =0;
                for(int executing_bundle_no=0; executing_bundle_no<executing_bundle_count; executing_bundle_no++){
                    if(parsedInput->command.bundles[executing_bundle_no].input){
                        strcpy(input_filename, parsedInput->command.bundles[executing_bundle_no].input);
                        has_input_file=true;
                    }
                    if(parsedInput->command.bundles[executing_bundle_no].output){
                        strcpy(output_filename, parsedInput->command.bundles[executing_bundle_no].output);
                        has_output_file=true;
                        ////printf("filename: %s", output_filename);
                    }
                }
                int pid[pid_count];
                int pid_index=-1;
                int child_status;
                for(int executing_bundle_no=0; executing_bundle_no<executing_bundle_count; executing_bundle_no++){
                    for(int i=0; storedBundleList[i]; i++){
                        if(strcmp(storedBundleList[i]->name, parsedInput->command.bundles[executing_bundle_no].name) == 0){
                            // entered bundle execution region

                            int fd[executing_bundle_count][2]; // declare file descriptors
                            int command_count = storedBundleList[i]->command_count;
                            pipe(fd[executing_bundle_no]);
                            //execvp(storedBundleList[i]->args[command_no][0], storedBundleList[i]->args[command_no]);

                            int command_no=0;
                            for(int command_no=0; command_no<command_count; command_no++) {
                                int filedes[2];
                                pid_index++;
                                pid[pid_index]=fork();

                                if(pid[pid_index] == 0){

                                    char buffer[256];
                                    if(executing_bundle_no==0){
                                        if(executing_bundle_count > 1)
                                            dup2(fd[executing_bundle_no][1], STDOUT_FILENO);

                                        if(has_input_file ) {
                                            filedes[0] = open(input_filename, O_RDONLY | S_IRUSR | S_IWUSR);
                                            dup2(filedes[0], STDIN_FILENO);
                                            close(filedes[0]);
                                        }
                                        if(has_output_file ){
                                            filedes[1] = open( output_filename, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
                                            dup2(filedes[1], STDOUT_FILENO);
                                            close(filedes[1]);
                                        }
                                        for(int k=0; k < executing_bundle_count; k++) {
                                            close(fd[k][0]);
                                            close(fd[k][1]);
                                        }
                                        execvp(storedBundleList[i]->args[command_no][0], storedBundleList[i]->args[command_no]);
                                    }
                                    else if(executing_bundle_no == (executing_bundle_count-1)){
                                        dup2(fd[executing_bundle_no-1][0], STDIN_FILENO);

                                        if(has_output_file ){
                                            filedes[1] = open( output_filename, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
                                            dup2(filedes[1], STDOUT_FILENO);
                                            close(filedes[1]);
                                        }

                                        for(int k=0; k < executing_bundle_count; k++) {
                                            close(fd[k][0]);
                                            close(fd[k][1]);
                                        }
                                        execvp(storedBundleList[i]->args[command_no][0], storedBundleList[i]->args[command_no]);
                                    }
                                    else
                                    {
                                        dup2(fd[executing_bundle_no-1][0], STDIN_FILENO);
                                        dup2(fd[executing_bundle_no][1], STDOUT_FILENO);

                                        for(int k=0; k < executing_bundle_count; k++) {
                                            close(fd[k][0]);
                                            close(fd[k][1]);
                                        }
                                        execvp(storedBundleList[i]->args[command_no][0], storedBundleList[i]->args[command_no]);
                                    }
                                }
                                else {
                                    //PARENT
                                }
                            }
                        }
                    }
                }
                for(int i=0; i<pid_count; i++){
                    waitpid(pid[i],&child_status, 0);
                }
            }
        }
    }
}