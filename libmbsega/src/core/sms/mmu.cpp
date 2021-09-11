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

	switch (mapper)
	{
	    case SMSMapper::Sega: third_bank_page = 2; break;
	    case SMSMapper::CodeMasters: third_bank_page = 0; break;
	}

	current_ram_bank = 0;
	is_ram_enabled = false;
	sysram.fill(0);
	bankedram.fill(0);
	cout << "SMSMMU::Initialized" << endl;
    }

    void SMSMMU::shutdown()
    {
	gamerom.clear();
	cartmem.clear();
	cout << "SMSMMU::Shutting down..." << endl;
    }

    bool SMSMMU::isMapperCodeMasters()
    {
	return (mapper == SMSMapper::CodeMasters);
    }

    bool SMSMMU::isRegionUSA()
    {
	return (region == SMSRegion::USA);
    }

    bool SMSMMU::isRegionJapan()
    {
	return (region == SMSRegion::Japan);
    }

    bool SMSMMU::isRegionEurope()
    {
	return (region == SMSRegion::Europe);
    }

    bool SMSMMU::isRegionNTSC()
    {
	return (isRegionUSA() || isRegionJapan());
    }

    bool SMSMMU::isRegionPAL()
    {
	return isRegionEurope();
    }

    uint8_t SMSMMU::readSega(uint16_t addr)
    {
	uint8_t temp = 0;

	if (addr < 0x400)
	{
	    temp = gamerom[addr];
	}
	else if (addr < 0x4000)
	{
	    uint32_t bank_addr = (addr + (first_bank_page * 0x4000));
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0x8000)
	{
	    // NOTE: This is where the BIOS checks for a ROM
	    uint32_t bank_addr = ((addr - 0x4000) + (second_bank_page * 0x4000));
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0xC000)
	{
	    if (is_ram_enabled)
	    {
		uint32_t bank_addr = ((addr - 0x8000) + (current_ram_bank * 0x4000));
		temp = bankedram[bank_addr];
	    }
	    else
	    {
		uint32_t bank_addr = ((addr - 0x8000) + (third_bank_page * 0x4000));
		temp = cartmem[bank_addr];
	    }
	}
	else
	{
	    temp = sysram[(addr & 0x1FFF)];
	}

	return temp;
    }

    void SMSMMU::writeSega(uint16_t addr, uint8_t data)
    {
	if (addr < 0x8000)
	{
	    return;
	}
	else if (addr < 0xC000)
	{
	    if (is_ram_enabled)
	    {
		uint32_t bank_addr = ((addr - 0x8000) + (current_ram_bank * 0x4000));
		bankedram[bank_addr] = data;
	    }
	}
	else
	{
	    if (addr >= 0xFFFC)
	    {
		domempage((addr & 0x3), data);
	    }

	    sysram[(addr & 0x1FFF)] = data;
	}
    }

    uint8_t SMSMMU::readCodeMasters(uint16_t addr)
    {
	uint8_t temp = 0;
	if (addr < 0x4000)
	{
	    uint32_t bank_addr = (addr + (first_bank_page * 0x4000));
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0x8000)
	{
	    uint32_t bank_addr = ((addr - 0x4000) + (second_bank_page * 0x4000));
	    temp = cartmem[bank_addr];
	}
	else if (addr < 0xC000)
	{
	    uint32_t bank_addr = ((addr - 0x8000) + (third_bank_page * 0x4000));
	    temp = cartmem[bank_addr];
	}
	else
	{
	    temp = sysram[(addr & 0x1FFF)];
	}

	return temp;
    }

    void SMSMMU::writeCodeMasters(uint16_t addr, uint8_t data)
    {
	if (addr < 0xC000)
	{
	    switch (addr)
	    {
		case 0x0000:
		case 0x4000:
		case 0x8000: domempagecm(addr, data); break;
		default: return; break;
	    }
	}
	else
	{
	    sysram[(addr & 0x1FFF)] = data;
	}
    }

    void SMSMMU::domempagecm(uint16_t addr, uint8_t data)
    {
	uint8_t page = (data & 0x1F);
	switch (addr)
	{
	    case 0x0000: first_bank_page = page; break;
	    case 0x4000:
	    {
		if (testbit(data, 7))
		{
		    cout << "RAM enabled" << endl;
		}

		second_bank_page = page;
	    }
	    break;
	    case 0x8000: third_bank_page = page; break;
	    default: break;
	}
    }

    void SMSMMU::domempage(int addr, uint8_t data)
    {
	uint8_t page = (data & 0x3F);
	switch (addr)
	{
	    case 0:
	    {
		is_ram_enabled = testbit(data, 3);

		if (is_ram_enabled)
		{
		    current_ram_bank = testbit(data, 2) ? 1 : 0;
		}
	    }
	    break;
	    case 1: first_bank_page = page; break;
	    case 2: second_bank_page = page; break;
	    case 3: third_bank_page = page; break;
	}
    }

    uint8_t SMSMMU::readByte(uint16_t addr)
    {
	uint8_t temp = 0;

	switch (mapper)
	{
	    case SMSMapper::Sega: temp = readSega(addr); break;
	    case SMSMapper::CodeMasters: temp = readCodeMasters(addr); break;
	    default:
	    {
		cout << "Unrecognized mapper" << endl;
		exit(1);
	    }
	    break;
	}

	return temp;
    }

    void SMSMMU::writeByte(uint16_t addr, uint8_t data)
    {
	switch (mapper)
	{
	    case SMSMapper::Sega: writeSega(addr, data); break;
	    case SMSMapper::CodeMasters: writeCodeMasters(addr, data); break;
	    default:
	    {
		cout << "Unrecognized mapper" << endl;
		exit(1);
	    }
	    break;
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

	detect_codemasters_cart();

	gamerom = vector<uint8_t>(cartmem.begin(), (cartmem.begin() + 0xC000));
	return true;
    }

    // Detect if the currently loaded ROM is a CodeMasters game
    void SMSMMU::detect_codemasters_cart()
    {
	bool is_codemasters = false;
	// Calculate checksum
	uint16_t checksum = ((cartmem[0x7FE7] << 8) | cartmem[0x7FE6]);

	// If the checksum is zero, then the ROM is a normal Sega game
	if (checksum != 0)
	{
	    // Calculate the inverse checksum
	    uint16_t compute = (0x10000 - checksum);

	    uint16_t answer = ((cartmem[0x7FE9] << 8) | cartmem[0x7FE8]);

	    // If the checksum and inverse checksum are equal,
	    // the ROM is a CodeMasters game
	    if (compute == answer)
	    {
		is_codemasters = true;
	    }
	}

	if (is_codemasters)
	{
	    cout << "CodeMasters game detected" << endl;
	    mapper = SMSMapper::CodeMasters;
	    region = SMSRegion::Europe;
	}
	else
	{
	    if (is_auto_detection)
	    {
		int region_code = (cartmem[0x7FFF] >> 4);

		if (region_code >= 5)
		{
		    cout << "Game Gear game detected, some things may not work properly!" << endl;
		}
		else
		{
		    cout << "Master System game detected" << endl;

		    if (region_code == 3)
		    {
			cout << "Japan region detected" << endl;
			region = SMSRegion::Japan;
		    }
		    else
		    {
			cout << "Export region detected" << endl;
			region = SMSRegion::USA;
		    }
		}
	    }

	    mapper = SMSMapper::Sega;
	}
    }
}