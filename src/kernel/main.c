#include "define/integer.h"
#include "define/s_rom.h"
#include "lib/rom/rom.h"
#include "define/color.h"
#include "lib/graphics/graphics.h"
#include "lib/str.h"

asm(".code16gcc\n"
    "call main\n");

u_char8 print_char(u_char8 symbol, u_char8 color, u_char8 x, u_char8 y, u_short16 width)
{
    asm volatile(
        "mov %%cl, %%ah\n"
        "push $0xb800\n"
        "pop %%es\n"
        "stosw\n"
        : "=a"(symbol)
        : "a"(symbol), "c"(color),"D"((y * width + x) * 2)
        :
    );
    return symbol;
}

void print_logo(s_video_mode_info info, u_char8 line)
{
    print_char('Z', c_white, 3, line, info.width);
    print_char('O', c_blue, 4, line, info.width);
    print_char('S', c_red, 5, line, info.width);
}
void print_ok(s_video_mode_info info, u_char8 line)
{
    print_char('O', c_green, 2, line, info.width);
    print_char('K', c_green, 3, line, info.width);
}
void print_error(s_video_mode_info info, u_char8 line)
{
    print_char('E', c_red, 2, line, info.width);
    print_char('R', c_red, 3, line, info.width);
    print_char('R', c_red, 4, line, info.width);
    print_char('O', c_red, 5, line, info.width);
    print_char('R', c_red, 6, line, info.width);
}
void status(bool status, s_video_mode_info info, u_char8 line)
{
    if (status)
    {
        print_ok(info, line);
    } else {
        print_error(info, line);
    }; 
}

void main()
{
    // ###### init ######
    Rom rom = Rom();
    set_video_mode(0x3);
    s_video_mode_info info = get_video_mode_info();
    status(info.mode == 0x3, info, 1);
    status(rom.get_boot_disk_id() == 0x80, info, 2);
    print_logo(info, 4);

    // ###### main ######

    s_f_string8 disk = str_conv_dec(rom.get_boot_disk_id());
    // print_char(rom.get_boot_disk_id(), c_green, 2, 4, info.width);
    for (u_char8 i; i < 10; i++){
        print_char(disk.data[i], c_white, i + 8, 2, info.width);
    };
    s_daps test;
    test.p_size = 16;
    test.p_number_sectors = 8;
    test.p_offset = 0x1500;
    test.p_segment = 0;
    test.p_start_sector = 9;


    u_short16 err;
    // u_int32 address = (u_int32)((u_char8 *)&test);
    // s_address addr = rom.get_daps_address(&test);
    // u_int32 addr2 = rom.get_address_daps(addr);
    asm volatile(
        "mov $0x42, %%ah\n"
        // "push %%ds\n"
        "push %%cs\n"
        "pop %%ds\n"
        "int $0x13\n"
        // "pop %%ds\n"
        ""
        : "=a"(err)
        : "d"(rom.get_boot_disk_id()),"S"(&test)//,"c"(addr.p_address_high)
        :
    );
    print_char((err >> 8)+0x30, c_white, 8, 8, info.width);

    // if (address == addr2)
        // print_char(addr2 / 16 / 16, c_white, 8, 11, info.width);
        // print_char(address - 0x80, c_white, 8, 12, info.width);


    // rom.load_data_use_daps(&test);


    // u_char8 *test_call = (u_char8 *)0x7E00;

    asm(
        "push $0x0000\n\t"
        "pop %%ds\n\t"
        "call %%ax\n\t"
        // "jmp %%ax\n\t"
        :
        :"a"(test.p_offset)
        :
        );


    {
        for (u_char8 i; i < 16; i++){
        print_char(((u_char8 *)&test)[i]+0x30, c_white, 16 - i + 8, 10, info.width);
        };
        print_char(test.p_start_sector+0x30, c_white, 8, 11, info.width);

    }
    // test.p_number_sectors = 0x3132;
    // test.p_sector = 0x3132333435363738;
    // u_int32 address = (u_int32)((u_char8 *)&test);
    // s_f_string8 dph = str_conv_dec(rom.get_daps_address(&test).p_address_high);
    // s_f_string8 dpl = str_conv_dec(rom.get_daps_address(&test).p_address_low);
    // s_f_string8 sa = str_conv_dec(test.p_number_sectors);
    // print_char(rom.get_boot_disk_id(), c_green, 2, 4, info.width);
    // for (u_char8 i; i < 10; i++){
    //     print_char(dph.data[i], c_white, i + 8, 8, info.width);
    // };
    // for (u_char8 i; i < 10; i++){
    //     print_char(dpl.data[i], c_white, i + 8, 9, info.width);
    // };
    // for (u_char8 i; i < 10; i++){
    //     print_char(sa.data[i], c_white, i + 8, 10, info.width);
    // };
}
