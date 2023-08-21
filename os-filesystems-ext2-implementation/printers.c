#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ext2fs.h"

void print_block_group_descriptor(struct ext2_block_group_descriptor block_group_descriptor){
    printf("block bitmap block: %d \n", block_group_descriptor.block_bitmap);
    printf("inode bitmap block: %d \n", block_group_descriptor.inode_bitmap);
    printf("inode table(first block of the inode table): %d \n", block_group_descriptor.inode_table);
    printf("free block count: %d \n", block_group_descriptor.free_block_count);
    printf("free inode count: %d \n", block_group_descriptor.free_inode_count);
    printf("used directories count: %d \n", block_group_descriptor.used_dirs_count);
    printf("padding to alingment: %d \n", block_group_descriptor.pad);
}

void print_directory_entry(struct ext2_dir_entry *directory_entry){
    printf("inode number: %d \n", directory_entry->inode);
    printf("record length: %d \n", directory_entry->length);
    printf("name length: %d \n", directory_entry->name_length);
    printf("file type: %d \n", directory_entry->file_type);
    printf("file name char array: %s \n", directory_entry->name);
}

void print_super_block(struct ext2_super_block super_block){
    printf("inode count: %d \n", super_block.inode_count);
    printf("block count: %d \n", super_block.block_count);
    printf("free block count: %d \n", super_block.free_block_count);
    printf("free inode count: %d \n", super_block.free_inode_count);
    printf("first data block: %d \n", super_block.first_data_block);
    printf("log block size: %d \n", super_block.log_block_size);
    printf("blocks per group: %d \n", super_block.blocks_per_group);
    printf("inodes per group: %d \n", super_block.inodes_per_group);
    printf("first non reserved inode: %d \n", super_block.first_inode);
    printf("inode size: %d \n", super_block.inode_size);
    printf("block size: %d \n", BLOCK_SIZE);
}

void print_inode(struct ext2_inode inode){
    printf("mode: %u \n", inode.mode);
    printf("uid: %u \n", inode.uid);
    printf("size: %u \n", inode.size);
    printf("access time: %u \n", inode.access_time);
    printf("creation time: %u \n", inode.creation_time);
    printf("modification time: %u \n", inode.modification_time);
    printf("gid: %u \n", inode.gid);
    printf("hard link count: %u \n", inode.link_count);
    printf("direct block 0: %d \n", inode.direct_blocks[0]);
    printf("direct block 1: %d \n", inode.direct_blocks[1]);
    printf("direct block 2: %d \n", inode.direct_blocks[2]);
}

//SILINEBILIR ?
void print_inode_with_id(int inode_id){
    struct ext2_inode inode;
    uint32_t block_group=((inode_id-1) / g_super_block.inodes_per_group)+1;
    uint32_t inode_number=((inode_id-1) % g_super_block.inodes_per_group)+1;
    uint32_t base_offset=g_block_group_descriptors[block_group-1].inode_table*BLOCK_SIZE;
    uint32_t offset=base_offset+(inode_number-1)*INODE_SIZE;
    printf("BASE OFFSET: %d \n", g_block_group_descriptors[block_group-1].inode_table);
    printf("OFFSET: %d \n", offset);
    lseek(FILE_DESCRIPTOR, offset, SEEK_SET);
    read(FILE_DESCRIPTOR, &inode, sizeof(struct ext2_inode));
    print_inode(inode);
}