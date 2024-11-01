

/**
 **   $Log: stffs_files.c,v $
 **   Revision 1.3.32.4.4.1.2.1  2017/08/14 10:56:57  LPPL253
 **   -Addition on flexcan
 **
 **   Revision 1.3.32.4.4.1  2017/08/14 12:12:31  LPPL253
 **   -Correction in static ip
 **   -Correction in SNMP Walk
 **
 **   Revision 1.3.32.4  2017/01/04 05:50:52  LPPL239
 **   	- Shifted of default from Flash to SerialFlashMemory to access via ethernet
 **   	- BDC alarm bug clearence
 **   	- IPV6 address representation and configuration is made into standard format.
 **   	- Added new BLVD reconnect and disconnect voltage while mains are not available.
 **   	- Added new current charging logic in plrcl.c
 **
 **   Revision 1.3.32.3  2016/05/28 13:09:17  LPPL239
 **   VRLA Battery soc logic added and logo file removed from web
 **
 **   Revision 1.3.32.2  2016/05/09 06:16:31  LPPL239
 **   FIxing his.csv and read in web,saving in serial flash
 **   tested web in reliance on 6 may 2016
 **
 **   Revision 1.3.32.1  2016/04/19 09:57:54  LPPL239
 **   	- Added preprocessors to optimize the code.
 **   	- Warnings were removed and code optimised
 **   	- Added SNMPv3 with NOAuth,fault log his.csv
 **   	- Added new Fnet,dhcpv6,webpages in external.
 **
 **   Revision 1.3  2014/04/09 20:14:49  fr003137
 **   - Correcting the checks for empty config.gal and limiting running config.gal script
 **      based on length rather than on the location of '\n'.
 **
 **   Revision 1.2  2014/03/12 13:47:12  fr003137
 **   - MERGING 0.0.28.c10 into head, latest tag on head at the time: 0.0.28
 **
 **   Revision 1.1.10.2  2014/03/03 09:31:42  fr003253
 **   - Minor modification
 **
 **   Revision 1.1.10.1  2014/03/03 07:07:11  fr003253
 **   - 0.0.28_LC_1 (config.gal) merged to 0.0.28_C branch
 **
 **   Revision 1.1.8.3  2014/01/29 06:56:21  fr003253
 **   - write_complete_callback is now execute
 **   - new variable executing is added
 **
 **   Revision 1.1.8.2  2014/01/27 06:55:48  fr003253
 **   - Global Function to enabled gal file execution
 **
 **   Revision 1.1.8.1  2014/01/17 07:15:25  fr003253
 **   - Incorporating change for config.gal update via ethernet
 **
 **   Revision 1.1  2013/08/07 15:07:38  fr003137
 **   - New Feature:- STFFS (Mini mock flash file system) and remote firmware upgrade via TFTP
 **
 **
 **
 **/

#include <ctype.h>
#include <string.h>  // memcpy
#include "types.h"
#include "st.h"
#include "sysdef.h"
#include "fl.h"
#include "xf.h"
#include "al.h"
#include "sc.h"
//#include "st.h"

// Macros
#define BC_GAL_CMD_EXE_NO 5 //execute number of command in one cycle run
#define STFFS_FILE_OPEN_MIN_TIMEOUT 2 // to be transacted within said time.
#define STFFS_APP_BIN_MIN_TIMEOUT  2 // to be transacted within said time.

#define STFFS_DIR_CSV_SIZE  1024
#define STFFS_EXEC_BIN_SIZE 512
#define STFFS_LCFD_BIN_SIZE 512
#define STFFS_GAL_NO_DATA 0xFF

// Utility Strings, ordered aphabetically
#define STFFS_ACCESS_STR    "Access"
#define STFFS_ACCESS_STR_LEN    6
#define STFFS_BUSY_STR      "Busy"
#define STFFS_BUSY_STR_LEN  4
#define STFFS_CHECKSUM_STR  "Checksum"
#define STFFS_CHECKSUM_STR_LEN  8
#define STFFS_CHECKSUM_NEEDED_STR   "Checksum Needed"
#define STFFS_CHECKSUM_NEEDED_STR_LEN   15
#define STFFS_CHECKSUM_NOT_NEEDED_STR   "Checksum Not Needed"
#define STFFS_CHECKSUM_NOT_NEEDED_STR_LEN   19
//------------------------------------
#define STFFS_GAL_DELIMITER '$'
#define STFFS_GAL_DELIMITER_OFFSET 2
#define STFFS_GAL_DELIMITER_STR  "\n\r$$$#\n\r"
#define STFFS_GAL_DELIMITER_STR_LEN 8
//------------------------------------
#define STFFS_FILENAME_STR  "Filename"
#define STFFS_FILENAME_STR_LEN  8
#define STFFS_FILESIZE_STR  "Filesize(in Hex)"
#define STFFS_FILESIZE_STR_LEN  16
#define STFFS_INFO_STR  "Info"
#define STFFS_INFO_STR_LEN  4
#define STFFS_CLOSED_STR     "Closed"
#define STFFS_CLOSED_STR_LEN 6
#define STFFS_MOUNTED_STR   "Mounted"
#define STFFS_MOUNTED_STR_LEN   7
#define STFFS_NEWLINE_STR "\n"
#define STFFS_NEWLINE_STR_LEN   1
#define STFFS_OPEN_STR     "Open"
#define STFFS_OPEN_STR_LEN 4
#define STFFS_READ_ONLY_STR     "ReadOnly"
#define STFFS_READ_ONLY_STR_LEN 8
#define STFFS_READ_WRITE_STR    "ReadWrite"
#define STFFS_READ_WRITE_STR_LEN    9
#define STFFS_UNKNOWN_STR "Unknown"
#define STFFS_UNKNOWN_STR_LEN   7
#define STFFS_UPGRADE_STR   "upgrade"
#define STFFS_UPGRADE_STR_LEN   7
#define STFFS_LCFD_STR   "lcfd"
#define STFFS_LCFD_STR_LEN   4
#define STFFS_WRITE_ONLY_STR    "WriteOnly"
#define STFFS_WRITE_ONLY_STR_LEN    9

