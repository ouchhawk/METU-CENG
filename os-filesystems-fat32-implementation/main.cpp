#include "fat32.h"

std::vector<std::string> path_parser(std::string path){
    std::vector<std::string> ret;
    std::string dir;

    path.push_back('/'); // since given paths won't end with '/'
    for(int i=0; i<path.size(); i++){
        if(path.at(i)=='/'){
            if(i==0){
                continue;
            }
            else{
                ret.push_back(dir);
                dir.clear();
                continue;
            }
        }
        else{
            dir.push_back(path.at(i));
        }
    }
    return ret;
}

std::vector<std::string> path_connector(struct_Directory dir, std::string path){
    std::vector<std::string> ret, parsed_path= path_parser(path);
    if(path.size()>0){
        if(path[0]=='/'){
            return path_parser(path);
        }
        else if(path.size()==1){
            if( path[0]=='.'){
                return dir.path; // change nothing
            }
            else{
                for(int i=0; i<dir.path.size();i++){ // get the current path
                    ret.push_back(dir.path.at(i));
                }
                for(int i=0; i<parsed_path.size();i++){ // attach the given path
                    ret.push_back(parsed_path.at(i));
                }
                return ret;
            }
        }
        else if(path.size()==2){
            if(path[0]=='.' && path[1]=='.'){
                if(dir.path.size()==0){
                    return dir.path; // change nothing
                }
                else{
                    for(int i=0; i<dir.path.size()-1;i++){ // discard the last folder to get the parent path
                        ret.push_back(dir.path.at(i));
                    }
                    return ret;
                }
            }
            else if(path[0]=='.' && path[1]=='/'){
                return dir.path; // change nothing
            }
            else{
                for(int i=0; i<dir.path.size();i++){ // get the current path
                    ret.push_back(dir.path.at(i));
                }
                for(int i=0; i<parsed_path.size();i++){ // attach the given path
                    ret.push_back(parsed_path.at(i));
                }
                return ret;
            }
        }
        else if(path.size()>2){ // size check to prevent seg fault
            if(path[0]=='.' && path[1]=='.' && path[2]=='/'){
                for(int i=0; i<dir.path.size()-1;i++){ // discard the last folder to get the parent path
                    ret.push_back(dir.path.at(i));
                }
                for(int i=1; i<parsed_path.size();i++){ // attach the given path
                    ret.push_back(parsed_path.at(i));
                }
                return ret;
            }
            else if(path[0]=='.' && path[1]=='/'){
                for(int i=0; i<dir.path.size();i++){ // get the current path
                    ret.push_back(dir.path.at(i));
                }
                for(int i=1; i<parsed_path.size();i++){ // attach the given path
                    ret.push_back(parsed_path.at(i));
                }
                return ret;
            }
            else{
                for(int i=0; i<dir.path.size();i++){ // get the current path
                    ret.push_back(dir.path.at(i));
                }
                for(int i=0; i<parsed_path.size();i++){ // attach the given path
                    ret.push_back(parsed_path.at(i));
                }
                return ret;
            }
        }
    }
}

int path_finder(){

}
int path_walker(){

}

