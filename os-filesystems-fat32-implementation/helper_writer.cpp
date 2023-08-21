//
// Created by ugur_ on 15/06/2022.
//
#include "fat32.h"
#include <chrono>
#include <ctime>

uint32_t get_total_available_dentry_number(FS_INFO FS, __off_t dentry_offset){
    uint32_t size = (FS.sectors_per_cluster * FS.bytes_per_sector) / sizeof(FatFileLFN), counter=0;
    for(uint32_t i=0; i<size;i++){
        uint8_t entry_info=0;
        lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
        read(FS.file_descriptor, &entry_info, sizeof(uint8_t));

        if(entry_info== 0x00){
            // free
            counter++;
            dentry_offset+=sizeof(struct_FatFileLFN);
            continue;
        }
        else if (entry_info== 0x2E){
            // dot entry
            dentry_offset+=sizeof(struct_FatFileLFN);
            continue;
        }
        else if (entry_info== 0xE5){
            // erased
            counter++;
            dentry_offset+=sizeof(struct_FatFileLFN);
            continue;
        }
    }
    return counter;
}

Result create_FN_from_filename(std::string file_name){
    Result res;
    std::string capital;
    struct_FatFile83 FN;
    int i=0, j=0;
    for(; i<8; i++){
        if(i+j < file_name.size()){
            FN.filename[i]=toupper(file_name[i]);
            capital.push_back(toupper(file_name[i]));
        }
        else{
            FN.filename[i]=32;
            capital.push_back(' ');
        }
    }
    for(; j<3; j++){
        if(i+j < file_name.size()){
            FN.extension[j]=toupper(file_name[i+j]);
            capital.push_back(toupper(file_name[i+j]));
        }
        else{
            FN.extension[j]=32;
            capital.push_back(' ');
        }
    }
    std::time_t time = std::time(0);
    std::tm* now = std::localtime(&time);
    FN.creationDate=now->tm_mday + ((now->tm_mon +1) << 5) + (((1900 + now->tm_year) - 1980) << 9);
    FN.modifiedDate=now->tm_mday + ((now->tm_mon +1) << 5) + (((1900 + now->tm_year) - 1980) << 9);
    FN.creationTime=(now->tm_min << 5) + (now->tm_hour << 11);
    FN.modifiedTime=(now->tm_min << 5) + (now->tm_hour << 11);
    FN.creationTimeMs=0;
    FN.lastAccessTime=(now->tm_min << 5) + (now->tm_hour << 11);
    FN.attributes= 1 << 5;
    FN.reserved=0;
    FN.eaIndex=0; // will be modified later
    FN.firstCluster=0; // will be modified later
    FN.fileSize=0;

    res.FN = FN;
    res.chcksum = calculate_checksum(capital);
    return res;
}

std::vector<struct_FatFileLFN> create_LFN_from_filename(std::string file_name){
    int lfn_index=0, lfn_entries_needed=0;
    int lfn_length_capacity =13;
    std::vector<struct_FatFileLFN> lst;
    int base=0;
    int first_entry_bit = 64;

    if(file_name.size() <= lfn_length_capacity ){
        // 1 long 1 legacy
        lfn_entries_needed = 1;
    }
    else if((file_name.size() % lfn_length_capacity) == 0){
        // fitting name size, 1 for legacy
        lfn_entries_needed = file_name.size() / lfn_length_capacity;
    }
    else{
        lfn_entries_needed = file_name.size() / lfn_length_capacity + 1;
    }

    for(int i=0;i<lfn_entries_needed;i++){
        struct_FatFileLFN LFN;
        lst.push_back(LFN);
    }

    int index =1;
    for(int ii=lfn_entries_needed-1; ii>=0; ii--){
        struct_FatFileLFN LFN;
        int i=0, j=0, k=0;
        for(; i<5; i++){
            if(base+i+j+k < file_name.size()){
                LFN.name1[i]=(unsigned char) file_name[base+i];
            }
            else{
                LFN.name1[i]=0;
            }
        }
        for(; j<6; j++){
            if(base+i+j+k < file_name.size()){
                LFN.name2[j]=(unsigned char) file_name[base+i+j];
            }
            else{
                LFN.name2[j]=0;
            }
        }
        for(; k<2; k++){
            if(base+i+j+k < (file_name.size())){
                LFN.name3[k]=(unsigned char) file_name[base+i+j+k];
            }
            else{
                LFN.name3[k]=0;
            }
        }

        if(ii==0){
            LFN.sequence_number= lfn_entries_needed + first_entry_bit;
        }
        else{
            LFN.sequence_number= lfn_entries_needed - ii;
        }

        LFN.attributes=0x0F;
        LFN.reserved=0;
        LFN.firstCluster=0;

        lst.at(ii)=LFN;
        base+=13;
    }
    return lst;
}

unsigned char calculate_checksum(std::string filename)
{
    int i,j=0;
    unsigned char sum = 0;

    for (i = 11; i; i--, j++)
        sum = ((sum & 1) << 7) + (sum >> 1) + filename[j];

    return sum;
}

