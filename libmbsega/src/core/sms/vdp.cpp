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

#include <sms/vdp.h>
using namespace sega;
using namespace std;

namespace sega
{
    SMSVDP::SMSVDP()
    {

    }

    SMSVDP::~SMSVDP()
    {

    }

    void SMSVDP::init()
    {
	vram.fill(0);
	cram.fill(0);
	framebuffer.fill({0, 0, 0});

	// Values of VDP registers at power-up:
	// 0x00 = 0x36
	// 0x01 = 0x80
	// 0x02 = 0xFF
	// 0x03 = 0xFF
	// 0x04 = 0xFF
	// 0x05 = 0xFF
	// 0x06 = 0xFB
	// 0x0A = 0xFF
	// All other registers = 0x00

	array<uint8_t, 11> vdp_init_values = 
	{
	    0x36, 0x80, 0xFF, 0xFF,
	    0xFF, 0xFF, 0xFB, 0x00,
	    0x00, 0x00, 0xFF
	};

	for (int reg = 0; reg <= 0x0A; reg++)
	{
	    write_reg(reg, vdp_init_values[reg]);
	}

	cout << "SMS-VDP::Initialized" << endl;
    }

    void SMSVDP::shutdown()
    {
	cout << "SMS-VDP::Shutting down..." << endl;
    }

    void SMSVDP::increment_addr()
    {
	if (addr_register == 0x3FFF)
	{
	    addr_register = 0;
	}
	else
	{
	    addr_register += 1;
	}
    }

    void SMSVDP::write_reg(int reg, uint8_t data)
    {
	if (reg > 0xA)
	{
	    return;
	}

	switch (reg)
	{
	    case 0x0:
	    {
		limitvscroll = testbit(data, 7);
		limithscroll = testbit(data, 6);
		maskfirstcol = testbit(data, 5);
		is_line_irq = testbit(data, 4);
		mode_val |= (testbit(data, 2) << 3);
		mode_val |= (testbit(data, 1) << 1);
	    }
	    break;
	    case 0x1:
	    {
		mode_val |= (testbit(data, 3) << 2);
		mode_val |= testbit(data, 4);

		is_frame_irq = testbit(data, 5);
		is_vdp_enabled = testbit(data, 6);

		if (is_vblank && is_frame_irq)
		{
		    irq_gen = true;
		}
	    }
	    break;
	    case 0x02:
	    {
		pagename_addr = (data & 0xF);
	    }
	    break;
	    case 0x03:
	    {
		color_addr = data;
	    }
	    break;
	    case 0x04:
	    {
		pattern_addr = (data & 0x7);
	    }
	    break;
	    case 0x08:
	    {
		xscroll = data;
	    }
	    break;
	    case 0x09:
	    {
		scrolly_copy = data;
	    }
	    break;
	    case 0x0A:
	    {
		line_int = data;
	    }
	    break; 
	}
    }

    void SMSVDP::write_control(uint8_t data)
    {
	if (is_second_byte)
	{
	    command_word = ((command_word & 0xFF) | (data << 8));
	    code_register = (command_word >> 14);
	    addr_register = (command_word & 0x3FFF);

	    switch (code_register)
	    {
		case 0:
		{
		    cout << "Unimplemented: Code register mode 0" << endl;
		}
		break;
		case 2:
		{
		    int reg_num = ((command_word >> 8) & 0xF);
		    uint8_t reg_data = (command_word & 0xFF);

		    cout << "Writing value of " << hex << (int)reg_data << " to VDP register of " << dec << (int)reg_num << endl;
		    write_reg(reg_num, reg_data);
		}
		break;
	    }

	    is_second_byte = false;
	}
	else
	{
	    command_word = ((command_word & 0xFF00) | data);
	    addr_register = (command_word & 0x3FFF);
	    is_second_byte = true;
	}
    }

    void SMSVDP::write_data(uint8_t data)
    {
	switch (code_register)
	{
	    case 0:
	    case 1:
	    case 2: vram[addr_register] = data; break;
	    case 3: cram[(addr_register & 31)] = data; break;
	}

	increment_addr();
    }

    uint8_t SMSVDP::read_status()
    {
	uint8_t status = (is_vblank << 7);
	
	is_vblank = false;
	irq_gen = false;
	is_second_byte = false;
	return status;
    }

    uint8_t SMSVDP::read_vcounter()
    {
	return vcounter;
    }

    bool SMSVDP::is_irq_gen()
    {
	return irq_gen;
    }

    uint8_t SMSVDP::get_vjump()
    {
	// TODO: PAL timings
	uint8_t temp = 0;
	switch (display_res)
	{
	    case 0: temp = 0xDA; break;
	    case 1: temp = 0xEA; break;
	    default: temp = 0xFF; break;
	}

	return temp;
    }

    uint8_t SMSVDP::get_vjump_to()
    {
	// TODO: PAL timings
	uint8_t temp = 0;
	switch (display_res)
	{
	    case 0: temp = 0xD5; break;
	    case 1: temp = 0xE5; break;
	    default: temp = 0x00; break;
	}

	return temp;
    }

