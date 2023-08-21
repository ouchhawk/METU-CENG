#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ext2fs.h"

void initialize_global_variables(){
    lseek(FILE_DESCRIPTOR, 1024, SEEK_SET);
    read(FILE_DESCRIPTOR, &g_super_block, sizeof(struct ext2_super_block));
    BLOCK_SIZE= power(2, 10+ g_super_block.log_block_size);
    INODE_SIZE= g_super_block.inode_size;
    FS_SIZE=g_super_block.block_count*BLOCK_SIZE;
    int group_count = g_super_block.inode_count / g_super_block.inodes_per_group;
    g_block_group_descriptors = malloc(sizeof(struct ext2_block_group_descriptor));

    lseek(FILE_DESCRIPTOR, BLOCK_SIZE, SEEK_SET);
    read(FILE_DESCRIPTOR, g_block_group_descriptors, group_count*sizeof(struct ext2_block_group_descriptor));
    DATA_BLOCK=g_block_group_descriptors->inode_table + (g_super_block.inode_size*g_super_block.inode_count/BLOCK_SIZE);
    //ROOT_OFFSET=find_directory_offset(2);
    ROOT_OFFSET=16384;
}

void create_inode_list(){
    u_int32_t inode_bitmap, inode_table_offset, inode_offset, size, access_time, creation_time, modification_time, deletion_time, block_count_512, flags, reserved, single_indirect, double_indirect, triple_indirect;
    u_int16_t mode, uid, gid, link_count;
    char full_path[255];
    struct ext2_inode *previous_inode = (struct ext2_inode*) malloc(sizeof(struct ext2_inode));

    lseek(FILE_DESCRIPTOR, g_block_group_descriptors->inode_bitmap*BLOCK_SIZE, SEEK_SET);
    read(FILE_DESCRIPTOR, &inode_bitmap, sizeof(u_int32_t));
    
    for(int i=0; i<g_super_block.inode_count; i++){
        struct ext2_inode *inode = (struct ext2_inode*) malloc(sizeof(struct ext2_inode));
        inode_table_offset=g_block_group_descriptors->inode_table*BLOCK_SIZE;
        inode_offset=g_super_block.inode_size*i;
        lseek(FILE_DESCRIPTOR, inode_table_offset + inode_offset , SEEK_SET);
        read(FILE_DESCRIPTOR, &inode->mode, sizeof(u_int16_t));
        read(FILE_DESCRIPTOR, &inode->uid, sizeof(u_int16_t));
        read(FILE_DESCRIPTOR, &inode->size, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->access_time, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->creation_time, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->modification_time, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->deletion_time, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->gid, sizeof(u_int16_t));
        read(FILE_DESCRIPTOR, &inode->link_count, sizeof(u_int16_t));
        read(FILE_DESCRIPTOR, &inode->block_count_512, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->flags, sizeof(u_int32_t));
        read(FILE_DESCRIPTOR, &inode->reserved, sizeof(u_int32_t));

        inode->is_set = (((inode_bitmap>>i) % 2)) == 0 ? 0 : 1;

        //strcpy(inode->full_path, get_path_of_inode(inode));
        
        if(i==0){
            g_initial_inode=inode;
            previous_inode=inode;
        }
        else{
            previous_inode->next=inode;
            previous_inode=inode;
        }
    }
    previous_inode->next=NULL;

    // int m=0;
    // while(g_initial_inode!=NULL){
    //     m++;
    //     printf("INODE NUMBER IS %d AND IS_SET? %d\n", m,g_initial_inode->is_set);
    //     print_inode(*g_initial_inode);
    //     g_initial_inode=g_initial_inode->next;
    // }
}

//DONE
struct ext2_inode get_inode (int inode_number){
    struct ext2_inode inode;
    uint32_t block_group=((inode_number-1) / g_super_block.inodes_per_group)+1;
    uint32_t local_inode_number=((inode_number-1) % g_super_block.inodes_per_group)+1;
    uint32_t inode_table_offset=g_block_group_descriptors[block_group-1].inode_table * BLOCK_SIZE;
    uint32_t offset=inode_table_offset+(local_inode_number-1)*INODE_SIZE;
    lseek(FILE_DESCRIPTOR, offset, SEEK_SET);
    read(FILE_DESCRIPTOR, &inode, sizeof(struct ext2_inode));
    return inode;
}

//DONE
u_int32_t find_directory_offset(u_int32_t dir_inode_number){
    u_int32_t inode_number;

    for(u_int32_t j=DATA_BLOCK*BLOCK_SIZE; j<FS_SIZE; j+=BLOCK_SIZE){
        lseek(FILE_DESCRIPTOR, j, SEEK_SET);
        read(FILE_DESCRIPTOR, &inode_number, sizeof(uint32_t));
        //printf("%u \n", *inode_number);
        if(dir_inode_number == inode_number){  
            return j;
        }
    }
}

// returns directory entry of given directory
uint32_t path_walker(struct path *path){
    uint32_t current_offset=0;
    uint32_t inode_number=0;
    uint16_t record_length=0;
    uint8_t name_length;
    uint8_t file_type = 0;
    char *name = malloc(255*sizeof(char));
    char *buff = malloc(sizeof(char));
    //printf("%u \n", path->length);

    current_offset=find_directory_offset(2);
    lseek(FILE_DESCRIPTOR, current_offset, SEEK_SET);
    for(int i=0; i<path->length-1; i++){
        for(int j=DATA_BLOCK*BLOCK_SIZE; j<FS_SIZE; j+=BLOCK_SIZE){
            read(FILE_DESCRIPTOR, &inode_number, sizeof(uint32_t)); 
            read(FILE_DESCRIPTOR, &record_length, sizeof(uint16_t)); 
            read(FILE_DESCRIPTOR, &name_length, sizeof(uint8_t)); 
            //printf("%u \n", name_length);
            read(FILE_DESCRIPTOR, &file_type, sizeof(uint8_t)); 
            int k=0;
            for(k; k<name_length; k++){
                read(FILE_DESCRIPTOR, &buff, sizeof(char));
                name[k]=buff;
            }
            name[k]=0;
            printf("%s \n", name);
            
            printf("-> %s vs %s \n", path->parsed_path[i], name);

            if(strcmp(path->parsed_path[i], name) == 0){
                current_offset=find_directory_offset(inode_number);
                printf("BINGO %s and inode %d with offset %u\n ", name, inode_number, current_offset);
                break;
            }
            else{
                current_offset+=(record_length);
                lseek(FILE_DESCRIPTOR, current_offset, SEEK_SET); 
            }
        }
    }
    return current_offset;
}