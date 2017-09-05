/*
 * NeuroShield.cpp - Driver for NeuroShield
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

using namespace std;
extern "C" {
	#include <stdint.h>
}

NeuroShieldSPI spi;

// ------------------------------------------------------------ //
//    Constructor to the class NeuroShield
// ------------------------------------------------------------
NeuroShield::NeuroShield() {
}

// ------------------------------------------------------------ 
// Initialize the neural network
// 0:fail,  other: success(return total_neurons)
// ------------------------------------------------------------
uint16_t NeuroShield::begin()
{
	bool read_value = spi.connect(ARDUINO_SS);	// default CS

	if (read_value != 1) {
		return(0);
	}
	else {
		countTotalNeurons();
		clearNeurons();
	}
	
	return(total_neurons);
}
uint16_t NeuroShield::begin(uint8_t slaveSelect)
{
	bool read_value = spi.connect(slaveSelect);
	
	if (read_value != 1) {
		return(0);
	}
	else {
		countTotalNeurons();
		clearNeurons();
	}
	
	return(total_neurons);
}

// --------------------------------------------------------
// Get/Set the Neuron Context Register
//---------------------------------------------------------
void NeuroShield::setNcr(uint16_t value)
{
	spi.write(NM_NCR, value);
}

uint16_t NeuroShield::getNcr()
{
	return(spi.read(NM_NCR));
}

// --------------------------------------------------------
// Get/Set the COMP register (component)
//---------------------------------------------------------
void NeuroShield::setComp(uint8_t value)
{
	spi.write(NM_COMP, (value & 0x00FF));
}

uint8_t NeuroShield::getComp()
{
	return((uint8_t)(spi.read(NM_COMP) & 0x00FF));
}

// --------------------------------------------------------
// Get/Set the LCOMP register (last component)
//---------------------------------------------------------
void NeuroShield::setLastComp(uint8_t value)
{
	spi.write(NM_LCOMP, (value & 0x00FF));
}

// --------------------------------------------------------
// Set the Component Index register
//---------------------------------------------------------
void NeuroShield::setIndexComp(uint16_t value)
{
	spi.write(NM_INDEXCOMP, value);
}

// --------------------------------------------------------
// Get the Distance register
//---------------------------------------------------------
uint16_t NeuroShield::getDist()
{
	return(spi.read(NM_DIST));
}

// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void NeuroShield::setCat(uint16_t value)
{
	spi.write(NM_CAT, value);
}

uint16_t NeuroShield::getCat()
{
	return(spi.read(NM_CAT));
}

// --------------------------------------------------------
// Get/Set the AIF register
//---------------------------------------------------------
void NeuroShield::setAif(uint16_t value)
{
	spi.write(NM_AIF, value);
}

uint16_t NeuroShield::getAif()
{
	return(spi.read(NM_AIF));
}

// --------------------------------------------------------
// Get/Set the Minimum Influence Field register
//---------------------------------------------------------
void NeuroShield::setMinif(uint16_t value)
{
	spi.write(NM_MINIF, value);
}

uint16_t NeuroShield::getMinif()
{
	return(spi.read(NM_MINIF));
}

// --------------------------------------------------------
// Get/Set the Maximum Influence Field register
//---------------------------------------------------------
void NeuroShield::setMaxif(uint16_t value)
{
	spi.write(NM_MAXIF, value);
}

uint16_t NeuroShield::getMaxif()
{
	return(spi.read(NM_MAXIF));
}

// --------------------------------------------------------
// Get the Neuron Identifier
//---------------------------------------------------------
uint16_t NeuroShield::getNid()
{
	return(spi.read(NM_NID));
}

// --------------------------------------------------------
// Get/Set the Global Context register
//---------------------------------------------------------
void NeuroShield::setGcr(uint16_t value)
{
	// GCR[15-8]= unused
	// GCR[7]= Norm (0 for L1; 1 for LSup)
	// GCR[6-0]= Active context value
	spi.write(NM_GCR, value);
}

uint16_t NeuroShield::getGcr()
{
	return(spi.read(NM_GCR));
}

// --------------------------------------------------------
// Reset the chain to first neuron in SR Mode
//---------------------------------------------------------
void NeuroShield::resetChain()
{
	spi.write(NM_RSTCHAIN, 0);
}

// --------------------------------------------------------
// Get/Set the Network Status register
// bit 2 = UNC (read only)
// bit 3 = ID (read only)
// bit 4 = SR mode
// bit 5 = KNN mode
//---------------------------------------------------------
void NeuroShield::setNsr(uint16_t value)
{
	spi.write(NM_NSR, value);
}

uint16_t NeuroShield::getNsr()
{
	return(spi.read(NM_NSR));
}

// --------------------------------------------------------
// Read the number of committed neurons
//---------------------------------------------------------
uint16_t NeuroShield::getNcount()
{
	return(spi.read(NM_NCOUNT));
}

// --------------------------------------------------------
// Set the PowerSave mode
//---------------------------------------------------------
void NeuroShield::setPowerSave()
{
	spi.write(NM_POWERSAVE, 1);
}

// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn
// Reset the Maximum Influence Field to default value=0x4000
// ------------------------------------------------------------ 
void NeuroShield::forget()
{
	spi.write(NM_FORGET, 0);
}

// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// ------------------------------------------------------------ 
void NeuroShield::forget(uint16_t maxif)
{
	spi.write(NM_FORGET, 0);
	spi.write(NM_MAXIF, maxif);
}

// ------------------------------------------------------------ 
// Count total neurons in SR-mode
// ------------------------------------------------------------ 
void NeuroShield::countTotalNeurons()
{
	uint16_t read_cat;
	
	spi.write(NM_FORGET, 0);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_TESTCAT, 0x0001);
	spi.write(NM_RSTCHAIN, 0);
	
	total_neurons = 0;
	while (1) {
		read_cat = spi.read(NM_CAT);
		if (read_cat == 0xFFFF)
			break;
		total_neurons++;
	}
	spi.write(NM_NSR, 0x0000);
	spi.write(NM_FORGET, 0);
}

// --------------------------------------------------------------
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// Clear the memory of the neurons
// --------------------------------------------------------------
void NeuroShield::clearNeurons()
{
	spi.write(NM_FORGET, 0);
	spi.write(NM_NSR, 0x0010);
	for (int i = 0; i < NEURON_SIZE; i++)
		spi.write(NM_TESTCOMP, 0);
	spi.write(NM_RSTCHAIN, 0);
	spi.write(NM_NSR, 0x0000);
}

// --------------------------------------------------------
// Broadcast a vector to the neurons and return the recognition status
// 0= unknown, 4=uncertain, 8=Identified
//---------------------------------------------------------
uint16_t NeuroShield::broadcast(uint8_t vector[], uint16_t length)
{
	spi.writeVector(NM_COMP, vector, (length - 1));
	spi.write(NM_LCOMP, vector[length - 1]);
	
	return(spi.read(NM_NSR));
}

//-----------------------------------------------
// Learn a vector using the current context value
//----------------------------------------------
uint16_t NeuroShield::learn(uint8_t vector[], uint16_t length, uint16_t category)
{
	broadcast(vector, length);
	spi.write(NM_CAT, category);
	return(spi.read(NM_NCOUNT));
}

// ---------------------------------------------------------
// Classify a vector and return its classification status
// NSR=0, unknown
// NSR=8, identified
// NSR=4, uncertain
// ---------------------------------------------------------
uint16_t NeuroShield::classify(uint8_t vector[], uint16_t length)
{
	broadcast(vector, length);
	return(spi.read(NM_NSR));
}

//----------------------------------------------
// Recognize a vector and return the best match, or the 
// category, distance and identifier of the top firing neuron
//----------------------------------------------
uint16_t NeuroShield::classify(uint8_t vector[], uint16_t length, uint16_t* distance, uint16_t* category, uint16_t* nid)
{
	broadcast(vector, length);
	*distance = spi.read(NM_DIST);
	*category = spi.read(NM_CAT);
	*nid = spi.read(NM_NID);
	return(spi.read(NM_NSR));
}

//----------------------------------------------
// Recognize a vector and return the response  of up to K top firing neurons
// The response includes the distance, category and identifier of the neuron
// The Degenerated flag of the category is masked rmask the degenerated response, use the current context value
// Return the number of firing neurons or K whichever is smaller
//----------------------------------------------
uint16_t NeuroShield::classify(uint8_t vector[], uint16_t length, uint16_t k, uint16_t distance[], uint16_t category[], uint16_t nid[])
{
	uint16_t recog_nbr = 0;
	
	broadcast(vector, length);
	for (int i = 0; i < k; i++)	{
		distance[i] = spi.read(NM_DIST);
		if (distance[i] == 0xFFFF) {
			distance[i] = 0xFFFF;
			category[i] = 0xFFFF;
			nid[i] = 0xFFFF;
		}
		else {
			recog_nbr++;
			category[i] = spi.read(NM_CAT);
			nid[i] = spi.read(NM_NID);
		}
	}
	return(recog_nbr);
}

// ------------------------------------------------------------ 
// Set a context and associated minimum and maximum influence fields
// ------------------------------------------------------------ 
void NeuroShield::setContext(uint8_t context)
{
	// context[15-8]= unused
	// context[7]= Norm (0 for L1; 1 for LSup)
	// context[6-0]= Active context value
	uint16_t read_val = spi.read(NM_GCR);
	read_val = (read_val & 0xFF80) | (context & 0x007F);
	spi.write(NM_GCR, read_val);
}

// ------------------------------------------------------------ 
// Set a context and associated minimum and maximum influence fields
// ------------------------------------------------------------ 
void NeuroShield::setContext(uint8_t context, uint16_t minif, uint16_t maxif)
{
	// context[15-8]= unused
	// context[7]= Norm (0 for L1; 1 for LSup)
	// context[6-0]= Active context value
	uint16_t read_val = spi.read(NM_GCR);
	read_val = (read_val & 0xFF80) | (context & 0x007F);
	spi.write(NM_GCR, read_val);
	spi.write(NM_MINIF, minif);
	spi.write(NM_MAXIF, maxif);
}

// ------------------------------------------------------------ 
// Get a context and associated minimum and maximum influence fields
// ------------------------------------------------------------ 
void NeuroShield::getContext(uint8_t* context, uint16_t* minif, uint16_t* maxif)
{
	// context[15-8]= unused
	// context[7]= Norm (0 for L1; 1 for LSup)
	// context[6-0]= Active context value
	*context = (uint8_t)(spi.read(NM_GCR) & 0x007F);
	*minif = spi.read(NM_MINIF);
	*maxif = spi.read(NM_MAXIF);
}

// --------------------------------------------------------
// Set the neurons in Radial Basis Function mode (default)
//---------------------------------------------------------
void NeuroShield::setRbfClassifier()
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, (temp_nsr & 0x00DF));
}

// --------------------------------------------------------
// Set the neurons in K-Nearest Neighbor mode
//---------------------------------------------------------
void NeuroShield::setKnnClassifier()
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, (temp_nsr | 0x0020));
}

//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting at index 0
//-------------------------------------------------------------
void NeuroShield::readNeuron(uint16_t nid, uint8_t model[], uint16_t* ncr, uint16_t* aif, uint16_t* cat)
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	if (nid > 0) {
		// move to index in the chain of neurons
		for (int i = 0; i < nid; i++)
			spi.read(NM_CAT);
	}
	
	*ncr = spi.read(NM_NCR);
	for (int i = 0; i < NEURON_SIZE; i++)
		model[i] = spi.read(NM_COMP);
	*aif = spi.read(NM_AIF);
	*cat = spi.read(NM_CAT);
	spi.write(NM_NSR, temp_nsr);		// set the NN back to its calling status
}

//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting index is 0
// Returns an array of NEURONSIZE bytes with the following format
// 2-bytes NCR, NEURONSIZE-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//-------------------------------------------------------------
void NeuroShield::readNeuron(uint16_t nid, uint8_t neuron[])
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	if (nid > 0) {
		// move to index in the chain of neurons
		for (int i = 0; i < nid; i++)
			spi.read(NM_CAT);
	}
	uint16_t read_val = spi.read(NM_NCR);
	neuron[0] = (uint8_t)((read_val >> 8) & 0x00FF);
	neuron[1] = (uint8_t)(read_val & 0x00FF);
	for (int i = 0; i < NEURON_SIZE; i++)
		neuron[i + 2] = spi.read(NM_COMP);
	read_val = spi.read(NM_AIF);
	neuron[NEURON_SIZE + 2] = (uint8_t)((read_val >> 8) & 0x00FF);
	neuron[NEURON_SIZE + 3] = (uint8_t)(read_val & 0x00FF);
	read_val = spi.read(NM_MINIF);
	neuron[NEURON_SIZE + 4] = (uint8_t)((read_val >> 8) & 0x00FF);
	neuron[NEURON_SIZE + 5] = (uint8_t)(read_val & 0x00FF);
	read_val = spi.read(NM_CAT);
	neuron[NEURON_SIZE + 6] = (uint8_t)((read_val >> 8) & 0x00FF);
	neuron[NEURON_SIZE + 7] = (uint8_t)(read_val & 0x00FF);
	spi.write(NM_NSR, temp_nsr);		// set the NN back to its calling status
}

//----------------------------------------------------------------------------
// Read the contents of the committed neurons and output in a generic format compatible with
// all NeuroMem chips regardless of their neuron size and capacity
// The neurons array should be sized to HEADERSIZE + MAXNEURON * (NEURONSIZE + 8)
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//----------------------------------------------------------------------------
uint16_t NeuroShield::readNeurons(uint8_t neurons[])
{
	uint32_t offset = 8;
	
	uint16_t ncount = spi.read(NM_NCOUNT);
	uint16_t temp_nsr = spi.read(NM_NSR);		// save value to restore NN status upon exit
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	neurons[0] = offset;				// default header size of 8 bytes
	neurons[1] = 0;						// reserved to encode a future format identifier
	neurons[2] = 0x01;
	neurons[3] = 0x00;
	neurons[4] = 0;
	neurons[5] = 0;
	neurons[6] = ((ncount >> 8) & 0x00FF);
	neurons[7] = (ncount & 0x00FF);
	for (int i = 0; i < ncount; i++) {
		uint16_t read_val = spi.read(NM_NCR);
		neurons[offset] = (uint8_t)((read_val >> 8) & 0x00FF);
		neurons[offset + 1] = (uint8_t)(read_val & 0x00FF);
		for (int j = 0; j < NEURON_SIZE; j++) {
			read_val = spi.read(NM_COMP);
			neurons[offset + 2 + j] = (uint8_t)(read_val & 0x00FF);
		}
		read_val = spi.read(NM_AIF);
		neurons[offset + NEURON_SIZE + 2] = (uint8_t)((read_val >> 8) & 0x00FF);
		neurons[offset + NEURON_SIZE + 3] = (uint8_t)(read_val & 0x00FF);
		read_val = spi.read(NM_MINIF);
		neurons[offset + NEURON_SIZE + 4] = (uint8_t)((read_val >> 8) & 0x00FF);
		neurons[offset + NEURON_SIZE + 5] = (uint8_t)(read_val & 0x00FF);
		read_val = spi.read(NM_CAT);
		neurons[offset + NEURON_SIZE + 6] = (uint8_t)((read_val >> 8) & 0x00FF);
		neurons[offset + NEURON_SIZE + 7] = (uint8_t)(read_val & 0x00FF);
		offset += (NEURON_SIZE + 8);
	}
	spi.write(NM_NSR, temp_nsr);			// set the NN back to its calling status
	return(ncount);
}

//---------------------------------------------------------------------
// Clear the committed neurons and restore a new content for the neurons
// from an input array formatted as follows
// Default header size of 8 bytes: headerSize, reserved, 2-bytes MaxLength, 4-bytes neuronCount
// followed by neuroCount record of (MaxLength + 8) bytes: 
// 2-bytes NCR, MaxLength-bytes COMP, 2-bytes AIF, 2-bytes MINIF, 2-bytes CAT
//---------------------------------------------------------------------
uint16_t NeuroShield::writeNeurons(uint8_t neurons[])
{
	uint32_t offset = neurons[0];
	uint16_t ncount = (neurons[6] << 8) + neurons[7];
	if (ncount > total_neurons)
		ncount = total_neurons;
	uint16_t temp_nsr = spi.read(NM_NSR);		// save value to restore NN status upon exit
	uint16_t temp_gcr = spi.read(NM_GCR);
	spi.write(NM_FORGET, 0);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	for (int i = 0; i < ncount; i++) {
		spi.write(NM_NCR, neurons[offset + 1]);
		for (int j = 0; j < NEURON_SIZE; j++)
			spi.write(NM_COMP, neurons[offset + 2 + j]);
		spi.write(NM_AIF, (neurons[offset + NEURON_SIZE + 2] << 8) + neurons[offset + NEURON_SIZE + 3]);
		spi.write(NM_MINIF, (neurons[offset + NEURON_SIZE + 4] << 8) + neurons[offset + NEURON_SIZE + 5]);
		spi.write(NM_CAT, (neurons[offset + NEURON_SIZE + 6] << 8) + neurons[offset + NEURON_SIZE + 7]);
		offset += (NEURON_SIZE + 8);
	}
	spi.write(NM_NSR, temp_nsr);			// set the NN back to its calling status
	spi.write(NM_GCR, temp_gcr);
	return(spi.read(NM_NCOUNT));
}

// --------------------------------------------------------
// Get/Set the NM500 register value
//---------------------------------------------------------
uint16_t NeuroShield::testCommand(uint8_t read_write, uint8_t reg, uint16_t data)
{
	if (read_write == 0) {
		return(spi.read(reg));
	}
	else if (read_write == 1) {
		spi.write(reg, data);
		return(0);
	}
	
	return(0);
}

// ----------------------------------------------------------------
// Get FPGA Version
// ----------------------------------------------------------------
uint16_t NeuroShield::fpgaVersion()
{
	return(spi.version());
}

// ----------------------------------------------------------------
// Excute NM500 SW Reset
// ----------------------------------------------------------------
void NeuroShield::nm500Reset()
{
	spi.reset();
}

// ----------------------------------------------------------------
// Select LED Scenario
// ----------------------------------------------------------------
void NeuroShield::ledSelect(uint8_t data)
{
	spi.ledSelect(data);
}