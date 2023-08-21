#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ext2fs.h"

u_int32_t power(u_int32_t a, u_int32_t b){
    u_int32_t result=1;
    for(u_int32_t i=0; i<b;i++){
        result=result*a;
    }
    return result;
}

int get_size_of_pointer_string(char *path_string){
    int length=0;
    while(*path_string){
        path_string++;
        length++;
    }
    return length;
}

struct path* path_parser(char *path_string){
    int length=0;
    struct path *path = malloc(sizeof(struct path));

    for(int i=0; i<get_size_of_pointer_string(path_string); i++){
        if(path_string[i] == '/'){
            length++;
        }
    }
    path->length=length;
    char *token = strtok(path_string, "/");
    int i=0;
    while( token != NULL && i< path->length ) {
        strcpy(path->parsed_path[i], token);
        //printf("%s \n", path->parsed_path[i]);
        i++;
        token = strtok(NULL, "/");
    }
    return path;
}

