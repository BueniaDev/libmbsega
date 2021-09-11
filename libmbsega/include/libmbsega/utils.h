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

#ifndef LIBMBSEGA_UTILS
#define LIBMBSEGA_UTILS

#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <functional>
#include "libmbsega_api.h"
using namespace std;
using namespace std::placeholders;

namespace sega
{
    using audiofunc = function<void(int16_t, int16_t)>;

    template<typename T>
    inline bool testbit(T reg, int bit)
    {
	return ((reg >> bit) & 1) ? true : false;
    }

    template<typename T>
    inline T setbit(T reg, int bit)
    {
	return (reg | (1 << bit));
    }

    template<typename T>
    inline T resetbit(T reg, int bit)
    {
	return (reg & ~(1 << bit));
    }

    struct segaRGB
    {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
    };

    inline void generate_crc_table(array<uint32_t, 256> &table)
    {
	uint32_t polynomial = 0xEDB88320;

	for (uint32_t i = 0; i < 256; i++)
	{
	    uint32_t value = i;

	    for (size_t j = 0; j < 8; j++)
	    {
		if (testbit(value, 0))
		{
		    value = (polynomial ^ (value >> 1));
		}
		else
		{
		    value >>= 1;
		}
	    }

	    table[i] = value;
	}
    }

    inline uint32_t calculate_crc32(uint32_t initial, vector<uint8_t> data)
    {
	array<uint32_t, 256> crc_table;
	generate_crc_table(crc_table);

	uint32_t crc = (initial ^ 0xFFFFFFFF);

	for (size_t i = 0; i < data.size(); i++)
	{
	    crc = (crc_table[((crc ^ data[i]) & 0xFF)] ^ (crc >> 8));
	}

	return (crc ^ 0xFFFFFFFF);
    }

    class LIBMBSEGA_API mbsegaFrontend
    {
	public:
	    mbsegaFrontend()
	    {

	    }

	    ~mbsegaFrontend()
	    {

	    }

	    virtual bool init() = 0;
	    virtual void shutdown() = 0;
	    virtual void runapp() = 0;
	    virtual vector<uint8_t> loadfile(string filename) = 0;
	    virtual void audiocallback(int16_t left, int16_t right) = 0;
    };

    namespace constants
    {
	namespace machinecycles
	{
	    static constexpr uint32_t SMS_NTSC = (53693175 / 5);
	    static constexpr uint32_t SMS_PAL = (53203424 / 5);
	};

	namespace fps
	{
	    static constexpr uint32_t NTSC = 60;
	    static constexpr uint32_t PAL = 50;
	};
    };

    inline uint32_t get_sms_clock_rate(bool is_pal)
    {
	uint32_t clock_rate = 0;

	if (is_pal)
	{
	    clock_rate = constants::machinecycles::SMS_PAL;
	}
	else
	{
	    clock_rate = constants::machinecycles::SMS_NTSC;
	}

	return (clock_rate / 3);
    }

    inline uint32_t get_machine_clocks(bool is_pal)
    {
	if (is_pal)
	{
	    return (constants::machinecycles::SMS_PAL / constants::fps::PAL);
	}
	else
	{
	    return (constants::machinecycles::SMS_NTSC / constants::fps::NTSC);
	}
    }
};

#endif // LIBMBSEGA_UTILS