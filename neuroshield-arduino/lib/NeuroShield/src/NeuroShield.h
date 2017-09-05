/*
 * NeuroShield.h - Driver for NeuroShield
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

#ifndef _NEUROSHIELD_H
#define _NEUROSHIELD_H

#include <NeuroShieldSPI.h>

extern "C" {
	#include <stdint.h>
}

#define NM_NCR			0x00
#define NM_COMP			0x01
#define NM_LCOMP		0x02
#define NM_DIST			0x03
#define NM_INDEXCOMP	0x03
#define NM_CAT			0x04
#define NM_AIF			0x05
#define NM_MINIF		0x06
#define NM_MAXIF		0x07
#define NM_TESTCOMP		0x08
#define NM_TESTCAT		0x09
#define NM_NID			0x0A
#define NM_GCR			0x0B
#define NM_RSTCHAIN		0x0C
#define NM_NSR			0x0D
#define NM_POWERSAVE	0x0E
#define NM_NCOUNT		0x0F
#define NM_FORGET		0x0F

#define NEURON_SIZE 	256		// memory capacity of each neuron in byte

#define ARDUINO_CON		5		// SPI_SEL
#define ARDUINO_SD_CS	6		// SDCARD_SSn
#define ARDUINO_SS		7		// NM500_SSn

class NeuroShield
{
	public:
	
		NeuroShield();
		uint16_t begin();
		uint16_t begin(uint8_t slave_select);
		
		void setNcr(uint16_t value);
		uint16_t getNcr();
		void setComp(uint8_t value);
		uint8_t getComp();
		void setLastComp(uint8_t value);
		void setIndexComp(uint16_t value);
		uint16_t getDist();
		void setCat(uint16_t value);
		uint16_t getCat();
		void setAif(uint16_t value);
		uint16_t getAif();
		void setMinif(uint16_t value);
		uint16_t getMinif();
		void setMaxif(uint16_t value);
		uint16_t getMaxif();
		uint16_t getNid();
		void setGcr(uint16_t value);
		uint16_t getGcr();
		void resetChain();
		void setNsr(uint16_t value);
		uint16_t getNsr();
		uint16_t getNcount();
		void setPowerSave();
		void forget();
		void forget(uint16_t maxif);
		
		void countTotalNeurons();
		void clearNeurons();
		
		void setContext(uint8_t context);
		void setContext(uint8_t context, uint16_t minif, uint16_t maxif);
		void getContext(uint8_t* context, uint16_t* minif, uint16_t* maxif);
		void setRbfClassifier();
		void setKnnClassifier();
		
		uint16_t broadcast(uint8_t vector[], uint16_t length);
		uint16_t learn(uint8_t vector[], uint16_t length, uint16_t category);
		uint16_t classify(uint8_t vector[], uint16_t length);
		uint16_t classify(uint8_t vector[], uint16_t length, uint16_t* distance, uint16_t* category, uint16_t* nid);
		uint16_t classify(uint8_t vector[], uint16_t length, uint16_t k, uint16_t distance[], uint16_t category[], uint16_t nid[]);
		
		void readNeuron(uint16_t nid, uint8_t model[], uint16_t* ncr, uint16_t* aif, uint16_t* cat);
		void readNeuron(uint16_t nid, uint8_t nuerons[]);
		uint16_t readNeurons(uint8_t neurons[]);
		uint16_t writeNeurons(uint8_t neurons[]);
		
		uint16_t testCommand(uint8_t read_write, uint8_t reg, uint16_t data);
		
		uint16_t fpgaVersion();
		void nm500Reset();
		void ledSelect(uint8_t data);
		
		uint16_t total_neurons;
};
#endif