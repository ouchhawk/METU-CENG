#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ext2fs.h"

void traverse_and_find_file(char* path){
    char* inode_number=malloc(strlen(path));;
    char* folder_name=malloc(strlen(path));;
    int flag=0, current_inode_number=0, next_offset=0;
    int result=0;
    
    u_int32_t* temp_inode_number = malloc(sizeof(u_int32_t));
    u_int16_t* temp_record_length = malloc(sizeof(u_int16_t));
    u_int8_t* temp_name_length = malloc(sizeof(u_int8_t));
    u_int8_t* temp_file_type = malloc(sizeof(u_int8_t));
    char* temp_name=malloc(255);

    *temp_record_length=0;

    for(int i=0, j=0; i<strlen(path); i++, j++){
        if(flag==0 && path[i]!='/'){ //inode verilmius
            inode_number[i]=path[i];
        }
        else if (flag==0 && path[i]!='/'){
            current_inode_number=atoi(inode_number);
        }
        else if (flag==0 && path[i]=='/'){ 
            flag=1;
        } 
        else if (flag==1 && path[i]!='/'){
            folder_name[i-1]=path[i];
        }
        else if (flag==1 && path[i]=='/'){

            lseek(FILE_DESCRIPTOR, DATA_BLOCK*BLOCK_SIZE+next_offset, SEEK_SET);
            read(FILE_DESCRIPTOR, temp_inode_number, sizeof(uint32_t)); 
            read(FILE_DESCRIPTOR, temp_record_length, sizeof(uint16_t)); 
            read(FILE_DESCRIPTOR, temp_name_length, sizeof(uint8_t)); 
            *temp_name_length+= 4 - (*temp_name_length % 4);
            read(FILE_DESCRIPTOR, temp_file_type, sizeof(uint8_t)); 
            read(FILE_DESCRIPTOR, temp_name, *temp_name_length); 

            printf("this is the first string: %s \n", folder_name);
            while(strcmp(folder_name,temp_name) != 0){
                lseek(FILE_DESCRIPTOR, DATA_BLOCK*BLOCK_SIZE+next_offset, SEEK_SET);
                read(FILE_DESCRIPTOR, temp_inode_number, sizeof(uint32_t)); 
                read(FILE_DESCRIPTOR, temp_record_length, sizeof(uint16_t)); 
                read(FILE_DESCRIPTOR, temp_name_length, sizeof(uint8_t)); 
                read(FILE_DESCRIPTOR, temp_file_type, sizeof(uint8_t)); 
                //(*temp_name_length)+= 4 - (*temp_name_length % 4);
                read(FILE_DESCRIPTOR, temp_name, *temp_name_length); 
                next_offset+=*temp_record_length;

                printf("inode number: %d \n", *temp_inode_number);
                printf("record length: %d \n", *temp_record_length);
                printf("name length: %d \n", *temp_name_length);
                printf("type %d \n", *temp_file_type);
                printf("name: %s \n", temp_name);
            }
            printf("this is the inode number: %d", *temp_inode_number);
            //return 1;
            /*
            lseek(FILE_DESCRIPTOR, DATA_BLOCK*BLOCK_SIZE, SEEK_SET);
            read(FILE_DESCRIPTOR, dir_entry.inode, sizeof(uint32_t)); 
            read(FILE_DESCRIPTOR, dir_entry.file_type, sizeof(uint16_t)); 
            read(FILE_DESCRIPTOR, dir_entry.file_type, sizeof(uint8_t)); 
            read(FILE_DESCRIPTOR, dir_entry.file_type, sizeof(uint8_t)); 
            read(FILE_DESCRIPTOR, dir_entry.name, dir_entry.name_length); 

            printf("inode number: %d \n", dir_entry.inode);
            printf("record length: %d \n", dir_entry.length);
            printf("name length: %d \n", dir_entry.name_length);
            printf("type %d \n", dir_entry.file_type);
            printf("name: %s \n", dir_entry.name);
            */
        }
    }
    printf("this is the inode number: %d", atoi(inode_number));
}

