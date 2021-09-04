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

#ifndef LIBMBSEGA_SMS_CPU
#define LIBMBSEGA_SMS_CPU

#include "utils.h"
#include "vdp.h"
#include "psg.h"
#include "mmu.h"
#include "zilogz80.h"
using namespace sega;
using namespace std;

namespace sega
{
    class LIBMBSEGA_API SMSInterface : public BeeZ80Interface
    {
	public:
	    SMSInterface(SMSMMU &mem, SMSVDP &vdp, SMSPSG &psg);
	    ~SMSInterface();

	    uint8_t readByte(uint16_t addr);
	    void writeByte(uint16_t addr, uint8_t val);
	    uint8_t portIn(uint16_t port);
	    void portOut(uint16_t port, uint8_t val);

	private:
	    SMSMMU &memory;
	    SMSVDP &graphics;
	    SMSPSG &audiopsg;
    };

    class LIBMBSEGA_API SMSCPU
    {
	public:
	    SMSCPU(SMSMMU &mem, SMSVDP &vdp, SMSPSG &psg);
	    ~SMSCPU();

	    void init();
	    void shutdown();

	    int runinstruction();
	    void handleinterrupts();

	private:
	    BeeZ80 z80core;
	    SMSMMU &memory;
	    SMSVDP &graphics;
	    SMSPSG &audiopsg;
	    SMSInterface *inter = NULL;
    };
};

#endif // LIBMBSEGA_SMS_CPU