int main(int argc, char *argv[])
{
#pragma region DEFINE VARIABLES
    char *FS_IMAGE;
    int FILE_DESCRIPTOR;
    __off_t FAT_OFFSET=0, FAT_CLUSTER=0;
    __off_t ROOT_OFFSET=0, ROOT_CLUSTER=0;
    struct_BPB_struct BPB{};
    struct_Directory current_directory{};
    std::vector<std::string> connected_path;
#pragma endregion

    FS_IMAGE = (char*) malloc(sizeof(char)*(strlen(argv[1])+1));
    strcpy(FS_IMAGE, argv[1]);
    FILE_DESCRIPTOR = open(FS_IMAGE, O_RDWR);
    lseek(FILE_DESCRIPTOR, 0, SEEK_SET);
    read(FILE_DESCRIPTOR, &BPB, sizeof(struct struct_BPB_struct));
    FAT_CLUSTER=BPB.ReservedSectorCount;
    FAT_OFFSET= FAT_CLUSTER * (int) BPB.BytesPerSector;
    ROOT_CLUSTER=BPB.extended.FATSize * BPB.NumFATs + BPB.ReservedSectorCount;
    ROOT_OFFSET= ROOT_CLUSTER * (int) BPB.BytesPerSector;

    FS_INFO FS{
        .file_descriptor=FILE_DESCRIPTOR,
        .sectors_per_cluster=BPB.SectorsPerCluster,
        .bytes_per_sector = BPB.BytesPerSector,
        .root_data_sector=BPB.extended.FATSize * BPB.NumFATs + BPB.ReservedSectorCount,
        .fat_sector=BPB.ReservedSectorCount,
        .root_base_offset= ROOT_OFFSET,
        .fat_base_offset= FAT_OFFSET,
        .fat_size=BPB.extended.FATSize,
        .address_size=4 // 32 bits (4 bytes)
    };

    Result result{ .head_entry_no=2, .head_offset=FS.fat_base_offset + 2*FS.address_size, .is_path_valid=false, .is_direct_access=false };

    //std::cout << FAT_OFFSET << ", " << ROOT_OFFSET << std::endl;
    //print_BPB(BPB);
    //print_BPB_extended(BPB.extended);

    print_path(current_directory.path);
    while(true){
        std::vector<std::string> command;
        std::string ln, temp;
        //std::cin >> command;
        getline(std::cin, ln);

        for(int i=0; i<ln.size(); i++){
            if(ln[i]==' '){
                command.push_back(temp);
                temp.clear();
                continue;
            }
            else{
                temp.push_back(ln[i]);
            }
        }
        command.push_back(temp);

        if(command.at(0).compare("quit") == 0){
            break;
        }
        else if(command.at(0).compare("cd") == 0){
            std::vector<std::string> given_path = path_parser(command.at(1));
            connected_path = path_connector(current_directory, command.at(1));

            Input input{.command = CD, .is_directory=true};
            result.is_direct_access=false;
            result = is_path_valid(connected_path, FS, input, result);

            if(command.at(1).compare("/") == 0){
                print_path(connected_path);
                connected_path.clear();
                current_directory.path=connected_path;
                current_directory.head_entry_no=2;
                current_directory.head_offset=FS.fat_base_offset + 2*FS.address_size;
            }
            else if(result.is_path_valid) {
                print_path(connected_path);
                current_directory.path=connected_path;
                current_directory.head_entry_no=result.head_entry_no;
                current_directory.head_offset=result.head_offset;
            }
            else{
                print_path(current_directory.path);
            }
            connected_path.clear();
        }
        else if(command.at(0).compare("ls") == 0){
            if(command.size()==1){
                Input input{.command = LS, .is_directory=true};
                is_path_valid(current_directory.path, FS, input, result);
                print_path(current_directory.path);
                connected_path.clear();
            }
            else if(command.size()==2){
                if(command.at(1)=="-l"){
                    Input input{.command = LSL, .is_directory=true};
                    is_path_valid(current_directory.path, FS, input, result);
                    print_path(current_directory.path);
                    connected_path.clear();
                }
                else{
                    std::vector<std::string> given_path = path_parser(command.at(1));
                    connected_path = path_connector(current_directory, command.at(1));
                    Input input{.command = LS, .is_directory=true};
                    result.is_direct_access=false;
                    result = is_path_valid(connected_path, FS, input, result);

                    if(result.is_path_valid) {
                        print_path(connected_path);
                    }
                    else{
                        print_path(current_directory.path);
                    }
                    connected_path.clear();
                }
            }
            else if(command.size()==3){
                if(command.at(1)=="-l"){
                    Input input{.command = LSL, .is_directory=true};
                    std::vector<std::string> given_path = path_parser(command.at(2));
                    connected_path = path_connector(current_directory, command.at(2));
                    result.is_direct_access=true;
                    is_path_valid(connected_path, FS, input,result);
                    print_path(current_directory.path);
                    connected_path.clear();
                }
            }
        }
        else if(command.at(0).compare("cat") == 0) {
            if(command.size()==2){
                std::vector<std::string> given_path = path_parser(command.at(1));
                connected_path = path_connector(current_directory, command.at(1));
                Input input{.command = CAT, .is_directory=true};
                result.is_direct_access=true;
                result.is_path_valid=false;
                result = is_path_valid(connected_path, FS, input, result);

                if(result.is_path_valid) {
                    print_file_content(result, FS);
                    print_path(current_directory.path);
                }
                else{
                    print_path(current_directory.path);
                }
                connected_path.clear();
            }
        }
        else if(command.at(0).compare("touch") == 0 || command.at(0).compare("mkdir") == 0 ) {
            if(command.size()==2){
                Input input;
                std::vector<std::string> given_path = path_parser(command.at(1));
                connected_path = path_connector(current_directory, command.at(1));

                if(command.at(0).compare("touch") == 0){
                    input.command = TOUCH;
                    input.is_directory=false;
                }
                else if(command.at(0).compare("mkdir") == 0){
                    input.command = MKDIR;
                    input.is_directory=true;
                }

                result.is_direct_access=true;
                result.is_path_valid=true;
                result = is_path_valid(connected_path, FS, input, result);

                if(result.is_path_valid) {
                    register_directory_entry(connected_path, FS, input, result, connected_path.back());
                    print_path(current_directory.path);
                }
                else{
                    print_path(current_directory.path);
                }
                connected_path.clear();
            }
        }
        else if(command.at(0).compare("mv") == 0) {
            if(command.size()==3){
                std::vector<std::string> connected_path1 = path_connector(current_directory, command.at(1));
                std::vector<std::string> connected_path2 = path_connector(current_directory, command.at(2));

                Input input1{.command = MV, .is_directory=false};
                Input input2{.command = MV, .is_directory=true};

                Result result1;
                result1.is_direct_access=true;
                result1 = is_path_valid(connected_path1, FS, input1, result1);
                Result result2 = is_path_valid(connected_path2, FS, input2, result2);


                // local variables
                __off_t dentry_offset = result1.mv_offset1;
                char erase_byte = 0xE5;
                std::vector<struct_FatFileLFN> list_to_be_moved;

                // store and erase part
                while (is_long_entry(dentry_offset, FS)) {
                    struct_FatFileLFN entry;
                    lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                    read(FS.file_descriptor, &entry, sizeof(struct_FatFileLFN));
                    list_to_be_moved.push_back(entry); // store entries

                    lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                    write(FS.file_descriptor, &erase_byte, 1); // erase lfn entries
                    dentry_offset+=sizeof (struct_FatFile83);
                }
                struct_FatFileLFN entry;
                lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                read(FS.file_descriptor, &entry, sizeof(struct_FatFileLFN));
                list_to_be_moved.push_back(entry);

                lseek(FS.file_descriptor, dentry_offset, SEEK_SET);
                write(FS.file_descriptor, &erase_byte, 1); // erase legacy entry

                // find head entry of destination
                Result resu;
                Input inpu{.command = CD, .is_directory=true};
                std::vector<std::string> temp_path = connected_path2;

                resu = is_path_valid(temp_path, FS, inpu, resu);
                __off_t offset =0;

                __off_t head_entry_no = resu.head_entry_no;
                __off_t cluster_size = FS.sectors_per_cluster * FS.bytes_per_sector;
                __off_t dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                                             (head_entry_no - 2) * FS.sectors_per_cluster * FS.bytes_per_sector;
                __off_t head_entry_offset = head_entry_offset =
                                                    FS.fat_base_offset + head_entry_no * FS.address_size;

                // write to new location
                while (offset < cluster_size) {
                    uint8_t entry_info = 0;
                    lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
                    read(FS.file_descriptor, &entry_info, sizeof(uint8_t));

                    if (entry_info == 0x00) {
                        // free
                        lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
                        for(int i=0; i<list_to_be_moved.size(); i++){
                            write(FS.file_descriptor, &list_to_be_moved.at(i), sizeof(struct_FatFileLFN)); // for legacy entry
                            offset += sizeof(struct_FatFileLFN);
                            if(offset==cluster_size){
                                struct_FatFile83 FN;
                                uint cluster = allocate_fat_entry(FS, head_entry_no, false);
                                dentry_base_offset = FS.root_data_sector * FS.bytes_per_sector +
                                                     (cluster - 2) * FS.sectors_per_cluster * FS.bytes_per_sector;
                                offset=0;
                                lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
                            }
                        }
                        break;
                    } else if (entry_info == 0x2E) {
                        // dot entry
                        offset += sizeof(struct_FatFileLFN);
                        continue;
                    } else if (entry_info == 0xE5) {
                        // erased
//                        lseek(FS.file_descriptor, dentry_base_offset + offset, SEEK_SET);
//                        for(int i=0; i<list_to_be_moved.size(); i++){
//                            write(FS.file_descriptor, &list_to_be_moved.at(i), sizeof(struct_FatFileLFN)); // for legacy entry
//                        }
//                        break;
                        offset += sizeof(struct_FatFileLFN);
                        continue;

                    } else {
                        // in use
                        offset += sizeof(struct_FatFileLFN);
                        continue;
                    }
                }
                print_path(current_directory.path);
                connected_path.clear();
                connected_path1.clear();
                connected_path2.clear();
            }
        }
    }
}