#define STFFS_MOVE_INDEX(data, index, len)  (index)+=(len);(data)[(index)]=0;
#define STFFS_ADD_COMMA_AND_MOVE_INDEX(data, index) (data)[(index)]=','; (index)++;(data)[(index)]=0;

static enum stffs_dir_csv_col_e
{
    STFFS_DIR_CSV_COL_FIRST,
    STFFS_DIR_CSV_COL_ACCESS=STFFS_DIR_CSV_COL_FIRST,
    STFFS_DIR_CSV_COL_FILENAME,
    STFFS_DIR_CSV_COL_FILESIZE,
    STFFS_DIR_CSV_COL_CHECKSUM,
    STFFS_DIR_CSV_COL_MOUNTED,
    STFFS_DIR_CSV_COL_OPEN,
    STFFS_DIR_CSV_COL_INFO,
    STFFS_NUM_DIR_CSV_COLS
};

// Variables

#pragma define_section binary_images ".xram_bss" far_absolute RW
__declspec(binary_images) UINT8 stffs_ram_image_boot_bin[BOOT_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_defaults_bin[DEFAULTS_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_dir_csv[STFFS_DIR_CSV_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_exec_bin[STFFS_EXEC_BIN_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_lcfd_bin[STFFS_LCFD_BIN_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_app_bin[APP_SIZE];
__declspec(binary_images) UINT8 stffs_ram_image_bc_gal[STFFS_BC_GAL_FILE_SIZE];


#pragma define_section hisimage ".xram_bss" far_absolute RW
__declspec(hisimage) UINT8 stffs_ram_image_his_csv[STFFS_HIS_CSV_SIZE];
__declspec(hisimage) UINT8 stffs_ram_image_sum_csv[STFFS_SUM_CSV_SIZE];

typedef struct
{
    char *str;
    UINT8 len;
}stffs_str_len_t;

UINT32 bc_gal_cmd_size = 0;

// should match the enum stffs_dir_columns_e sequence
static const stffs_str_len_t stffs_dir_csv_columns[STFFS_NUM_DIR_CSV_COLS]=
{
    {STFFS_ACCESS_STR,  STFFS_ACCESS_STR_LEN},
    {STFFS_FILENAME_STR,STFFS_FILENAME_STR_LEN},
    {STFFS_FILESIZE_STR,STFFS_FILESIZE_STR_LEN},
    {STFFS_CHECKSUM_STR,STFFS_CHECKSUM_STR_LEN},
    {STFFS_MOUNTED_STR, STFFS_MOUNTED_STR_LEN},
    {STFFS_OPEN_STR,    STFFS_OPEN_STR_LEN},
    {STFFS_INFO_STR,    STFFS_INFO_STR_LEN},
};

// should match the enum stffs_access_e sequence
static const stffs_str_len_t stffs_file_access [STFFS_NUM_ACCESSES]=
{
    {STFFS_READ_ONLY_STR,   STFFS_READ_ONLY_STR_LEN},
    {STFFS_WRITE_ONLY_STR,  STFFS_WRITE_ONLY_STR_LEN},
    {STFFS_READ_WRITE_STR,  STFFS_READ_WRITE_STR_LEN}
};

// Protos -- ffs adapters
static UINT8 stffs_dir_csv_read_init_callback(stffs_file_t *fp);
static UINT8 stffs_defaults_execute(stffs_file_t *fp);
static UINT8 stffs_exec_execute(stffs_file_t *fp);
static UINT8 stffs_lcfd_execute(stffs_file_t *fp);
static UINT8 stffs_bc_gal_write_ram_complete_callback(stffs_file_t *fp);
static UINT8 stffs_bc_gal_write_init_callback(stffs_file_t *fp);
static UINT8 stffs_bc_gal_read_init_callback(stffs_file_t *fp);
static UINT8 stffs_hiscsv_write_init_callback(stffs_file_t *fp);
static UINT8 stffs_sumcsv_write_init_callback(stffs_file_t *fp);

static void stffs_fl_read_req_adapter(stffs_file_t *fp);
static void stffs_xf_read_req_adapter(stffs_file_t *fp);
static void stffs_fl_write_req_adapter(stffs_file_t *fp);
static void stffs_xf_write_req_adapter(stffs_file_t *fp);
static void stffs_fl_erase_req_adapter(stffs_file_t *fp);
static void stffs_xf_erase_req_adapter(stffs_file_t *fp);
static UINT8 stffs_fl_req_is_done_adapter(stffs_file_t *fp);
static UINT8 stffs_xf_req_is_done_adapter(stffs_file_t *fp);

static UINT8 pt_proc_cmd_stream_portion(stffs_file_t *fp);
static UINT8 stffs_verify_gal_input(const void *ptr, SINT32 num_bytes);
static UINT8 stffs_verify_gal_char(const char* ptr_data);

//stffs files
stffs_file_t stffs_files[STFFS_NUM_FILES];
stffs_file_t const stffs_files_defaults[STFFS_NUM_FILES]=
{
    {
        STFFS_APP_BIN_NAME, // char  *  name;
        XAPP1_START,        //  UINT32 flash_image;
        stffs_ram_image_app_bin,      // UINT8 *  ram_image;
        APP_SIZE,           //  UINT32 size;
        0,                  //  UINT8 use_alternate_size;
        XF_STFFS_REQ_APP,       //  UINT32 requester;
        STFFS_APP_BIN_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,    //  enum stffs_access_e  access;
        1,                  //  UINT8 confirm_checksum_for_writes;

        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,//enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        0,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
    {
        STFFS_BOOT_BIN_NAME,// char  *  name;
        BOOT_START,         //  UINT32 flash_image;
        stffs_ram_image_boot_bin,     // UINT8 *  ram_image;
        BOOT_SIZE,          //  UINT32 size;
        0,                  //  UINT8 use_alternate_size;
        FL_STFFS_REQ,       //  UINT32 requester;
        STFFS_BOOT_BIN_MIN_TIMER, // enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,//   UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,   //  enum stffs_access_e  access;
        1,                  //  UINT8 confirm_checksum_for_writes;

        stffs_fl_read_req_adapter, //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_fl_write_req_adapter,//  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_fl_erase_req_adapter,//  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_fl_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE, //enum stffs_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        0,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
    {
        STFFS_DEFAULTS_BIN_NAME,// char  *  name;
        START_ADDR_DFLT,         //  UINT32 flash_image;
        stffs_ram_image_defaults_bin,// UINT8 *  ram_image;
        DEFAULTS_SIZE,      //  UINT32 size;
        0,                  //  UINT8 use_alternate_size;
        XF_STFFS_REQ_DEFALTS,       //  UINT32 requester;
        STFFS_DEFAULTS_MIN_TIMER, // enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,//   UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,   //  enum stffs_access_e  access;
        0,//1,                  //  UINT8 confirm_checksum_for_writes;

        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        stffs_defaults_execute,// UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE, //enum stffs_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
    {
        /* special file, always mounted */
        STFFS_DIR_CSV_NAME, // char  *  name;
        0,                  // NOT USED:  UINT32 flash_image;
        stffs_ram_image_dir_csv,// NOT USED: UINT8 *  ram_image;
        STFFS_DIR_CSV_SIZE, //  UINT32 size;
        1,                  //  UINT8 use_alternate_size;
        0,                  // NOT USED:  UINT32 requester;
        STFFS_DIR_CSV_MIN_TIMER,// enum st_min_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // NOT USED:  UINT8 confirm_checksum_for_writes;

        NULL,               //  void (*fl_read_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_write_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_erase_req)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        stffs_dir_csv_read_init_callback, // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_IDLE,   // NOT USED: enum stffs_file_state_e state;
        STFFS_MODE_FIRST,   // NOT USED: enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum;
    },
    {
        /* special file, always mounted */
        STFFS_EXEC_BIN_NAME, // char  *  name;
        0,                  // NOT USED:  UINT32 flash_image;
        stffs_ram_image_exec_bin,// NOT USED: UINT8 *  ram_image;
        STFFS_EXEC_BIN_SIZE, //  UINT32 size;
        1,                  //  UINT8 use_alternate_size;
        0,                  // NOT USED:  UINT32 requester;
        STFFS_EXEC_BIN_MIN_TIMER,// enum st_min_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_WRITE_ONLY,    //  enum stffs_access_e  access;
        0,                  // NOT USED:  UINT8 confirm_checksum_for_writes;

        NULL,               //  void (*fl_read_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_write_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_erase_req)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        stffs_exec_execute,// UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_IDLE,   // NOT USED: enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,   // NOT USED: enum stffs_mode_e mode;
        STFFS_UPGRADE_STR_LEN,// UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum;
    },
    {
        /* special file, always mounted */
        STFFS_LCFD_BIN_NAME, // char  *  name;
        0,                  // NOT USED:  UINT32 flash_image;
        stffs_ram_image_lcfd_bin,// NOT USED: UINT8 *  ram_image;
        STFFS_LCFD_BIN_SIZE, //  UINT32 size;
        1,                  //  UINT8 use_alternate_size;
        0,                  // NOT USED:  UINT32 requester;
        STFFS_LCFD_BIN_MIN_TIMER,// enum st_min_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_WRITE_ONLY,    //  enum stffs_access_e  access;
        0,                  // NOT USED:  UINT8 confirm_checksum_for_writes;

        NULL,               //  void (*fl_read_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_write_req)(struct stffs_file_s *fp);
        NULL,               //  void (*fl_erase_req)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        stffs_lcfd_execute,// UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_IDLE,   // NOT USED: enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,   // NOT USED: enum stffs_mode_e mode;
        STFFS_LCFD_STR_LEN,// UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum;
    },
    {
        /* special file, always mounted */
        STFFS_EXEC_BC_GAL_NAME, // char  *  name;
        X_BC_GAL_FILE_START,    // NOT USED:  UINT32 flash_image;
        stffs_ram_image_bc_gal,// NOT USED: UINT8 *  ram_image;
        STFFS_BC_GAL_FILE_SIZE, //  UINT32 size;
        1,                 //  UINT8 use_alternate_size;
        XF_BC_REQ,       //  UINT32 requester;
        STFFS_BC_GAL_MIN_TIMER,// enum st_min_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,    //  enum stffs_access_e  access;
        0,                  // NOT USED:  UINT8 confirm_checksum_for_writes;

        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);

        stffs_bc_gal_read_init_callback,   // UINT8 (*read_init_callback)(struct stffs_file_s *fp);
        stffs_bc_gal_write_init_callback,  // UINT8 (*write_init_callback)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete_callback)(struct stffs_file_s *fp);
        stffs_bc_gal_write_ram_complete_callback, // UINT8 (*write_ram_complete_callback)(struct stffs_file_s *fp);
        stffs_bc_gal_execute,// UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE, //enum stffs_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        STFFS_BC_GAL_FILE_SIZE,    // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum;
    },
    {
        STFFS_HIS_CSV_NAME, // char  *  name;
        0,                  //  UINT32 flash_image;
        stffs_ram_image_his_csv,    // UINT8 *  ram_image;
        STFFS_HIS_CSV_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_HIS_CSV_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        NULL,               //stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        stffs_hiscsv_write_init_callback,// UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_IDLE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
    {
        STFFS_SUM_CSV_NAME, // char  *  name;
        0,                  //  UINT32 flash_image;
        stffs_ram_image_sum_csv,    // UINT8 *  ram_image;
        STFFS_SUM_CSV_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_SUM_CSV_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        NULL,               //stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        NULL,               //stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        stffs_sumcsv_write_init_callback,// UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_IDLE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
 {
        STFFS_LPS_INDEX, // char  *  name;
        START_ADDR_INDEX,                  //  UINT32 flash_image;
        stffs_ram_image_index,    // UINT8 *  ram_image;
        STFFS_LPS_INDEX_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_INDEX,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_INDEX_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
				NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_CSS, // char  *  name;
        START_ADDR_CSS,                  //  UINT32 flash_image;
        stffs_ram_image_css,    // UINT8 *  ram_image;
        STFFS_LPS_CSS_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_CSS,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_CSS_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_JS, // char  *  name;
        START_ADDR_JS,                  //  UINT32 flash_image;
        stffs_ram_image_js,    // UINT8 *  ram_image;
        STFFS_LPS_JS_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_JS,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_JS_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_ALA, // char  *  name;
        START_ADDR_ALA,                  //  UINT32 flash_image;
        stffs_ram_image_ala,    // UINT8 *  ram_image;
        STFFS_LPS_ALA_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_ALARMPAGE,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_ALA_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_DASH, // char  *  name;
        START_ADDR_DASH,                  //  UINT32 flash_image;
        stffs_ram_image_dash,    // UINT8 *  ram_image;
        STFFS_LPS_DASH_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_DASHBOARD,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_DASH_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_BAT, // char  *  name;
        START_ADDR_BAT,                  //  UINT32 flash_image;
        stffs_ram_image_bat,    // UINT8 *  ram_image;
        STFFS_LPS_BAT_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_BAT,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_BAT_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_HOME, // char  *  name;
        START_ADDR_HOME,                  //  UINT32 flash_image;
        stffs_ram_image_home,    // UINT8 *  ram_image;
        STFFS_LPS_HOME_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_HOME,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_HOME_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_DCDC, // char  *  name;
        START_ADDR_DCDC,                  //  UINT32 flash_image;
        stffs_ram_image_dcdc,    // UINT8 *  ram_image;
        STFFS_LPS_DCDC_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_DCDC,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_DCDC_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    },
		{
        STFFS_LPS_DOWNLOAD, // char  *  name;
        START_ADDR_DOWN,                  //  UINT32 flash_image;
        stffs_ram_image_download,    // UINT8 *  ram_image;
        STFFS_LPS_DOWNLOAD_SIZE,         //  UINT32 size;
        0,//1,                  //  UINT8 use_alternate_size;
        XF_WEB_DOWN,//0,                  //  UINT32 requester; FL_STFFS_REQ
        STFFS_LPS_DOWNLOAD_MIN_TIMER,// enum st_dh_timer_e open_min_timer;
        STFFS_FILE_OPEN_MIN_TIMEOUT,  //  UINT8 open_min_timeout;
        STFFS_ACCESS_READ_WRITE,//STFFS_ACCESS_READ_ONLY,    //  enum stffs_access_e  access;
        0,                  // const UINT8 confirm_checksum_for_writes:1;
        
        stffs_xf_read_req_adapter,  //  void (*fl_read_req)(struct stffs_file_s *fp);
        stffs_xf_write_req_adapter, //  void (*fl_write_req)(struct stffs_file_s *fp);
        stffs_xf_erase_req_adapter, //  void (*fl_erase_req)(struct stffs_file_s *fp);
        stffs_xf_req_is_done_adapter, // UINT8 (*fl_req_is_done)(struct stffs_file_s *fp);
        

        NULL,               // UINT8 (*read_init)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_init)(struct stffs_file_s *fp);

        NULL,               // UINT8 (*read_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*write_ram_complete)(struct stffs_file_s *fp);
        NULL,               // UINT8 (*execute)(struct stffs_file_s *fp);

        STFFS_STATE_INIT_RAM_IMAGE,   // enum stffs_file_state_e state;
        STFFS_MODE_READ_BINARY,    // enum stffs_mode_e mode;
        0,                  // UINT32 alternate_size;
        0,                  // UINT32 read_write_index;
        0,                  // UINT8 executing;
        1,                  // UINT8 mounted;                // lets user know if a file is ready to be read.
        0,                  // UINT8 open;
        0,                  // calculated_checksum
    }
};

// Functions definitions


/*****************************************************************************
*****************************************************************************/
UINT8 stffs_init_file(stffs_file_t *fp)
{
    UINT32 index = stffs_get_file_index_by_name(fp->name);

    if (index<STFFS_NUM_FILES)
    {
        stffs_files[index] = stffs_files_defaults[index];
        st_arm_min_timer(stffs_files[index].open_min_timer,0);  //reset timers to start with!
        return index;
    }
    return STFFS_NUM_FILES;
}

/*****************************************************************************
read_init_callbacks: return 1 on sucess!
*****************************************************************************/
static UINT8 stffs_dir_csv_read_init_callback(stffs_file_t *fp)
{
    UINT32 i;
    enum stffs_dir_csv_col_e j;
    stffs_file_t *file;
    UINT8 *data=fp->ram_image;
    UINT32 index=0;
    
    // header
    for (j=STFFS_DIR_CSV_COL_FIRST; j<STFFS_NUM_DIR_CSV_COLS; j++)
    {
        memcpy(&data[index],stffs_dir_csv_columns[j].str,stffs_dir_csv_columns[j].len);

        STFFS_MOVE_INDEX(data,index,stffs_dir_csv_columns[j].len);
        STFFS_ADD_COMMA_AND_MOVE_INDEX(data,index);
    }
    

    // files
    for (i=0; i<STFFS_NUM_FILES; i++)
    {
        file=&stffs_files[i];
        memcpy(&data[index],STFFS_NEWLINE_STR,STFFS_NEWLINE_STR_LEN);
        STFFS_MOVE_INDEX(data,index,STFFS_NEWLINE_STR_LEN);
        
        // file information in columns as per stffs_dir_csv_col_e
        for (j=STFFS_DIR_CSV_COL_FIRST; j<STFFS_NUM_DIR_CSV_COLS; j++)
        {
            switch(j)
            {
                case STFFS_DIR_CSV_COL_ACCESS:
                    memcpy(&data[index], stffs_file_access[file->access].str,stffs_file_access[file->access].len);
                    STFFS_MOVE_INDEX(data, index, stffs_file_access[file->access].len);
                    break;

                case STFFS_DIR_CSV_COL_FILENAME:
                    strcpy((char*)&data[index], file->name);
                    STFFS_MOVE_INDEX(data, index, strlen((const char*)&data[index]));
                    break;

                case STFFS_DIR_CSV_COL_FILESIZE:
                    if (fp == file)
                    {
                        // if the file is dir_csv itself,
                        memcpy(&file->ram_image[index],STFFS_UNKNOWN_STR,STFFS_UNKNOWN_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_UNKNOWN_STR_LEN);
                    }
                    else
                    {
                        st_ulltox((char*)&data[index], file->use_alternate_size?file->alternate_size:file->size,0);
                        STFFS_MOVE_INDEX(data, index, strlen((const char*)&data[index]));
                    }
                        
                    break;

                case STFFS_DIR_CSV_COL_CHECKSUM:
                    if (file->confirm_checksum_for_writes)
                    {
                        memcpy(&data[index],STFFS_CHECKSUM_NEEDED_STR,STFFS_CHECKSUM_NEEDED_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_CHECKSUM_NEEDED_STR_LEN);
                    }
                    else
                    {
                        memcpy(&data[index],STFFS_CHECKSUM_NOT_NEEDED_STR,STFFS_CHECKSUM_NOT_NEEDED_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_CHECKSUM_NOT_NEEDED_STR_LEN);
                    }
                    break;
                
                case STFFS_DIR_CSV_COL_MOUNTED:
                    if (file->mounted)
                    {
                        memcpy(&data[index],STFFS_MOUNTED_STR,STFFS_MOUNTED_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_MOUNTED_STR_LEN);
                    }
                    else
                    {
                        memcpy(&data[index],STFFS_BUSY_STR,STFFS_BUSY_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_BUSY_STR_LEN);
                    }
                    break;

                case STFFS_DIR_CSV_COL_OPEN:
                    if (file->open)
                    {
                        memcpy(&data[index],STFFS_OPEN_STR,STFFS_OPEN_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_OPEN_STR_LEN);
                    }
                    else
                    {
                        memcpy(&data[index],STFFS_CLOSED_STR,STFFS_CLOSED_STR_LEN);
                        STFFS_MOVE_INDEX(data, index, STFFS_CLOSED_STR_LEN);
                    }
                    break;

                // Unimplemented cases!
                case STFFS_DIR_CSV_COL_INFO:
                case STFFS_NUM_DIR_CSV_COLS:
                    break;
            }
            STFFS_ADD_COMMA_AND_MOVE_INDEX(data,index);
        }
    }

    fp->alternate_size = index; // no need to add one here since STFFS_ADD_COMMA_AND_MOVE_INDEX
                                //  has already moved index with data pointing to null with revised index!
    return 1;
}

/*****************************************************************************
write_init_callback
*****************************************************************************/
static UINT8 stffs_bc_gal_write_init_callback(stffs_file_t *fp)
{
  UINT8 *f = fp->ram_image;
  UINT32 len=0;
  
    #warning This is compute intensive section. May need to implement in a different way in future to support larger files.
    #warning Watchdog is kicked here.
    for (len=0, f = fp->ram_image; len < STFFS_BC_GAL_FILE_SIZE; len++, f++)
    {
        st_kick_watchdog();
        *f = STFFS_GAL_NO_DATA;
    }
    fp->alternate_size = STFFS_BC_GAL_FILE_SIZE;
  
}
static UINT8 stffs_hiscsv_write_init_callback(stffs_file_t *fp)
{
  UINT8 *f = fp->ram_image;
  UINT32 len=0;
  
    #warning This is compute intensive section. May need to implement in a different way in future to support larger files.
    #warning Watchdog is kicked here.
    for (len=0, f = fp->ram_image; len < STFFS_HIS_CSV_SIZE; len++, f++)
    {
        st_kick_watchdog();
        *f = 0x20;
    }
  
}
static UINT8 stffs_sumcsv_write_init_callback(stffs_file_t *fp)
{
  UINT8 *f = fp->ram_image;
  UINT32 len=0;
  
    #warning This is compute intensive section. May need to implement in a different way in future to support larger files.
    #warning Watchdog is kicked here.
    for (len=0, f = fp->ram_image; len < STFFS_SUM_CSV_SIZE; len++, f++)
    {
        st_kick_watchdog();
        *f = 0x20;
    }
  
}

/*****************************************************************************
read_init_callback
- Last 8 bytes of file has delimiter "\n\r$$$#\n\r"
*****************************************************************************/
static UINT8 stffs_bc_gal_read_init_callback(stffs_file_t *fp)
{
  static UINT8 first_time=1;
  UINT8 c, *f = fp->ram_image;
  UINT32 len=0;
  
  if (first_time) // alternate size is altered only during any config file write or on first time read only.
  {
    first_time = 0;
    #warning This is compute intensive section. May need to implement in a different way in future to support larger files.
    #warning Watchdog is kicked here.
    for (len=0, f = fp->ram_image;
            (len < (STFFS_BC_GAL_FILE_SIZE-STFFS_GAL_DELIMITER_STR_LEN-1)) &&   // size check, len < (delimiter + NULL)
            (*f != STFFS_GAL_NO_DATA) &&                                        // check for data availability
            (
                (f[STFFS_GAL_DELIMITER_OFFSET] == STFFS_GAL_DELIMITER) ?        // check if delimiter string check need to be performed.
                                                                                // this will optimize the need to use strncmp.
                                                                                // The check is performed only if delimiter charecter
                                                                                // is found.
                strncmp((char*)f, STFFS_GAL_DELIMITER_STR, STFFS_GAL_DELIMITER_STR_LEN) != 0 : 1
                                                                                // this might be a redundant compare if read_write_index
                                                                                // is not made to store it by design.
            );
            len++, f++
      )
        st_kick_watchdog();
  
    fp->alternate_size = len;
  }
}
/*****************************************************************************
write_completion_callback
*****************************************************************************/
static UINT8 stffs_defaults_execute(stffs_file_t *fp)
{
    fl_defaults_init();
    return 1;
}
/*****************************************************************************
stffs_exec_execute
*****************************************************************************/
static UINT8 stffs_exec_execute(stffs_file_t *fp)
{
    char *upgrade_info="UPGRADING VIA ETH...";
    if (strncmp((const char*)fp->ram_image,STFFS_UPGRADE_STR,STFFS_UPGRADE_STR_LEN) == 0)
    {
        #warning Use this as adhoc. A real ethernet upgrade can be implemented in a different function.
        #warning CAUTION - the following call may never return!
        return pt_ota_upgrade(upgrade_info);
    }    
    return 0;
}
/*****************************************************************************
stffs_lcfd
*****************************************************************************/
static UINT8 stffs_lcfd_execute(stffs_file_t *fp)
{
    if (strncmp((const char*)fp->ram_image,STFFS_LCFD_STR,STFFS_LCFD_STR_LEN) == 0)
    {
        #warning Use this as adhoc. A real ethernet upgrade can be implemented in a different function.
        #warning CAUTION - the following call may never return!
        pt_puts("Factory Defaults Done");
        return pt_load_config_factory_defaults("1");
    }
    return 0;
}

/*****************************************************************************
ram_complete_callback
- File size is added with internal use of 8 bytes for delimiter
- These 8 bytes are for file delimiting, not for external use
*****************************************************************************/
static UINT8 stffs_bc_gal_write_ram_complete_callback(stffs_file_t *fp)
{
    if(fp->read_write_index > (STFFS_BC_GAL_FILE_SIZE - STFFS_GAL_DELIMITER_STR_LEN-1))   // index > (delimiter + NULL)
      fp->read_write_index = STFFS_BC_GAL_FILE_SIZE - STFFS_GAL_DELIMITER_STR_LEN-1;
      
    strcpy((char*)&fp->ram_image[fp->read_write_index], STFFS_GAL_DELIMITER_STR); // this is only kept in RAM for script execution
                                                                                  // and not stored in Flash

    fp->alternate_size = fp->read_write_index;

    return 1;
}

/*****************************************************************************
- Allow to execute bc_gal configuration at the time of file copy or init
*****************************************************************************/
UINT8 stffs_bc_gal_execute(stffs_file_t *fp)
{
    static UINT8 power_on = 1;
    if(power_on == 1)
    {
      power_on = 0;
      if(cnfg_reset_gal)
      {
        fp->executing = 1;
        bc_gal_cmd_size = 0;
        cnfg_reset_gal = 0;
      }
      return 1;
    }
    fp->executing = 1;
    bc_gal_cmd_size = 0;
}
/*****************************************************************************
read_req_adapters
*****************************************************************************/
static void stffs_fl_read_req_adapter(stffs_file_t *fp)
{
    fl_issue_read_req((fl_requester_t)fp->requester, (UINT8 *)fp->flash_image, fp->ram_image, fp->size);
}
static void stffs_xf_read_req_adapter(stffs_file_t *fp)
{
    xf_issue_read_req((xf_requester_t)fp->requester, fp->flash_image, (char *)fp->ram_image, fp->size);
}

/*****************************************************************************
write_req_adapters
*****************************************************************************/
static void stffs_fl_write_req_adapter(stffs_file_t *fp)
{
    fl_issue_write_req((fl_requester_t)fp->requester, (UINT8 *)fp->flash_image, fp->ram_image, fp->size);
}
static void stffs_xf_write_req_adapter(stffs_file_t *fp)
{
    xf_issue_write_req((xf_requester_t)fp->requester, fp->flash_image, (char *)fp->ram_image, fp->size);
}

/*****************************************************************************
erase_req_adapters
*****************************************************************************/
static void stffs_fl_erase_req_adapter(stffs_file_t *fp)
{
    fl_issue_erase_req((fl_requester_t)fp->requester, (UINT8 *)fp->flash_image, fp->size);
}
static void stffs_xf_erase_req_adapter(stffs_file_t *fp)
{
    xf_issue_erase_req((xf_requester_t)fp->requester, fp->flash_image, fp->size);
}
/*****************************************************************************
req_is_done_adapters
*****************************************************************************/
static UINT8 stffs_fl_req_is_done_adapter(stffs_file_t *fp)
{
    return fl_request_is_done((fl_requester_t)fp->requester);
}
static UINT8 stffs_xf_req_is_done_adapter(stffs_file_t *fp)
{
    return xf_request_is_done((xf_requester_t)fp->requester);
}




/*****************************************************************************
*****************************************************************************/
void stffs_bc_gal_handler(void)
{
  stffs_file_t *fp = &stffs_files[STFFS_BC_GAL_FILE];
  if(fp->executing == 1)
    pt_proc_cmd_stream_portion(fp);

}
/****************************************************************************
Instruction:
- Comment starts with #
- Both lower and upper case allowed
- Blank line or line with only space/tabs are allowed
- commad need not start with CHA/cha 
- Last line of the file must be '$$$' (not for user - internal)
- File size must be within 48kbytes
- Use comments as less as possible to save file size
****************************************************************************/
static UINT8 pt_proc_cmd_stream_portion(stffs_file_t *fp)
{
  static UINT8 cmd_len; 
  UINT8 line_len, ret, i, j;
  UINT8 *pl, *pc;
  char cmd[PT_CMD_LEN + 1];
  char cmdc[PT_CMD_LEN + 1];
  UINT16 local_len = 0;
  const UINT8 *bc_gal_stream;

  bc_gal_stream = fp->ram_image + bc_gal_cmd_size;
  
  while (*bc_gal_stream != STFFS_GAL_NO_DATA)//ÿ
  {
    if(local_len == BC_GAL_CMD_EXE_NO)
    {
      local_len = 0;
      //report success alarm
      return(PT_OK);
    }
    else
    {
      pl = memchr(bc_gal_stream, '\n',PT_CMD_LEN); //first occurance of char in string

      if (pl == NULL)
        break;    // no more commands;
      else
      {
        pc = memchr(bc_gal_stream, '#',PT_CMD_LEN);

        if(pc == NULL)//#not found
          pc = pl-1;//point to '\r'
        else if(pc > pl)//'#' pointer is beyond '\n' pointer
            pc = pl-1;
        
        line_len = (UINT8)((pl - bc_gal_stream) + 1);
        cmd_len = (UINT8)((pc - bc_gal_stream));
        
        //Confirm execution length within boundary
        if(bc_gal_cmd_size + line_len <= fp->alternate_size)
          bc_gal_cmd_size += line_len;
        else
        {
          fp->executing = 0; //stop
          local_len = BC_GAL_CMD_EXE_NO;
          break;
        }
          
        if ((cmd_len <= PT_CMD_LEN) && (cmd_len != 0))
        {
          strncpy(cmd, (const char *)bc_gal_stream, cmd_len);
          cmd[cmd_len] = '\0';
          
          for(i=0,j=0; j < cmd_len; j++)
          {
            if(isspace(cmd[j]) || (strncmp(&cmd[j], "\t", 1) == 0))
              ;
            else if(isalpha(cmd[j]))
            {
              cmdc[i++] = (char)toupper(cmd[j]);
            }
            else if (strncmp(&cmd[j], "$$$", 3) == 0)
            {
              //report length error - @local_len
              i=0;
              fp->executing = 0; //stop
              local_len = BC_GAL_CMD_EXE_NO;
              break;
            }
            else if(isdigit(cmd[j]) ||
                    ispunct(cmd[j]) ||
                   (strncmp(&cmd[j], ",",  1) == 0) || 
                   (strncmp(&cmd[j], "=",  1) == 0) || 
                   (strncmp(&cmd[j], "\"", 1) == 0) || 
                   (strncmp(&cmd[j], "/" , 1) == 0) || 
                   (strncmp(&cmd[j], "-" , 1) == 0) || 
                   (strncmp(&cmd[j], "." , 1) == 0) || 
                   (strncmp(&cmd[j], ":" , 1) == 0) || 
                   (strncmp(&cmd[j], "\0", 1) == 0))
              cmdc[i++] = cmd[j];
            else
            {
              i=0;
              fp->executing = 0; //stop
              break;
            }
          }

          if(i)//command is available
          {
            cmdc[i] = '\0';
            i=0;
            if (strncmp(&cmdc[0], "CHA", 3) == 0)//scale out 'cha '
              i+=3;
            ret = pt_cha_internal(&cmdc[i]);
            local_len++;
          }
        }
        bc_gal_stream += line_len;
      }
    }
  }
  bc_gal_cmd_size = 0;
  fp->executing = 0; //stop
  //report success alarm
  return(PT_OK);
}


/*****************************************************************************
*****************************************************************************/
static UINT8 stffs_verify_gal_input(const void *ptr, SINT32 num_bytes)
{
  const char* ptr_data;
  SINT32 i;
  
  ptr_data = (const char*)ptr;
  for(i=0;i<num_bytes;i++)
  {
    if(stffs_verify_gal_char(ptr_data))
      ptr_data++;
    else
    {
      //reply alarm - config.gal error char found @ptr_data
      return FALSE;
    }
  }
  return TRUE;
}

/*****************************************************************************
*****************************************************************************/
static UINT8 stffs_verify_gal_char(const char* ptr_data)
{
  if(isalnum(*ptr_data) || ispunct(*ptr_data) || isspace(*ptr_data))
    return TRUE;
  if((*ptr_data == '\r') || (*ptr_data == '.') || (*ptr_data == '=') || 
     (*ptr_data == '-')  || (*ptr_data == '\"') ||
     (*ptr_data == ';')  || (*ptr_data == ',') || (*ptr_data == '/'))
    return TRUE;
  
  return FALSE;
  
}