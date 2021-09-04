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

#ifndef LIBMBSEGA_SMS_PSG
#define LIBMBSEGA_SMS_PSG

#include <fstream>
#include "utils.h"
#include "sn76489.h"
using namespace sega;
using namespace std;

namespace sega
{
    class LIBMBSEGA_API SMSPSG
    {
	public:
	    SMSPSG();
	    ~SMSPSG();

	    void init();
	    void shutdown();
	    void writereg(uint8_t data);
	    void clock_psg(int cycles);

	    void setaudiocallback(audiofunc cb);

	private:
	    SN76489 psg;
	    audiofunc outputaudio;
	    const uint32_t psg_clock = 3579545;

	    float out_step = 0.0f;
	    float in_step = 0.0f;
	    float out_time = 0.0f;

	    float sample_ratio = 0.0f;
	    float sample_phase = 0.0f;

	    uint32_t psg_cycles = 0;

	    void mixaudio();
    };
};

#endif // LIBMBSEGA_SMS_PSG