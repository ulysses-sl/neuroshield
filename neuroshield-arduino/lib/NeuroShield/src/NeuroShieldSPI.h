/*
 * NeuroShieldSPI.h - Driver for NeuroShield
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

/*
 * Revision History (v1.1.4)
 * 2018/06/22    v1.1.4    Minor changes
 * 2018/01/03    v1.1.3    Add burst-mode read
 * 2017/12/20    v1.1.2    Modify the structure of neurondata
 * 2017/12/11    v1.1.1    Add Powersave command and Minor changes to the library
 * 2017/08/17    v1.0.0    First Release
 */

#ifndef _NEUROSHIELDSPI_H
#define _NEUROSHIELDSPI_H

#define NM500_SPI_CLK_DIV	SPI_CLOCK_DIV8	// spi clock : 16MHz / 8 = 2MHz
#define NM500_SPI_CLK		2000000

extern "C" {
  #include <stdint.h>
}

class NeuroShieldSPI
{
	public:				
			
		NeuroShieldSPI();
		uint8_t shield_ss;
		bool connect(uint8_t slave_select);
		
		uint16_t read(uint8_t reg);
		void readVector16(uint16_t* data, uint16_t size);
		void write(uint8_t reg, uint16_t data);
		uint16_t writeVector(uint8_t* data, uint16_t size);
		uint16_t writeVector16(uint16_t* data, uint16_t size);
		
		uint16_t version();
		void reset();
		void ledSelect(uint8_t data);
		
		static const uint8_t module_nm500 = 0x01;		// addr[24:31] to access NM500 chip
		static const uint8_t module_fpga  = 0x02;
		static const uint8_t module_led   = 0x03;
};

#endif // _NEUROSHIELDSPI_H
