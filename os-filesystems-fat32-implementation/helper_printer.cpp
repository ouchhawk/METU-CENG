//
// Created by ugur_ on 10/06/2022.
//
#include "fat32.h"

void print_BPB(struct_BPB_struct BPB) {
    std::cout << "BS_JumpBoot :" << BPB.BS_JumpBoot << std::endl;
    std::cout << "BS_OEMName :" << BPB.BS_OEMName << std::endl;
    std::cout << "BytesPerSector :" << BPB.BytesPerSector << std::endl;
    std::cout << "SectorsPerCluster :" << (int) BPB.SectorsPerCluster << std::endl;
    std::cout << "ReservedSectorCount :" << BPB.ReservedSectorCount << std::endl;
    std::cout << "NumFATs :" << (int) BPB.NumFATs << std::endl;
    std::cout << "RootEntryCount :" << BPB.RootEntryCount << std::endl;
    std::cout << "TotalSectors16 :" << BPB.TotalSectors16 << std::endl;
    std::cout << "Media :" << (int) BPB.Media << std::endl;
    std::cout << "FATSize16 :" << BPB.FATSize16 << std::endl;
    std::cout << "SectorsPerTrack :" << BPB.SectorsPerTrack << std::endl;
    std::cout << "NumberOfHeads :" << BPB.NumberOfHeads << std::endl;
    std::cout << "HiddenSectors :" << BPB.HiddenSectors << std::endl;
    std::cout << "TotalSectors32 :" << BPB.TotalSectors32 << std::endl;
}

void print_BPB_extended(struct_BPBFAT32_struct BPB_extended) {
    std::cout << "FATSize :" << (BPB_extended.FATSize) << std::endl;
    std::cout << "ExtFlags :" << BPB_extended.ExtFlags << std::endl;
    std::cout << "FSVersion :" << BPB_extended.FSVersion << std::endl;
    std::cout << "RootCluster :" << (int) BPB_extended.RootCluster << std::endl;
    std::cout << "FSInfo :" << BPB_extended.FSInfo << std::endl;
    std::cout << "Reserved :" << BPB_extended.Reserved << std::endl;
    std::cout << "BS_DriveNumber :" << BPB_extended.BS_DriveNumber << std::endl;
    std::cout << "BS_Reserved1 :" << BPB_extended.BS_Reserved1 << std::endl;
    std::cout << "BS_BootSig :" << (int) BPB_extended.BS_BootSig << std::endl;
    std::cout << "BS_VolumeID :" << BPB_extended.BS_VolumeID << std::endl;
    std::cout << "BS_VolumeLabel :" << BPB_extended.BS_VolumeLabel << std::endl;
    std::cout << "BS_FileSystemType :" << BPB_extended.BS_FileSystemType << std::endl;
}

void print_path(std::vector<std::string> path){
    if (path.size()==0 )
        std::cout << '/';

    for(int i=0; i< path.size(); i++){
        std::cout << '/';
        std::cout << path.at(i);
    }
    std::cout << '>';
}

void print_file_details(std::vector<FileSummary> file_list, command_type command){
    time_t current_time;
    struct tm *local_time;
    current_time=time(NULL);
    local_time= localtime(&current_time);
    if(command==LSL){
        for(int i=0; i< file_list.size(); i++){
            if(file_list.at(i).is_directory){
                std::cout << "drwx------ root root ";
                std::cout << "0";
                print_date_time(file_list.at(i).modified_date, file_list.at(i).modified_time);
                std::cout << file_list.at(i).name;
            }
            else{
                std::cout << "-rwx------ root root ";
                std::cout << file_list.at(i).size;
                print_date_time(file_list.at(i).modified_date, file_list.at(i).modified_time);
                std::cout << file_list.at(i).name;
            }
            std::cout << std::endl;
        }
    }
    else if(command==LS){
        for(int i=0; i< file_list.size(); i++){
            std::cout << file_list.at(i).name;
            std::cout << ' ';
        }
        std::cout << std::endl;
    }
}

void print_month(int num){
    switch(num){
        case 1:
            std::cout << "January";
            break;
        case 2:
            std::cout << "February";
            break;
        case 3:
            std::cout << "March";
            break;
        case 4:
            std::cout << "April";
            break;
        case 5:
            std::cout << "May";
            break;
        case 6:
            std::cout << "June";
            break;
        case 7:
            std::cout << "July";
            break;
        case 8:
            std::cout << "August";
            break;
        case 9:
            std::cout << "September";
            break;
        case 10:
            std::cout << "October";
            break;
        case 11:
            std::cout << "November";
            break;
        case 12:
            std::cout << "December";
            break;
        default:
            break;
    }
}

void print_date_time(uint16_t date, uint16_t time){
    uint day_mask = 0x1F;
    uint month_mask = 0x1E0;
    uint min_mask = 0x7E2;
    uint hour_mask= 0xF800;

    std::cout << " ";
    print_month(((date&month_mask)>>5));
    std::cout << " " << (date&day_mask)  << " ";

    if(((time&hour_mask)>>11) < 10){
        std::cout << '0';
    }
    std::cout << ((time&hour_mask)>>11);

    std::cout << ":";
    if(((time&min_mask)>>5) < 10){
        std::cout << '0';
    }
    std::cout <<  ((time&min_mask)>>5) << ' ';

    //std::cout << " " << time << " ";
}

void print_file_content(Result result, FS_INFO FS){
    uint32_t fat_entry = 0;
    FileSummary temp_file_summary;
    __off_t cluster_size = FS.sectors_per_cluster * FS.bytes_per_sector;

    uint head_entry_no = result.data_cluster_no + 2;
    __off_t head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
    __off_t dentry_base_offset, dentry_offset;

    while (fat_entry < 0x0FFFFFF8) {

        lseek(FS.file_descriptor, head_entry_offset, SEEK_SET);
        read(FS.file_descriptor, &fat_entry, 4);

        dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                             (head_entry_no - 2) * FS.sectors_per_cluster * FS.bytes_per_sector;
        dentry_offset = dentry_base_offset;


        lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
        for(__off_t index=0; index<cluster_size; index++){
            char ch;
            read(FS.file_descriptor, &ch, 1);
            if(ch) std::cout << ch;
        }

        head_entry_no = fat_entry;
        head_entry_offset = FS.fat_base_offset + head_entry_no * FS.address_size;
    }


    std::cout << std::endl;
}