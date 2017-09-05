/*
 * NeuroShieldSPI.cpp - Driver for NeuroShield
 * Copyright (c) 2016, General Vision Inc, All rights reserved
 * Copyright (c) 2017, nepes inc, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <NeuroShield.h>
#include <NeuroShieldSPI.h>
#include <SPI.h>

using namespace std;
extern "C" {
  #include <stdint.h>
}

// ----------------------------------------------------------------
//    Constructor to the class ShieldSPI
// ----------------------------------------------------------------
NeuroShieldSPI::NeuroShieldSPI(){	
}

// ----------------------------------------------------------------
// Initialize the SPI communication and verify proper interface
// to the NM500 by reading the default Minif value of 2-bytes
// return an error:0 otherwise=1
// ----------------------------------------------------------------
bool NeuroShieldSPI::connect(uint8_t slave_select)
{
	uint16_t read_value;
	
	shield_ss = slave_select;
	
	SPI.begin();
	SPI.setClockDivider(NM500_SPI_CLK_DIV);		// spi clock : 16MHz / 8 = 2MHz
	
	pinMode(shield_ss, OUTPUT);
	digitalWrite(shield_ss, HIGH);
	
	pinMode(ARDUINO_CON, OUTPUT);				// ARDUINO_CON must be LOW
	digitalWrite(ARDUINO_CON, LOW);
	
	pinMode(ARDUINO_SD_CS, OUTPUT);				// ARDUINO_SD_CS must be HIGH
	digitalWrite(ARDUINO_SD_CS, HIGH);
	
	// return 1 if NM500 present and SPI comm successful
	for (int i = 0; i < 10; i++) {
		reset();	// NM500 reset
		delay(100);
		write(NM_FORGET, 0);
		delay(50);
		read_value = read(NM_MINIF);
		if (read_value == 2)
			return(1);
		delay(50);
	}
	
	return(0);
}

// ----------------------------------------------------------------
// SPI Read the register of a given module (module + reg = addr)
// ----------------------------------------------------------------
uint16_t NeuroShieldSPI::read(uint8_t reg)
{
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);							// Dummy for ID
	SPI.transfer((uint8_t)module_nm500);		// address (4-byte)
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(reg);
	SPI.transfer(0);							// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer(1);							// expect 1 word back
	uint16_t data = SPI.transfer(0);			// Send 0 to push upper data out
	data = (data << 8) + SPI.transfer(0);		// Send 0 to push lower data out
	digitalWrite(shield_ss, HIGH);
	return(data);
}

// ----------------------------------------------------------------
// SPI Write the register of a given module (module + reg = addr)
// ----------------------------------------------------------------
void NeuroShieldSPI::write(uint8_t reg, uint16_t data)
{
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);									// Dummy for ID
	SPI.transfer((uint8_t)(module_nm500 + 0x80));		// module and write flag
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(reg);
	SPI.transfer(0);									// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer(1);									// expect 1 word back
	if ((reg == NM_COMP) || (reg == NM_LCOMP)) {
		SPI.transfer(0x00);								// upper data
		SPI.transfer((uint8_t)(data & 0x00FF));			// lower data
	}
	else {
		SPI.transfer((uint8_t)((data >> 8) & 0x00FF));	// upper data
		SPI.transfer((uint8_t)(data & 0x00FF));			// lower data
	}
	digitalWrite(shield_ss, HIGH);
}

// ----------------------------------------------------------------
// SPI Write burst mode at COMP register
// ----------------------------------------------------------------
uint16_t NeuroShieldSPI::writeVector(uint8_t reg, uint8_t* data, uint16_t size)
{
	if ((reg != NM_COMP) || (size > 255))
		return(0);
	
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);								// Dummy for ID
	SPI.transfer((uint8_t)(module_nm500 + 0x80));	// module and write flag
	SPI.transfer(0);								
	SPI.transfer(0);
	SPI.transfer(reg);
	SPI.transfer(0);								// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer((uint8_t)size);					// 0 ~ 255 byte
	for (int i = 0; i < size; i++) {
		SPI.transfer(0x00);							// COMP' upper data = 0x00
		SPI.transfer((uint8_t)(*data));				// lower data
		data++;
	}
	digitalWrite(shield_ss, HIGH);
	return(size);
}

// ----------------------------------------------------------------
// read FPGA Version
// ----------------------------------------------------------------
uint16_t NeuroShieldSPI::version()
{
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);						// Dummy for ID
	SPI.transfer((uint8_t)module_fpga);				// address (4-byte)
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(1);						// version check : 0x01
	SPI.transfer(0);						// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer(1);						// expect 1 word back
	uint16_t data = SPI.transfer(0);		// Send 0 to push upper data out
	data = (data << 8) + SPI.transfer(0);	// Send 0 to push lower data out
	digitalWrite(shield_ss, HIGH);
	return(data);
}

// ----------------------------------------------------------------
// excute NM500 SW reset
// ----------------------------------------------------------------
void NeuroShieldSPI::reset()
{
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);									// Dummy for ID
	SPI.transfer((uint8_t)(module_fpga + 0x80));	// address (4-byte)
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(2);									// nm500 sw reset : 0x02
	SPI.transfer(0);									// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer(1);									// expect 1 word back
	SPI.transfer(0);
	SPI.transfer(0);
	digitalWrite(shield_ss, HIGH);
}

// ----------------------------------------------------------------
// LED scenario select
// ----------------------------------------------------------------
void NeuroShieldSPI::ledSelect(uint8_t data)
{
	SPI.setClockDivider(NM500_SPI_CLK_DIV);
	digitalWrite(shield_ss, LOW);
	SPI.transfer(1);									// Dummy for ID
	SPI.transfer((uint8_t)(module_led + 0x80));	// address (4-byte)
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(data);									// led scenario select
	SPI.transfer(0);									// word size (3-byte)
	SPI.transfer(0);
	SPI.transfer(1);									// expect 1 word back
	SPI.transfer(0);
	SPI.transfer(0);
	digitalWrite(shield_ss, HIGH);
}