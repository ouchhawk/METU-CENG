#ifndef HW3_FAT32_H
#define HW3_FAT32_H

#include <sys/types.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <time.h>

// Bytes per sector is fixed at 512 in this homework.
#define BPS 512

#pragma pack(push, 1)
// Starting at offset 36 into the BIOS Parameter Block (BRB) for FAT32
typedef struct struct_BPBFAT32_struct {
    uint32_t FATSize;              // Logical sectors per FAT. Size: 4 bytes
    uint16_t ExtFlags;             // Drive description/Mirroring flags. Size: 2 bytes
    uint16_t FSVersion;            // Version. Size: 2 bytes.
    uint32_t RootCluster;          // Cluster number of root directory start. Typically it is two. Size: 4 bytes.
    uint16_t FSInfo;               // Logical sector number of FS Information sector on FAT32. Size: 2 bytes
    uint16_t BkBootSec;            // First logical sector number of a copy of the three FAT32 boot sectors. It is typically 6. Size: 2 bytes
    uint8_t Reserved[12];          // Reserved bytes. (Previously used by MS-DOS utility FDISK). Size: 12 byte
    uint8_t BS_DriveNumber;        // Physical drive number. Size: 1 byte
    uint8_t BS_Reserved1;          // Reserved. Used for various purposes on FAT12/16 systems. Size: 1 byte
    uint8_t BS_BootSig;            // Boot signature. 0x26 for FAT12/16, 0x29 for FAT32. Size: 1 byte
    uint32_t BS_VolumeID;          // Volume ID. (Mostly for FAT12/16). Size: 4 bytes
    uint8_t BS_VolumeLabel[11];    // Volume Name. (Not really important). Size: 11 bytes.
    uint8_t BS_FileSystemType[8];  // File system type. Padded with spaces at the end. In our case it will be "FAT32   ". Size: 8 bytes
} BPB32_struct;

typedef struct struct_BPB_struct {

    uint8_t BS_JumpBoot[3];        // Jump Instruction. Size: 3 bytes
    uint8_t BS_OEMName[8];         // The system that formatted the disk. Size: 8 bytes
    uint16_t BytesPerSector;       // Bytes per logical sector (It is always will be 512 in our case). Size: 2 bytes
    uint8_t SectorsPerCluster;     // Logical sectors per cluster in the order of two. Size: 1 byte
    uint16_t ReservedSectorCount;  // Count of reserved logical sectors. Size: 2 bytes
    uint8_t NumFATs;               // Number of file allocation tables. Default value is two but can be higher. Size: 1 byte
    uint16_t RootEntryCount;       // Maximum number of FAT12 or FAT16 directory entries. It is 0 for FAT32. Size: 2 bytes
    uint16_t TotalSectors16;       // Total logical sectors. It is 0 for FAT32. Size: 2 bytes
    uint8_t Media;                 // Media descriptor. Size: 1 byte
    uint16_t FATSize16;            // Logical sectors per FAT for FAT12/FAT16. It is 0 for FAT32. Size: 2 bytes
    uint16_t SectorsPerTrack;      // Not relevant
    uint16_t NumberOfHeads;        // Not relevant
    uint32_t HiddenSectors;        // Not relevant
    uint32_t TotalSectors32;       // Total logical sectors including the hidden sectors
    BPB32_struct extended;         // Extended parameters for FAT32
} BPB_struct;

typedef struct struct_FatFile83 {
    uint8_t filename[8];           // Filename for short filenames. First byte have special values.
    uint8_t extension[3];          // Remaining part used for file extension
    uint8_t attributes;            // Attributes
    uint8_t reserved;              // Reserved to mark extended attributes
    uint8_t creationTimeMs;        // Creation time down to ms precision
    uint16_t creationTime;         // Creation time with H:M:S format
    uint16_t creationDate;         // Creation date with Y:M:D format
    uint16_t lastAccessTime;       // Last access time
    uint16_t eaIndex;              // Used to store first two bytes of the first cluster
    uint16_t modifiedTime;         // Modification time with H:M:S format
    uint16_t modifiedDate;         // Modification date with Y:M:D format
    uint16_t firstCluster;         // Last two bytes of the first cluster
    uint32_t fileSize;             // Filesize in bytes
} FatFile83;

// The long filename information can be repeated as necessary before the original 8.3 filename entry
typedef struct struct_FatFileLFN {
    uint8_t sequence_number;
    uint16_t name1[5];      // 5 Chars of name (UTF-16 format)
    uint8_t attributes;     // Always 0x0f
    uint8_t reserved;       // Always 0x00
    uint8_t checksum;       // Checksum of DOS Filename. Can be calculated with a special formula.
    uint16_t name2[6];      // 6 More chars of name (UTF-16 format)
    uint16_t firstCluster;  // Always 0x0000
    uint16_t name3[2];      // 2 More chars of name (UTF-16 format)
} FatFileLFN;

typedef union struct_FatFileEntry {
    FatFile83 msdos;
    FatFileLFN lfn;
} FatFileEntry;

#pragma pack(pop)

// MY CUSTOM DEFINITIONS

typedef enum command_type {
    CD,
    LS,
    LSL,
    MKDIR,
    TOUCH,
    MV,
    CAT
}command_type;

typedef struct struct_Input {
    command_type command;
    bool is_directory;
} Input;

typedef struct struct_FileSummary {
    std::string name;
    uint32_t size;
    uint16_t modified_time;
    uint16_t modified_date;
    bool is_directory;
} FileSummary;


typedef struct struct_Result {
    struct_FatFile83 FN;
    uint chcksum;
    __off_t head_entry_no;
    __off_t head_offset;
    bool is_path_valid;
    bool is_direct_access;
    __off_t data_cluster_no;
    __off_t mv_offset1;
    __off_t mv_offset2;
} Result;

typedef struct struct_Directory {
    std::vector<std::string> path;
    __off_t head_entry_no;
    __off_t head_offset;
} Directory;

typedef struct struct_FS_INFO {
    int file_descriptor;
    uint8_t sectors_per_cluster;
    uint16_t bytes_per_sector;
    uint32_t root_data_sector;
    uint32_t fat_sector;
    off_t root_base_offset;
    off_t fat_base_offset;
    uint32_t fat_size;
    int address_size;
} FS_INFO;

// printer functions
void print_BPB(struct_BPB_struct BPB);
void print_BPB_extended(struct_BPBFAT32_struct BPB_extended);
void print_path(std::vector<std::string> path);
void print_file_details(std::vector<FileSummary> file_list, command_type command);
void print_date_time(uint16_t date, uint16_t time);
void print_file_content(Result result, FS_INFO FS);

// reader functions
bool is_directory(struct_FatFileLFN dentry);
bool is_directory(struct_FatFile83 dentry);
bool is_long_entry(__off_t entry_offset, FS_INFO FS);
bool has_checksum_integrity();
Result is_path_valid(std::vector<std::string> path, FS_INFO FS, Input input, Result result);

// writer functions
void register_directory_entry(std::vector<std::string> path, FS_INFO FS, Input input, Result result, std::string file_name);
uint allocate_fat_entry(FS_INFO FS, uint head_entry_no, bool is_addition);
unsigned char calculate_checksum(std::string filename);
std::vector<struct_FatFileLFN> create_LFN_from_filename(std::string file_name);
Result create_FN_from_filename(std::string file_name);
uint32_t get_total_available_dentry_number(FS_INFO FS, __off_t dentry_offset);
#endif //HW3_FAT32_H
