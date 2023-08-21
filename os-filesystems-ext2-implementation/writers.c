#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ext2fs.h"

int set_first_available_inode_bitmap_bit(){
    u_int32_t bitmap=0;
    u_int32_t updated_bitmap=0;
    u_int32_t inode_bitmap_offset=g_block_group_descriptors->inode_bitmap*BLOCK_SIZE;

    lseek(FILE_DESCRIPTOR, g_block_group_descriptors->inode_bitmap*BLOCK_SIZE, SEEK_SET);
    read(FILE_DESCRIPTOR, &bitmap, sizeof(u_int32_t));

    for(int k=0; k<32; k++){
        if (((bitmap>>k) % 2) == 0){
            updated_bitmap=(bitmap + power(2, k));
            lseek(FILE_DESCRIPTOR, inode_bitmap_offset, SEEK_SET);
            write(FILE_DESCRIPTOR, &updated_bitmap, sizeof(u_int32_t));
            return k+1; // return set inode idm
        }
    }
    return -1;
}

// NOT USED
int allocate_inode(uint32_t inode_number){
    int inode_bitmap_offset=0, inode_table_offset=0, allocated_inode_offset=0;
    u_int32_t* inode_bitmap; 
    u_int32_t* buff = malloc(sizeof(u_int32_t));
    inode_bitmap=malloc(4);
    inode_table_offset=g_block_group_descriptors->inode_table*BLOCK_SIZE;
    inode_bitmap_offset=g_block_group_descriptors->inode_bitmap*BLOCK_SIZE;
    
    for(int i=0; i<g_super_block.inode_count/8; i+=4){ // 1 byte represents 8 inodes, check 1 byte each time
        lseek(FILE_DESCRIPTOR, inode_bitmap_offset+i, SEEK_SET);
        read(FILE_DESCRIPTOR, inode_bitmap, 4);
        *buff=set_first_available_inode_bitmap_bit(*inode_bitmap);

        //printf("\n BUFF %u  \n", *buff);
        //printf("\n INODE BITMAP %d  \n", power(2,0));
        if(*buff != -1){
            lseek(FILE_DESCRIPTOR, inode_bitmap_offset+i, SEEK_SET);
            write(FILE_DESCRIPTOR, buff, 4);
            g_allocated_inode_number+=(i*8);
            

            allocated_inode_offset=inode_table_offset+(g_super_block.inode_size)*(g_allocated_inode_number-1);
            //lseek(FILE_DESCRIPTOR, inode_table_offset+ , SEEK_SET);
            //write(FILE_DESCRIPTOR, buff, 4);
            printf("\n ALLOCATED INODE NO= %u ", g_allocated_inode_number);
            printf("\n ALLOCATED INODE OFFSET= %u", allocated_inode_offset);

            return allocated_inode_offset;

            //write(FILE_DESCRIPTOR, buff, 4);

           
                
                /*if(g_allocated_inode_number == 32){
                *buff=4294967294;
                lseek(FILE_DESCRIPTOR, inode_bitmap_offset+i, SEEK_SET);
                write(FILE_DESCRIPTOR, buff, 4);
                return -2;
            } 
            else {
                //*buff=32643;
                lseek(FILE_DESCRIPTOR, inode_bitmap_offset+i, SEEK_SET);
                write(FILE_DESCRIPTOR, buff, 4);
                return -2;
            }
            */
        }
    }
    return -2;
}

struct ext2_inode write_inode(struct ext2_inode inode, uint32_t target_inode_id){
    u_int32_t target_inode_offset = g_block_group_descriptors->inode_table*BLOCK_SIZE + (target_inode_id-1)*INODE_SIZE;                       
    lseek(FILE_DESCRIPTOR, target_inode_offset, SEEK_SET);
    write(FILE_DESCRIPTOR, &inode.mode, sizeof(u_int16_t));
    write(FILE_DESCRIPTOR, &inode.uid, sizeof(u_int16_t));
    write(FILE_DESCRIPTOR, &inode.size, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.access_time, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.creation_time, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.modification_time, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.deletion_time, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.gid, sizeof(u_int16_t));
    write(FILE_DESCRIPTOR, &inode.link_count, sizeof(u_int16_t));
    write(FILE_DESCRIPTOR, &inode.block_count_512, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.flags, sizeof(u_int32_t));
    write(FILE_DESCRIPTOR, &inode.reserved, sizeof(u_int32_t));
}

/*struct ext2_dir_entry append_directory_entry(struct ext2_dir_entry *parent_directory, uint32_t parent_inode_number){
    
    struct ext2_dir_entry *temp;
    temp=parent_directory->directory_list;

    while(temp != NULL){
        

        temp=temp->next;
    }

    lseek(FILE_DESCRIPTOR, offset , SEEK_SET);
    read(FILE_DESCRIPTOR, &inode->mode, sizeof(u_int16_t));
    
    for(uint32_t head=DATA_BLOCK; head<=g_super_block.block_count*BLOCK_SIZE; head+=BLOCK_SIZE){

    }

    
}*/