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

#include <sms/mmu.h>
using namespace sega;
using namespace std;

namespace sega
{
    SMSMMU::SMSMMU()
    {
	sysram.fill(0);
    }

    SMSMMU::~SMSMMU()
    {

    }

    void SMSMMU::init()
    {
	first_bank_page = 0;
	second_bank_page = 1;
	third_bank_page = 2;
	cout << "SMSMMU::Initialized" << endl;
    }

    void SMSMMU::shutdown()
    {
	gamerom.clear();
	cartmem.clear();
	cout << "SMSMMU::Shutting down..." << endl;
    }

    uint8_t SMSMMU::readByte(uint16_t addr)
    {
	uint8_t temp = 0;

	// Read from mirror 0xDFFC-0xDFFF, not the memory map registers
	if (addr >= 0xFFFC)
	{
	    addr -= 0x2000;
	}

	if (!is_codemasters && (addr < 0x400))
	{
	    temp = gamerom[addr];
	}
	else if (addr < 0x4000)
	{
	    uint32_t bank_addr = (addr + (0x4000 * first_bank_page));
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0x8000)
	{
	    uint32_t bank_addr = (addr + (0x4000 * second_bank_page));
	    bank_addr -= 0x4000;
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0xC000)
	{
	    cout << "Reading byte from address of " << hex << (int)addr << endl;
	    exit(0);
	}
	else
	{
	    temp = sysram[(addr & 0x1FFF)];
	}

	return temp;
    }

    void SMSMMU::writeByte(uint16_t addr, uint8_t data)
    {
	// ROM cannot be written to
	if (addr < 0x8000)
	{
	    return;
	}
	else if (addr < 0xC000)
	{
	    cout << "Writing to ROM/RAM bank" << endl;
	    exit(0);
	}
	else
	{
	    if (addr >= 0xFFFC)
	    {
		cout << "Writing to mapper register of " << hex << (int)addr << endl;
		exit(0);
	    }

	    sysram[(addr & 0x1FFF)] = data;
	}
    }

    bool SMSMMU::loadROM(vector<uint8_t> data)
    {
	if (data.empty())
	{
	    cout << "Error: could not load SMS ROM" << endl;
	    return false;
	}

	// Strip off any 512-byte header found at
	// the beginning of the ROM
	uint64_t offset = ((data.size() % 0x4000) == 512) ? 512 : 0;

	cartmem.resize(0x100000, 0);
	copy((data.begin() + offset), data.end(), cartmem.begin());

	// Determine if the ROM is 1 MB in size 
	// (for proper memory paging)
	is_one_meg = (data.size() > 0x80000);

	detect_codemasters_cart();

	gamerom = vector<uint8_t>(cartmem.begin(), (cartmem.begin() + 0xC000));
	return true;
    }

    // Detect if the currently loaded ROM is a CodeMasters game
    void SMSMMU::detect_codemasters_cart()
    {
	// Calculate checksum
	uint16_t checksum = ((cartmem[0x7FE7] << 8) | cartmem[0x7FE6]);

	// If the checksum is zero, then the ROM is a normal Sega game
	if (checksum == 0)
	{
	    return;
	}

	// Calculate the inverse checksum
	uint16_t compute = (0x10000 - checksum);

	uint16_t answer = ((cartmem[0x7FE9] << 8) | cartmem[0x7FE8]);

	// If the checksum and inverse checksum are equal,
	// the ROM is a CodeMasters game
	if (compute == answer)
	{
	    cout << "CodeMasters game detected" << endl;
	    is_codemasters = true;
	}
    }
}