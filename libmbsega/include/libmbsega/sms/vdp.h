/*
    This file is part of libmbsega.
    Copyright (C) 2021 BueniaDev.

    libmbsega is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmbsega is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libmbsega.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBMBSEGA_SMS_VDP
#define LIBMBSEGA_SMS_VDP

#include "utils.h"
#include "mmu.h"
using namespace sega;
using namespace std;

namespace sega
{
    class LIBMBSEGA_API SMSVDP
    {
	public:
	    SMSVDP(SMSMMU &mem);
	    ~SMSVDP();

	    void init();
	    void shutdown();

	    void write_control(uint8_t data);
	    void write_data(uint8_t data);
	    uint8_t read_status();
	    uint8_t read_data();
	    uint8_t read_vcounter();
	    uint8_t read_hcounter();
	    void clock_vdp(int cycles);
	    vector<segaRGB> getframebuffer();
	    bool is_irq_gen();

	private:
	    SMSMMU &memory;

	    bool is_sms1_vdp = true;

	    bool is_second_byte = false;
	    uint16_t command_word = 0;
	    int code_register = 0;
	    uint16_t addr_register = 0;

	    void increment_addr();
	    void render_scanline();

	    void render_graphics_one();
	    void render_mode4();

	    uint16_t getnamebase();

	    segaRGB getoldstylecolor(int pen_select);

	    void set_pixel(int x, int y, segaRGB color);

	    void write_reg(int reg, uint8_t data);

	    bool limitvscroll = false;
	    bool limithscroll = false;
	    bool maskfirstcol = false;
	    int mode_val = 0;
	    uint8_t scrolly_copy = 0;

	    uint8_t xscroll = 0;
	    uint8_t yscroll = 0;
	    uint8_t line_int = 0;

	    int line_interrupt = 0;

	    int pagename_addr = 0;
	    int color_addr = 0;
	    int pattern_addr = 0;

	    uint16_t hcounter = 0;
	    uint8_t vcounter = 0;

	    uint8_t get_vjump();
	    uint8_t get_vjump_to();
	    int get_height();

	    int display_res = 0;

	    uint8_t read_buffer = 0;

	    bool irq_gen = false;

	    bool vcounter_first = false;
	    bool is_vblank = false;

	    int backdrop_color = 0;

	    uint8_t status_byte = 0x00;

	    bool is_frame_irq = false;
	    bool is_line_irq = false;
	    bool is_vdp_enabled = false;

	    array<uint8_t, 0x4000> vram;
	    array<uint8_t, 32> cram;
	    array<segaRGB, (256 * 240)> framebuffer;
    };
};


#endif // LIBMBSEGA_SMS_VDP