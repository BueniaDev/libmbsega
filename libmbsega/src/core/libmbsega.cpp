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

#include "libmbsega.h"
#include <cmath>
using namespace sega;
using namespace std;

namespace sega
{
    MasterSystemCore::MasterSystemCore()
    {
	coremmu = make_unique<SMSMMU>();
	corevdp = make_unique<SMSVDP>();
	corepsg = make_unique<SMSPSG>();
	corecpu = make_unique<SMSCPU>(*coremmu, *corevdp, *corepsg);
    }

    MasterSystemCore::~MasterSystemCore()
    {

    }

    void MasterSystemCore::init()
    {
	coremmu->init();
	corevdp->init();
	corepsg->init();
	corecpu->init();
	cout << "mbsega-SMS::Initialized" << endl;
    }

    void MasterSystemCore::shutdown()
    {
	if (front != NULL)
	{
	    front->shutdown();
	}

	corecpu->shutdown();
	corepsg->shutdown();
	corevdp->shutdown();
	coremmu->shutdown();

	cout << "mbsega-SMS::Shutting down..." << endl;
    }

    void MasterSystemCore::setfrontend(mbsegaFrontend *cb)
    {
	front = cb;
	corepsg->setaudiocallback(bind(&mbsegaFrontend::audiocallback, front, _1, _2));
	cout << "Setting frontend..." << endl;
    }

    bool MasterSystemCore::loadROM(string filename)
    {
	cout << "mbsega-SMS::Loading ROM " << filename << "..." << endl;

	if (front != NULL)
	{
	    return coremmu->loadROM(front->loadfile(filename));
	}

	return false;
    }

    segaRGB MasterSystemCore::getpixel(int x, int y)
    {
	// Sanity check to avoid crash caused by out-of-bounds vector accesses
	if (((x < 0) || (x >= 256)) || ((y < 0) || (y >= 240)))
	{
	    return {0, 0, 0};
	}

	return corevdp->getframebuffer().at(x + (y * 256));
    }

    vector<segaRGB> MasterSystemCore::getframebuffer()
    {
	return corevdp->getframebuffer();
    }

    bool MasterSystemCore::initcore()
    {
	init();

	if (front != NULL)
	{
	    front->init();
	}

	return true;
    }

    void MasterSystemCore::runcore()
    {
	uint32_t total_machine_clocks = constants::machinecycles::SMS_NTSC;
	uint32_t clock_cycles = 0;
	
	while (clock_cycles < total_machine_clocks)
	{
	    int z80_cycles = corecpu->runinstruction();

	    // Handle interrupts
	    corecpu->handleinterrupts();

	    int machine_clocks = (z80_cycles * 3);
	    int vdp_clocks = (machine_clocks / 2);

	    int psg_clocks = z80_cycles;

	    corevdp->clock_vdp(vdp_clocks);
	    corepsg->clock_psg(psg_clocks);
	    clock_cycles += machine_clocks;
	}
    }

    void MasterSystemCore::runapp()
    {
	if (front != NULL)
	{
	    front->runapp();
	}
    }
};