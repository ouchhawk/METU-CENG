//
// Created by ugur_ on 15/06/2022.
//
#include "fat32.h"
#include <bits/stdc++.h>

Result is_path_valid(std::vector<std::string> path, FS_INFO FS, Input input, Result result) {
    // empty list means root directory
    if (path.size() == 0 && input.command == CD || path.size()==1 && path.at(0)=="") {
        result.is_path_valid = true;
        result.head_entry_no=2;
        return result;
    }

    bool read_legacy_next = false;
    std::vector<FileSummary> file_list;
    int directory_depth = 0;
    uint32_t fat_entry = 0;
    uint max_entry_size = FS.fat_size * FS.bytes_per_sector / 32; // each entry is 32 bytes long in fat32
    uint head_entry_no = 2;
    __off_t head_entry_offset =
            FS.fat_base_offset + head_entry_no * FS.address_size; // skip initial 2 entries to get the root entry
    __off_t dentry_base_offset = 0, dentry_offset = 0;
    struct_FatFileLFN long_file_entry;
    struct_FatFile83 legacy_file_entry;
    FileSummary temp_file_summary;


//    if (result.is_direct_access && (input.command == LS || input.command == LSL)) {
//        head_entry_no = result.head_entry_no;
//        head_entry_offset = result.head_offset;
//    }

    while (fat_entry <= 0x0FFFFFFF) {
        start:

        lseek(FS.file_descriptor, head_entry_offset, SEEK_SET);
        read(FS.file_descriptor, &fat_entry, 4);
        //head_entry_no=((head_entry_offset-FS.fat_base_offset) / FS.address_size) - 2;

        dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                             (head_entry_no - 2) * FS.sectors_per_cluster * FS.bytes_per_sector;
        dentry_offset = dentry_base_offset;

        while ((dentry_offset - dentry_base_offset) < (FS.sectors_per_cluster * FS.bytes_per_sector)) {

            uint entry_info = 0;
            lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
            read(FS.file_descriptor, &entry_info, 1);

            if (entry_info == 0x00) {
                // free
                dentry_offset += sizeof(struct_FatFileLFN);
                continue;
            } else if (entry_info == 0x2E) {
                // dot entry
                dentry_offset += sizeof(struct_FatFileLFN);
                continue;
            } else if (entry_info == 0xE5) {
                // erased
                dentry_offset += sizeof(struct_FatFileLFN);
                continue;
            } else {
                std::list<std::string> name_as_list;
                std::string name;
                __off_t dentry_offset_for_mv=dentry_offset;
                while (is_long_entry(dentry_offset, FS)) {
                    lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                    read(FS.file_descriptor, &long_file_entry, sizeof(struct_FatFileLFN));
                    dentry_offset += sizeof(struct_FatFileLFN);

                    for (int i = 0; i < 5; i++) {
                        if ( long_file_entry.name1[i] < 255 && long_file_entry.name1[i] > 0)
                            name.push_back(char(long_file_entry.name1[i]));
                    }
                    for (int i = 0; i < 6; i++) {
                        if (long_file_entry.name2[i] < 255 && long_file_entry.name1[i] > 0)
                            name.push_back(char(long_file_entry.name2[i]));
                    }
                    for (int i = 0; i < 2; i++) {
                        if (long_file_entry.name3[i] < 255 && long_file_entry.name1[i] > 0)
                            name.push_back(char(long_file_entry.name3[i]));
                    }

                    name_as_list.push_front(name);
                    name.clear();
                }

                for(std::string str : name_as_list){
                    for (int i = 0; i < str.length(); i++) {
                        if(str.at(i) > 0)
                            name.push_back(str.at(i));
                    }
                }

                lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                read(FS.file_descriptor, &legacy_file_entry, sizeof(struct_FatFile83));
                dentry_offset += sizeof(struct_FatFile83);


                if((input.command == LS || input.command == LSL) && path.size() == directory_depth){
                    FileSummary sum;
                    sum.name=name;
                    sum.modified_date=legacy_file_entry.modifiedDate;
                    sum.modified_time=legacy_file_entry.modifiedTime;
                    sum.size=legacy_file_entry.fileSize;
                    sum.is_directory = is_directory(legacy_file_entry);
                    file_list.push_back((sum));
                }

                if (path.size() > 0 && directory_depth < path.size()) {
                    bool name_test = true;
//                    for (int i = 0; i < path.at(directory_depth).size(); i++) {
//                        if (path.at(directory_depth).at(i) != name.at(i)) {
//                            name_test = false;
//                            break;
//                        }
//                    }
                    //char a = name.at(name.size()-1);
                    //char b = path.at(directory_depth).at(path.at(directory_depth).size()-1);

                    name_test = name == path.at(directory_depth);

//                    int o = name.length();
//                    if(input.command==CD || input.command==LS || input.command==LSL || input.command==CAT){
//                        //o--;
//                    }
//                    int u = path.at(directory_depth).length();
//                    if (o != u) {
//                        name_test = false;
//                    }

                    if(input.command==CAT && name_test && directory_depth == path.size()-1){
                        if (legacy_file_entry.fileSize == 0){
                            result.is_path_valid = false;
                            result.data_cluster_no =
                                 legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16) - 2;
                            result.head_entry_no = head_entry_no;
                            result.head_offset = head_entry_offset;
                            return result;
                        }
                        else{
                            result.is_path_valid = true;
                            result.data_cluster_no =
                                    legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16) - 2;
                            result.head_entry_no = head_entry_no;
                            result.head_offset = head_entry_offset;
                            return result;
                        }
                    }

                    if (name_test && input.command == TOUCH && directory_depth == path.size()-1) {
                        // file exists
                        result.is_path_valid = false;
                        return result;
                    } else if (name_test && input.command == MKDIR && directory_depth == path.size() - 1) {
                        // dir exists
                        result.is_path_valid = false;
                        return result;
                    } else if (name_test && input.command == MV && directory_depth == path.size() - 1) {
                        // dir exists
                        result.mv_offset1 = dentry_offset_for_mv;
                        result.mv_offset2 = legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16) - 2;
                        return result;
                    }

                    if (name_test) {
                        if (directory_depth <= (path.size() - 1)) {
                            if (input.command == MV) {

                            }

                            if (input.command == CD) {
                                if (input.is_directory == is_directory(legacy_file_entry)) {
                                    if (directory_depth == path.size() - 1) {
                                        result.head_entry_no = legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                                        result.head_offset = FS.fat_base_offset + 2 * FS.address_size + head_entry_no * FS.address_size;
                                        result.is_path_valid = true;
                                        return result;
                                    }
                                    else {
                                            head_entry_no =
                                                    legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                                            head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
                                            directory_depth++;
                                            result.head_entry_no = head_entry_no;
                                            result.head_offset = head_entry_offset;
                                            goto start;
                                        }
                                    }
                            } else if (input.command == LSL || input.command == LS) {
                                // enter the directory and continue
                                file_list.clear();
                                head_entry_no = legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                                head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
                                directory_depth++;
                                result.head_entry_no = head_entry_no;
                                result.head_offset = head_entry_offset;
                                goto start;

                            } else if (input.command == CAT ) {
                                head_entry_no =
                                        legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                                head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
                                directory_depth++;
                                goto start;
                            }
                            else{
                                head_entry_no = legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                                head_entry_offset =
                                        FS.fat_base_offset + 2 * FS.address_size + head_entry_no * FS.address_size;
                                directory_depth++;
                                goto start;
                            }
                        } else {
                            head_entry_no = legacy_file_entry.firstCluster + (legacy_file_entry.eaIndex << 16);
                            head_entry_offset =
                                    FS.fat_base_offset + 2 * FS.address_size + head_entry_no * FS.address_size;
                            directory_depth++;
                            goto start;
                        }
                    }
                }
            }
        }
        if (fat_entry <= 0x0FFFFFF0) {
            head_entry_no = fat_entry + 2;
            head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
        } else {
            break;
        }

    }
    if (input.command == LSL || input.command == LS) {
        print_file_details(file_list, input.command);
    }
    if (input.command == TOUCH || input.command == MKDIR) {
        result.is_path_valid = true;
        return result;
    }

    result.is_path_valid = false;
    return result;
}


bool is_directory(struct_FatFile83 dentry){
    if((dentry.attributes & 0x10) == 0x10){
        return true;
    }
    return false;
}
bool is_directory(struct_FatFileLFN dentry){
    if((dentry.attributes & 0x10) == 0x10){
        return true;
    }
    return false;
}

bool is_long_entry(__off_t entry_offset, FS_INFO FS){

    uint8_t attribute=0, mask = 0x0F;
    lseek(FS.file_descriptor, entry_offset+11, SEEK_SET);
    read(FS.file_descriptor, &attribute, sizeof(uint8_t));
    if(attribute & mask){
        return true;
    }
    else{
        return false;
    }
}

bool has_checksum_integrity(){
}