    int SMSVDP::get_height()
    {
	int temp = 0;
	switch (display_res)
	{
	    case 0: temp = 192; break;
	    case 1: temp = 224; break;
	    case 2: temp = 240; break;
	}

	return temp;
    }

    uint16_t SMSVDP::getnamebase()
    {
	int temp = pagename_addr;

	if (display_res == 0) // Using small resolution
	{
	    return ((temp & 0xE) << 10);
	}

	return (((temp & 0xC) << 10) | 0x700);
    }

    void SMSVDP::clock_vdp(int cycles)
    {
	int clock_info = (cycles * 2);
	int height = get_height();

	hcounter += clock_info;

	if (hcounter >= 685)
	{
	    hcounter -= 685;

	    uint8_t vcount = vcounter;
	    vcounter += 1;

	    if (vcount == 0xFF)
	    {
		vcounter = 0;
		render_scanline();
		vcounter_first = true;
	    }
	    else if ((vcount == get_vjump()) && vcounter_first)
	    {
		vcounter_first = false;
		vcounter = get_vjump_to();
	    }
	    else if (vcounter == height)
	    {
		is_vblank = true;
	    }

	    if (vcounter >= height)
	    {
		if (vcounter != height)
		{
		    line_interrupt = line_int;
		}

		yscroll = scrolly_copy;
		int mode = mode_val;

		if (mode == 11)
		{
		    display_res = 1;
		}
		else if (mode == 14)
		{
		    display_res = 2;
		}
		else
		{
		    display_res = 0;
		}
	    }

	    if (vcounter < height)
	    {
		render_scanline();
	    }

	    if (vcounter <= height)
	    {
		bool underflow = false;

		if (line_interrupt == 0)
		{
		    underflow = true;
		}

		line_interrupt -= 1;

		if (underflow)
		{
		    line_interrupt = line_int;

		    if (is_line_irq)
		    {
			irq_gen = true;
		    }
		}
	    }
	}

	if (is_vblank && is_frame_irq)
	{
	    irq_gen = true;
	}
    }

    void SMSVDP::render_scanline()
    {
	if (!is_vdp_enabled)
	{
	    return;
	}

	switch (mode_val)
	{
	    case 0: render_graphics_one(); break;
	    case 8:
	    case 10:
	    case 11:
	    case 12:
	    case 14:
	    case 15: render_mode4(); break;
	    default:
	    {
		cout << "Unrecognized graphics mode of " << dec << (int)mode_val << endl;
	    }
	    break;
	}
    }

    void SMSVDP::render_graphics_one()
    {
	return;
    }

    void SMSVDP::render_mode4()
    {
	return;
    }

    // Reference for VDP colors: (http://www.sega-16.com/forum/showthread.php?30530-SMS-VDP-output-levels)
    segaRGB SMSVDP::getoldstylecolor(int pen_select)
    {
	// Blue-channel is non-linear (verified from die shot)
	// Reference: (https://www.retrorgb.com/sega-master-system-blue-channel-mysteries-further-uncovered.html)
	array<uint8_t, 4> level = {0, 78, 160, 238};
	array<uint8_t, 4> blue_level = {0, 98, 160, 238};

	pen_select &= 0xF;

	switch (pen_select)
	{
	    case 0: return {level[0], level[0], blue_level[0]}; break;
	    case 1: return {level[0], level[0], blue_level[0]}; break;
	    case 2: return {level[0], level[2], blue_level[0]}; break;
	    case 3: return {level[0], level[3], blue_level[0]}; break;
	    case 4: return {level[0], level[0], blue_level[1]}; break;
	    case 5: return {level[0], level[0], blue_level[3]}; break;
	    case 6: return {level[1], level[0], blue_level[0]}; break;
	    case 7: return {level[0], level[3], blue_level[3]}; break;
	    case 8: return {level[2], level[0], blue_level[0]}; break;
	    case 9: return {level[3], level[0], blue_level[0]}; break;
	    case 10: return {level[1], level[1], blue_level[0]}; break;
	    case 11: return {level[3], level[3], blue_level[0]}; break;
	    case 12: return {level[0], level[1], blue_level[0]}; break;
	    case 13: return {level[3], level[0], blue_level[3]}; break;
	    case 14: return {level[1], level[1], blue_level[1]}; break;
	    case 15: return {level[3], level[3], blue_level[3]}; break;
	    default: return {0, 0, 0}; break; // This shouldn't happen
	}
    }

    void SMSVDP::set_pixel(int x, int y, segaRGB color)
    {
	int ydiff = ((240 - get_height()) / 2);

	int xpos = x;
	int ypos = (y + ydiff);

	int screen_index = (xpos + (ypos * 256));
	framebuffer[screen_index] = color;
    }

    vector<segaRGB> SMSVDP::getframebuffer()
    {
	vector<segaRGB> buffer = vector<segaRGB>(framebuffer.begin(), framebuffer.end());
	return buffer;
    }
};