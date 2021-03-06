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

#ifndef LIBMBSEGA_SMS_MMU
#define LIBMBSEGA_SMS_MMU

#include "utils.h"

namespace sega
{
    enum SMSMapper : int
    {
	Sega = 0,
	CodeMasters = 1,
    };

    enum SMSRegion : int
    {
	USA,
	Japan,
	Europe
    };

    class LIBMBSEGA_API SMSMMU
    {
	public:
	    SMSMMU();
	    ~SMSMMU();

	    void init();
	    void shutdown();

	    vector<uint8_t> gamerom;

	    bool loadROM(vector<uint8_t> data);

	    uint8_t readByte(uint16_t addr);
	    void writeByte(uint16_t addr, uint8_t data);

	    bool isMapperCodeMasters();

	    bool isRegionJapan();
	    bool isRegionUSA();
	    bool isRegionEurope();

	    bool isRegionNTSC();
	    bool isRegionPAL();

	private:
	    vector<uint8_t> cartmem;
	    array<uint8_t, 0x2000> sysram;
	    array<uint8_t, 0x8000> bankedram;

	    bool is_auto_detection = true;

	    uint8_t readSega(uint16_t addr);
	    void writeSega(uint16_t addr, uint8_t data);

	    uint8_t readCodeMasters(uint16_t addr);
	    void writeCodeMasters(uint16_t addr, uint8_t data);

	    void detect_codemasters_cart();
	    void domempage(int addr, uint8_t data);
	    void domempagecm(uint16_t addr, uint8_t data);

	    int first_bank_page = 0;
	    int second_bank_page = 0;
	    int third_bank_page = 0;

	    bool is_ram_enabled = false;
	    int current_ram_bank = 0;

	    SMSMapper mapper;
	    SMSRegion region;
    };
};

#endif // LIBMBSEGA_SMS_MMU