int main(int argc, char *argv[])
{
    char *FS_IMAGE, *SOURCE, *DEST;
    int source_inode_number=0, new_inode_offset=0, next_offset=0; 
    struct ext2_inode source_inode, destination_inode;

    // Get command line arguments
    char *func = (char*) malloc(sizeof(char)*(strlen(argv[1])+1));
    strcpy(func, argv[1]);
    if (strcmp(func,"dup")==0){
        FS_IMAGE = (char*) malloc(sizeof(char)*(strlen(argv[2])+1));
        strcpy(FS_IMAGE, argv[2]);
        SOURCE = (char*) malloc(sizeof(char)*(strlen(argv[3])+1));
        strcpy(SOURCE, argv[3]);
        DEST = (char*) malloc(sizeof(char)*(strlen(argv[4])+1));
        strcpy(DEST, argv[4]);
    }
    FILE_DESCRIPTOR = open(FS_IMAGE, O_RDWR);
    initialize_global_variables();
    create_inode_list();
        
        //src_inode=malloc(sizeof(struct ext2_inode));
        //dest_inode=malloc(sizeof(struct ext2_inode));
        //print_inode_with_id(26);
        //printf("%d", inode_number);
        
        //

        //path_parser(SOURCE);
        //printf("%s", g_src.parsed_path[0]);

        // // for(int u=0; u<g_src.length; u++){
        // //     if(g_src.starts_with_inode){
        // //         source_inode_number=atoi(g_src.parsed_path[0]);
        // //         source_inode=get_inode(source_inode_number);
        // //         new_inode_offset=allocate_first_unallocated_inode();
        // //         break;
        // //     }
        // //     else{    
        // //         u_int16_t* next = malloc(sizeof(u_int16_t));
                    
        // //         struct ext2_inode current_inode;
        // //         current_inode=get_inode(2);

        // //         int dentry_length=0;
        // //         int dentry_inode=0;
        // //         int dentry_name_length=0;
        // //         int dentry_type=0;
        // //         int last_dentry_offset=0;
                
        // //         char* name = malloc(255);   
        // //         char* buff = malloc(255);   
        // //         int ino;
        // //         for(int i=0; i<20; i++){         
        // //             lseek(FILE_DESCRIPTOR, next_offset + current_inode.direct_blocks[0] * BLOCK_SIZE, SEEK_SET);
        // //             read(FILE_DESCRIPTOR, buff, 4);
        // //             dentry_inode=*buff;
        // //             printf("\n %d VS %d \n", *buff, dentry_inode);
        // //             read(FILE_DESCRIPTOR, buff, 2);
        // //             dentry_length=*buff;
        // //             read(FILE_DESCRIPTOR, buff, 1);
        // //             dentry_name_length=*buff;
        // //             read(FILE_DESCRIPTOR, buff, 1);
        // //             dentry_type=*buff;
        // //             read(FILE_DESCRIPTOR, buff, dentry_name_length);
        // //             printf("\n HERE IS %s\n", buff);


        // //             //printf("%s at %d len %d\n", buff, next_offset + current_inode.direct_blocks[0] * BLOCK_SIZE, strlen(g_src.parsed_path[u]));
        // //             if(strncmp(buff, g_src.parsed_path[u], strlen(g_src.parsed_path[u]))==0){
        // //                 printf("\n FOUND %s with inode %d at %d\n", g_src.parsed_path[u], dentry_inode, next_offset + current_inode.direct_blocks[0] * BLOCK_SIZE);
        // //                 source_inode_number=dentry_inode;
        // //                 source_inode=get_inode(source_inode_number);
        // //                 new_inode_offset=allocate_first_unallocated_inode();
        // //                 last_dentry_offset=next_offset + current_inode.direct_blocks[0] * BLOCK_SIZE + dentry_length;
        // //                 printf("\n NEXT AVAILABLE DENTRY OFFSET %d\n", last_dentry_offset);
        // //                 break;
        // //             }
        // //             else{
        // //                 next_offset=next_offset + dentry_length ;
        // //                 printf("\n NEXT OFFSET %d\n", next_offset);
        // //             } 
        // //         }
        // //     }
        // // }
        
    //     u_int32_t* buff = malloc(sizeof(u_int32_t));
    //     lseek(FILE_DESCRIPTOR, new_inode_offset, SEEK_SET);

    //     *buff=source_inode.mode;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int16_t));
    //     *buff=source_inode.uid;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int16_t));
    //     *buff=source_inode.size;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.access_time;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.creation_time;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.modification_time;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.deletion_time;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.gid;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int16_t));
    //     *buff=source_inode.link_count;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int16_t));
    //     *buff=source_inode.block_count_512;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.flags;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     *buff=source_inode.reserved;
    //     write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     for(int i=0; i<EXT2_NUM_DIRECT_BLOCKS; i++){
    //         *buff=source_inode.direct_blocks[i];
    //         write(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    //     }            
    //     printf("\n WRITTEN OFFSET: %d \n", new_inode_offset);
    //     printf("\n WRITTEN INODE: %d \n", g_allocated_inode_number);
    //     print_inode(get_inode(g_allocated_inode_number));

    // }
    // else if (strcmp(func,"rm")==0){
    //     FS_IMAGE = (char*) malloc(sizeof(char)*(strlen(argv[2])+1));
    //     strcpy(FS_IMAGE, argv[2]);


    //     DEST = (char*) malloc(sizeof(char)*(strlen(argv[3])+1));
    //     strcpy(DEST, argv[3]);

    // }
    //print_super_block(g_super_block);
    //print_inode_with_id(17);
    //print_block_group_descriptor(g_block_group_descriptors[0]);
    //printf("%d", set_first_available_inode_bitmap_bit());
    //print_inode_size(17);
    //write_inode(get_inode(17), 18);



    /*u_int32_t* buff;
    buff=malloc(sizeof(u_int32_t));
    *buff=0; 
    lseek(FILE_DESCRIPTOR, 6144, SEEK_SET);
    read(FILE_DESCRIPTOR, buff, sizeof(u_int32_t));
    printf("\n NUMERIC %d \n", *buff);*/
/*
    unsigned char* buf;
    buf=malloc(sizeof(unsigned char));
    //*buf=112; 
    lseek(FILE_DESCRIPTOR, 16436, SEEK_SET);
    read(FILE_DESCRIPTOR, buf, 8);
    */

    /*for(int i=0; i<32; i++){
        lseek(FILE_DESCRIPTOR, i, SEEK_SET);
        write(FILE_DESCRIPTOR, buf, 1);  // eksik kalan byte'lar kalan yerler 0 ile doluyor
    }*/

    //free(func);
    //free(g_block_group_descriptors);

  
    //char path[300]="12/MLT/ugur.txt";
    //printf("DATA BLOCK %u \n", DATA_BLOCK);
    //printf("\n DATA BLOCK %u \n", find_directory_offset(11));

    //printf("\n PATH OFFSET %u \n", path_walker("/turkiye/hellos.xtx"));

    char *str = malloc(50);
    
    strcpy(str, "/turkiye/hoylanda/hellos.xtx");

    struct path *pth=path_parser(str); 

    //printf("%d", path_walker(&pth));
    printf("%d", path_walker(pth));
    create_inode_list();

    return 0;
}