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

#ifndef LIBMBSEGA_CORE
#define LIBMBSEGA_CORE

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include "utils.h"
#include "sms/mmu.h"
#include "sms/cpu.h"
#include "sms/vdp.h"
#include "sms/psg.h"
#include "sms/ioport.h"
using namespace sega;
using namespace std;

namespace sega
{
    class LIBMBSEGA_API MasterSystemCore
    {
	public:
	    MasterSystemCore();
	    ~MasterSystemCore();

	    unique_ptr<SMSMMU> coremmu;
	    unique_ptr<SMSCPU> corecpu;
	    unique_ptr<SMSVDP> corevdp;
	    unique_ptr<SMSPSG> corepsg;
	    unique_ptr<SMSPort> coreport;

	    mbsegaFrontend *front = NULL;

	    bool initcore();
	    bool loadROM(string filename);
	    segaRGB getpixel(int x, int y);
	    vector<segaRGB> getframebuffer();

	    void setfrontend(mbsegaFrontend *cb);

	    void keypressed(SMSButton button, bool is_player_1 = true);
	    void keyreleased(SMSButton button, bool is_player_1 = true);

	    void init();
	    void shutdown();

	    void runcore();
	    void runapp();
    };
};

#endif // LIBMBSEGA_CORE