uint allocate_fat_entry(FS_INFO FS, uint previous_last_entry_no, bool is_addition) {
    __off_t entry_size = (FS.sectors_per_cluster * FS.bytes_per_sector) / 32;
    uint32_t fat_entry=0, first_available_entry_no=0;
    uint index=0;
    uint head_entry_no =0;
    lseek(FS.file_descriptor, FS.fat_base_offset + previous_last_entry_no * 4, SEEK_SET);
    read(FS.file_descriptor, &fat_entry, 4);

    lseek(FS.file_descriptor, FS.fat_base_offset, SEEK_SET);
    read(FS.file_descriptor, &first_available_entry_no, 4);

    while(first_available_entry_no > 0) {
        head_entry_no+=4;
        read(FS.file_descriptor, &first_available_entry_no, 4);
    }

    fat_entry=0x0FFFFFFF;
    // change the last entry of the sequence
    lseek(FS.file_descriptor, FS.fat_base_offset + head_entry_no, SEEK_SET);
    write(FS.file_descriptor, &fat_entry, 4);

    head_entry_no = head_entry_no / 4;
    if(is_addition){
        lseek(FS.file_descriptor, FS.fat_base_offset + previous_last_entry_no * 4, SEEK_SET);
        write(FS.file_descriptor, &head_entry_no, 4);
    }
    return head_entry_no;
}

void register_directory_entry(std::vector<std::string> path, FS_INFO FS, Input input, Result result, std::string file_name){
    Result resu;
    Input inpu{.command = CD, .is_directory=true};
    std::vector<std::string> temp_path = path;
    temp_path.pop_back();
    resu = is_path_valid(temp_path, FS, inpu, resu);



    __off_t head_entry_no = resu.head_entry_no;
    __off_t cluster_size = FS.sectors_per_cluster * FS.bytes_per_sector;

    __off_t dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                                 (head_entry_no - 2) * FS.sectors_per_cluster * FS.bytes_per_sector;
    __off_t head_entry_offset = head_entry_offset =
                                        FS.fat_base_offset + head_entry_no * FS.address_size; // skip initial 2 entries to get the root entry



    __off_t offset=0, fat_entry=0;
    int lfn_index=0, lfn_entries_needed=0;
    int lfn_length_capacity =13;


    Result ret = create_FN_from_filename(file_name);
    struct_FatFile83 FN = ret.FN;
    uint chcksum = ret.chcksum;

    std::vector<struct_FatFileLFN> lfn_entries = create_LFN_from_filename(file_name);

    while(lfn_index < lfn_entries.size()){

        lseek(FS.file_descriptor, head_entry_offset, SEEK_SET);
        read(FS.file_descriptor, &fat_entry, 4);
        //head_entry_no=((head_entry_offset-FS.fat_base_offset) / FS.address_size) - 2;

        dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                             (head_entry_no - 2 ) * FS.sectors_per_cluster * FS.bytes_per_sector;

        // write attempt
        while (offset < cluster_size) {
            if(lfn_index == lfn_entries.size())
                break;

            uint8_t entry_info = 0;
            lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
            read(FS.file_descriptor, &entry_info, sizeof(uint8_t));

            if (entry_info == 0x00) {
                // free

                //struct_FatFileLFN LFN = create_LFN_from_filename(lfn_index * 13, file_name);
                struct_FatFileLFN LFN = lfn_entries[lfn_index];
                LFN.checksum = chcksum;
                lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
                write(FS.file_descriptor, &LFN, sizeof(struct_FatFileLFN));
                lfn_index++;
                offset += sizeof(struct_FatFileLFN);


            } else if (entry_info == 0x2E) {
                // dot entry
                offset += sizeof(struct_FatFileLFN);
                continue;
            } else if (entry_info == 0xE5) {
                // erased

                //struct_FatFileLFN LFN = create_LFN_from_filename(lfn_index * 13, file_name);
                struct_FatFileLFN LFN = lfn_entries[lfn_index];
                LFN.checksum = chcksum;
                lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
                write(FS.file_descriptor, &LFN, sizeof(struct_FatFileLFN));
                lfn_index++;
                offset += sizeof(struct_FatFileLFN);

            } else {
                // in use
                offset += sizeof(struct_FatFileLFN);
                continue;
            }
        }

        if(input.command==TOUCH){
            //uint16_t cluster = allocate_fat_entry(FS, head_entry_no, false);
            uint16_t  cluster = result.head_entry_no -2;
            FN.eaIndex = cluster / 65535;
            FN.firstCluster = cluster % 65535;
            lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
            write(FS.file_descriptor, &FN, sizeof(struct_FatFile83));
        }
        else if (input.command == MKDIR){
            uint cluster = allocate_fat_entry(FS, 0, false);
            FN.eaIndex = cluster / 65535;
            FN.firstCluster = cluster % 65535;
            FN.attributes += (1 << 4); // assign subdirectory bit
            lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
            write(FS.file_descriptor, &FN, sizeof(struct_FatFile83));

            __off_t new_dir_offset = FS.root_base_offset +
                                     (cluster - 2 ) * FS.sectors_per_cluster * FS.bytes_per_sector;
            lseek(FS.file_descriptor, new_dir_offset, SEEK_SET);

            Result dot = create_FN_from_filename(".");
            dot.FN.firstCluster=FN.firstCluster;
            dot.FN.attributes += (1 << 4);
            write(FS.file_descriptor, &dot, sizeof(struct_FatFile83));

            Result dotdot = create_FN_from_filename("..");
            dotdot.FN.firstCluster=head_entry_no;
            dotdot.FN.attributes += (1 << 4);
            write(FS.file_descriptor, &dotdot, sizeof(struct_FatFile83));
        }

        if(lfn_index == lfn_entries.size())
            break;
        else if(fat_entry >= 0x0FFFFF8){
            // allocate new fat entry
            head_entry_no = allocate_fat_entry(FS, head_entry_no, true);
            head_entry_offset = head_entry_no * cluster_size;
            offset=0;
        }
    }
}