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

#include <sms/psg.h>
using namespace sega;
using namespace std;

namespace sega
{
    SMSPSG::SMSPSG()
    {

    }

    SMSPSG::~SMSPSG()
    {

    }

    void SMSPSG::init()
    {
	sample_ratio = ((psg.get_sample_rate(psg_clock)) / 48000.0f);
	cout << "SMSPSG::Initialized" << endl;
    }

    void SMSPSG::shutdown()
    {
	cout << "SMSPSG::Shutting down..." << endl;
    }

    void SMSPSG::setaudiocallback(audiofunc cb)
    {
	outputaudio = cb;
    }

    void SMSPSG::writereg(uint8_t data)
    {
	psg.writeIO(data);
    }

    void SMSPSG::clock_psg(int cycles)
    {
	for (int i = 0; i < cycles; i++)
	{
	    psg_cycles += 1;

	    if (psg_cycles == psg.get_divisor())
	    {
		mixaudio();
		psg_cycles = 0;
	    }
	}
    }

    void SMSPSG::mixaudio()
    {
	psg.chipclock();
	while (sample_phase < 1.0)
	{
	    auto samples = psg.get_sample();

	    if (outputaudio)
	    {
		outputaudio(samples[0], samples[1]);
	    }

	    sample_phase += sample_ratio;
	}

	sample_phase -= 1.0f;
    }
};