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

    inline bool testbit(uint32_t reg, int bit)
    {
	return ((reg >> bit) & 1) ? true : false;
    }

    struct segaRGB
    {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
    };

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
	    static constexpr uint32_t SMS_NTSC = ((53693175 / 5) / 60);
	    static constexpr uint32_t SMS_PAL = ((53203424 / 5) / 50);
	};
    };
};

#endif // LIBMBSEGA_UTILS