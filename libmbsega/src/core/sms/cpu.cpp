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

#include <sms/cpu.h>
using namespace sega;
using namespace std;

namespace sega
{
    SMSInterface::SMSInterface(SMSMMU &mem, SMSVDP &vdp, SMSPSG &psg) : memory(mem), graphics(vdp), audiopsg(psg)
    {

    }

    SMSInterface::~SMSInterface()
    {

    }

    uint8_t SMSInterface::readByte(uint16_t addr)
    {
	return memory.readByte(addr);
    }

    void SMSInterface::writeByte(uint16_t addr, uint8_t val)
    {
	memory.writeByte(addr, val);
    }

    uint8_t SMSInterface::portIn(uint16_t port)
    {
	// TODO: Implement remaining I/O register reads
	uint8_t temp = 0;

	if ((port & 0xC1) == 0x40)
	{
	    temp = graphics.read_vcounter();
	}
	else if ((port & 0xC1) == 0x41)
	{
	    cout << "Reading VDP hcounter" << endl;
	    exit(0);
	    temp = 0;
	}
	else if ((port & 0xC1) == 0x80)
	{
	    cout << "Reading VDP data port..." << endl;
	    exit(0);
	    temp = 0;
	}
	else if ((port & 0xC1) == 0x81)
	{
	    cout << "Reading VDP status..." << endl;
	    temp = graphics.read_status();
	}
	else if ((port & 0xC1) == 0xC0)
	{
	    // TODO: Implement I/O port reads
	    cout << "Reading I/O port A..." << endl;
	    temp = 0xFF;
	}
	else if ((port & 0xC1) == 0xC1)
	{
	    // TODO: Implement I/O port reads
	    cout << "Reading I/O port B..." << endl;
	    temp = 0xFF;
	}
	else
	{
	    cout << "Reading byte from port " << hex << (int)(port & 0xFF) << endl;
	    exit(0);
	    temp = 0;
	}

	return temp;
    }

    void SMSInterface::portOut(uint16_t port, uint8_t val)
    {
	// TODO: Implement remaining I/O register writes
	if ((port & 0xC0) == 0x40)
	{
	    // cout << "Writing byte of " << hex << (int)val << " to SN76489 PSG" << endl;
	    audiopsg.writereg(val);
	}
	else if ((port & 0xC1) == 0x80)
	{
	    graphics.write_data(val);
	}
	else if ((port & 0xC1) == 0x81)
	{
	    graphics.write_control(val);
	}
	else
	{
	    cout << "Writing byte of " << hex << (int)val << " to port " << hex << (int)(port & 0xFF) << endl;
	    return;
	}
    }

    SMSCPU::SMSCPU(SMSMMU &mem, SMSVDP &vdp, SMSPSG &psg) : memory(mem), graphics(vdp), audiopsg(psg)
    {
	
    }

    SMSCPU::~SMSCPU()
    {
	if (inter != NULL)
	{
	    inter = NULL;
	}
    }

    void SMSCPU::init()
    {
	inter = new SMSInterface(memory, graphics, audiopsg);
	z80core.setinterface(inter);
	z80core.init();
	cout << "SMS-CPU::Initialized" << endl;
    }

    void SMSCPU::shutdown()
    {
	z80core.shutdown();
	cout << "SMS-CPU::Shutting down..." << endl;
    }

    int SMSCPU::runinstruction()
    {
	return z80core.runinstruction();
    }

    void SMSCPU::handleinterrupts()
    {
	if (graphics.is_irq_gen())
	{
	    z80core.generate_interrupt(0xFF); // RST 38H
	}
    }
};