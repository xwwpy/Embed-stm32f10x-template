#ifndef __OLED_H__
#define __OLED_H__
#include "stm32f10x.h"
#include "math_utils.h"
// 注意在GDDRAM中的每一页的每一个字节是从低位到高位排列


#define OLED_ADDRESS 0x78 // OLED 设备地址
// OLED 控制命令
#define OLED_DATA_CONTINUE_MODE 0x40 // Co = 0, D/C# = 1 连续数据模式
#define OLED_COMMAND_CONTINUE_MODE 0x00 // Co = 0, D/C# = 0 连续命令模式
#define OLED_DATA_MODE 0xc0 // Co = 1, D/C# = 1 单一数据模式
#define OLED_COMMAND_MODE 0x80 // Co = 1, D/C# = 0 单一命令模式


// OLED 寻址方式
// 设置寻址方式命令
#define OLED_SET_ADDRESSING_MODE 0x20 // 设置对应的寻址方式之前必须先发送此命令
#define OLED_HORIZONTAL_ADDRESSING_MODE 0x00 // 水平寻址模式
#define OLED_VERTICAL_ADDRESSING_MODE 0x01 // 垂直寻址模式
#define OLED_PAGE_ADDRESSING_MODE 0x02 // 页寻址模式

void oled_set_addressing_mode(u8 mode);


// 设置列起始和结束位置命令 仅当处于水平或垂直寻址模式下有效(并非)
#define OLED_SET_COLUMN_ADDRESS 0x21 // 前置命令
// 0-127 列地址 所以仅数据的后七位有效
void oled_set_column_address(u8 start, u8 end);
void reset_column_address(void); // 恢复列地址 每次更换寻址模式之后都要调用此函数恢复列地址


// 设置页起始和结束位置命令 仅当处于水平或垂直寻址模式下有效
#define OLED_SET_PAGE_ADDRESS 0x22 // 前置命令
// 0-7 页地址 所以仅数据的后三位有效
void oled_set_page_address(u8 start, u8 end);
void reset_page_address(void); // 恢复页地址 每次更换寻址模式之后都要调用此函数恢复页地址


// 设置列起始位置命令 仅当处于页寻址模式下有效
void oled_set_column_start_address_lower_nibble(u8 col); // 设置列起始地址低四位 0000_x3_x2_x1_x0
void oled_set_column_start_address_higher_nibble(u8 col); // 设置列起始地址高三位 0001_0_x2_x1_x0

void oled_set_page_address_start_column_and_page(u8 page, u8 col); // 设置页地址和起始列地址 页寻址模式下有效
void oled_set_page_address_start_column(u8 col); // 仅当处于页寻址模式下有效 设置起始列地址
// 设置起始页地址命令 仅当处于页寻址模式下有效
void oled_set_start_page_address(u8 page); // 设置起始页地址 0b1011_0x2x1x0 页地址 0-7

// 设置列的重映射（这里指的是GDDRAM的bit与屏幕像素的对应关系）默认情况下column address 0 映射到 SEG0（此时是相反的） 如果设置了重映射 则 column address 0 映射到 SEG127（此时就是对应的）
// remap为0时不重映射 其余重映射
// 默认映射命令
#define OLED_SET_COLUMN_REMAP_DEFAULT 0xA0
// 重映射命令
#define OLED_SET_COLUMN_REMAP_REMAP 0xA1
void oled_set_column_remap(u8 remap);


// 设置行的扫描方向 默认情况下 从COM0扫描到COM63（颠倒） 更改了扫描方向后 从COM63扫描到COM0
// remap为0时不更改默认的扫描方向 其余更改为反向扫描
// 默认扫描方向命令
#define OLED_SET_ROW_SCAN_DEFAULT_DIRECTION 0xC0 // Com0 - Com63
// 反向扫描命令
#define OLED_SET_ROW_REVERSE_SCAN_DIRECTION 0xC8 // Com63 - Com0
void oled_set_row_scan_direction(u8 remap);

// 设置OLED显示的对应关系
// 默认的显示，即 0 in GDDRAM 对应于 off on OLED 1 in GDDRAM 对应于 on on OLED
#define OLED_SET_DISPLAY_MODE_DEFAULT 0xA6
// 反相显示，即 0 in GDDRAM 对应于 on on OLED 1 in GDDRAM 对应于 off on OLED
#define OLED_SET_DISPLAY_MODE_REVERSE 0xA7
void oled_set_display_mod(u8 mode); // mode 为0时为默认显示 其余为反相显示

#define OLED_DISPLAY_ON 0xAF // 打开显示
#define OLED_DISPLAY_OFF 0xAE // 关闭显示
void oled_display_on(void);
void oled_display_off(void);


// 启用充电泵
#define OLED_CHARGE_PUMP_ENABLE 0x8D, 0x14
void oled_enable_charge_pump(void); // 启用充电泵 顺序参考文档


#define OLED_DEFAULT_ROW_SIZE 16 // 默认的字符高度 必须是8的倍数
#define OLED_DEFAULT_COLUMN_SIZE 8 // 默认的字符宽度
#define OLED_MAX_PAGE 8 // OLED 最大页数
#define OLED_MAX_COLUMN 128 // OLED 最大列数

/**
 * @brief 写入多个数据
 * 
 * @param bytes 数据
 * @param length 数据的长度
 */
void oled_write_data(u8* bytes, u8 length);
/**
 * @brief 写入多个命令
 * 
 * @param command 命令
 * @param length 命令的长度
 */

void oled_write_command(u8* command, u8 length);

/**
 * @brief OLED 初始化
 * 
 */
void oled_init(void);
void clear_oled(void);
void fill_oled(u8 val);
void fill_oled_with_char(u8 chr, u8 r_len, u8 c_len, u8 start_p_x, u8 start_p_y, u8 mode);
void oled_default_show_char(u8 chr, u8 p_x, u8 p_y);
void oled_write_datas(u8* bytes, u8 r_len, u8 c_len, u8 start_p_x, u8 start_p_y);
u8* get_char_data_pointer(u8 chr);
void oled_default_show_string_with_cut(u8 x, u8 y, u8* str, u8 len);
void oled_default_show_string_with_no_cut(u8 x, u8 y, u8* str, u8 len);
void oled_default_show_string_with_cut_better(u8 x, u8 y, u8* str, u8 len);
void oled_default_show_string_with_no_cut_better(u8 x, u8 y, u8* str, u8 len);
void oled_default_show_number_with_cut(u8 x, u8 y, uint32_t num, u8 len);
void oled_default_show_number_with_no_cut(u8 x, u8 y, uint32_t num, u8 len);
void oled_default_show_number_with_cut_better(u8 x, u8 y, uint32_t num, u8 len);
void oled_default_show_number_with_no_cut_better(u8 x, u8 y, uint32_t num, u8 len);
void oled_show_number(u8 x, u8 y, int32_t num, u8 len);
void oled_show_string(u8 x, u8 y, u8* str, u8 len);
void oled_show_float_number(u8 x, u8 y, float num, u8 len, u8 dot_len);
void oled_show_binary_number(u8 x, u8 y, uint32_t num, u8 len);
void oled_show_hex_number(u8 x, u8 y, uint32_t num, u8 len);
void get_next_row_and_col(u8* next_row, u8* next_col, u8 start_row, u8 start_col, u8 display_len, u8 max_row, u8 max_col);

#endif // !__OLED_H__
