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

/*
 * Revision History (v1.1.4)
 * 2018/06/22    v1.1.4    Minor changes
 * 2018/01/03    v1.1.3    Add burst-mode read
 * 2017/12/20    v1.1.2    Modify the structure of neurondata
 * 2017/12/11    v1.1.1    Add Powersave command and Minor changes to the library
 * 2017/08/17    v1.0.0    First Release
 */

#include <NeuroShield.h>
#include <NeuroShieldSPI.h>

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
	bool read_value = spi.connect(ARDUINO_SS);	// default CS.

	if (read_value != 1) {
		return(0);
	}
	else {
		countTotalNeurons();
		clearNeurons();

		uint16_t fpga_version = spi.version();
		if ((fpga_version != 0x0001) && (fpga_version != 0x0002))
			support_burst_read = 1;

		return(total_neurons);
	}
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

		uint16_t fpga_version = spi.version();
		if ((fpga_version != 0x0001) && (fpga_version != 0x0002))
			support_burst_read = 1;

		return(total_neurons);
	}
}

// --------------------------------------------------------
// Get/Set the Neuron Context Register
//---------------------------------------------------------
void NeuroShield::setNcr(uint16_t value)
{
	spi.write(NM_NCR, value);
	POWERSAVE;
}

uint16_t NeuroShield::getNcr()
{
	uint16_t ret_val = spi.read(NM_NCR);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the COMP register (component)
//---------------------------------------------------------
void NeuroShield::setComp(uint8_t value)
{
	spi.write(NM_COMP, (value & 0x00FF));
	POWERSAVE;
}

uint8_t NeuroShield::getComp()
{
	uint8_t ret_val = (uint8_t)(spi.read(NM_COMP) & 0x00FF);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the LCOMP register (last component)
//---------------------------------------------------------
void NeuroShield::setLastComp(uint8_t value)
{
	spi.write(NM_LCOMP, (value & 0x00FF));
	POWERSAVE;
}

// --------------------------------------------------------
// Set the Component Index register
//---------------------------------------------------------
void NeuroShield::setIndexComp(uint16_t value)
{
	spi.write(NM_INDEXCOMP, value);
	POWERSAVE;
}

// --------------------------------------------------------
// Get the Distance register
//---------------------------------------------------------
uint16_t NeuroShield::getDist()
{
	uint16_t ret_val = spi.read(NM_DIST);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the Category register
//---------------------------------------------------------
void NeuroShield::setCat(uint16_t value)
{
	spi.write(NM_CAT, value);
	POWERSAVE;
}

uint16_t NeuroShield::getCat()
{
	uint16_t ret_val = spi.read(NM_CAT);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the AIF register
//---------------------------------------------------------
void NeuroShield::setAif(uint16_t value)
{
	spi.write(NM_AIF, value);
	POWERSAVE;
}

uint16_t NeuroShield::getAif()
{
	uint16_t ret_val = spi.read(NM_AIF);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the Minimum Influence Field register
//---------------------------------------------------------
void NeuroShield::setMinif(uint16_t value)
{
	spi.write(NM_MINIF, value);
	POWERSAVE;
}

uint16_t NeuroShield::getMinif()
{
	uint16_t ret_val = spi.read(NM_MINIF);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get/Set the Maximum Influence Field register
//---------------------------------------------------------
void NeuroShield::setMaxif(uint16_t value)
{
	spi.write(NM_MAXIF, value);
	POWERSAVE;
}

uint16_t NeuroShield::getMaxif()
{
	uint16_t ret_val = spi.read(NM_MAXIF);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Get the Neuron Identifier
//---------------------------------------------------------
uint16_t NeuroShield::getNid()
{
	uint16_t ret_val = spi.read(NM_NID);
	POWERSAVE;
	return(ret_val);
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
	POWERSAVE;
}

uint16_t NeuroShield::getGcr()
{
	uint16_t ret_val = spi.read(NM_GCR);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Reset the chain to first neuron in SR Mode
//---------------------------------------------------------
void NeuroShield::resetChain()
{
	spi.write(NM_RSTCHAIN, 0);
	POWERSAVE;
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
	POWERSAVE;
}

uint16_t NeuroShield::getNsr()
{
	uint16_t ret_val = spi.read(NM_NSR);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Read the number of committed neurons
//---------------------------------------------------------
uint16_t NeuroShield::getNcount()
{
	uint16_t ret_val = spi.read(NM_NCOUNT);
	POWERSAVE;
	return(ret_val);
}

// --------------------------------------------------------
// Set the PowerSave mode
//---------------------------------------------------------
void NeuroShield::setPowerSave()
{
	spi.write(NM_POWERSAVE, 1);
	POWERSAVE;
}

// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn
// Reset the Maximum Influence Field to default value=0x4000
// ------------------------------------------------------------ 
void NeuroShield::forget()
{
	spi.write(NM_FORGET, 0);
	POWERSAVE;
}

// ------------------------------------------------------------ 
// Un-commit all the neurons, so they become ready to learn,
// Set the Maximum Influence Field (default value=0x4000)
// ------------------------------------------------------------ 
void NeuroShield::forget(uint16_t maxif)
{
	spi.write(NM_FORGET, 0);
	spi.write(NM_MAXIF, maxif);
	POWERSAVE;
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
	POWERSAVE;
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
	spi.write(NM_TESTCAT, 1);
	spi.write(NM_NSR, 0x0000);
	for (int i = 0; i < NEURON_SIZE; i++) {
		spi.write(NM_INDEXCOMP, i);
		spi.write(NM_TESTCOMP, 0);
	}
	spi.write(NM_FORGET, 0);
	POWERSAVE;
}

// --------------------------------------------------------
// Broadcast a vector to the neurons and return the recognition status
// 0=unknown, 4=uncertain, 8=Identified
//---------------------------------------------------------
uint16_t NeuroShield::broadcast(uint8_t vector[], uint16_t length)
{
	uint16_t ret_val;
	spi.writeVector(vector, (length - 1));
	spi.write(NM_LCOMP, vector[length - 1]);
	ret_val = spi.read(NM_NSR);
	POWERSAVE;
	return(ret_val);
}

//-----------------------------------------------
// Learn a vector using the current context value
//----------------------------------------------
uint16_t NeuroShield::learn(uint8_t vector[], uint16_t length, uint16_t category)
{
	uint16_t ret_val;
	broadcast(vector, length);
	spi.write(NM_CAT, category);
	ret_val = spi.read(NM_NCOUNT);
	POWERSAVE;
	return(ret_val);
}

// ---------------------------------------------------------
// Classify a vector and return its classification status
// NSR=0, unknown
// NSR=8, identified
// NSR=4, uncertain
// ---------------------------------------------------------
uint16_t NeuroShield::classify(uint8_t vector[], uint16_t length)
{
	uint16_t ret_val;
	broadcast(vector, length);
	ret_val = spi.read(NM_NSR);
	POWERSAVE;
	return(ret_val);
}

//----------------------------------------------
// Recognize a vector and return the best match, or the 
// category, distance and identifier of the top firing neuron
//----------------------------------------------
uint16_t NeuroShield::classify(uint8_t vector[], uint16_t length, uint16_t* distance, uint16_t* category, uint16_t* nid)
{
	uint16_t ret_val;
	broadcast(vector, length);
	*distance = spi.read(NM_DIST);
	*category = spi.read(NM_CAT);
	*nid = spi.read(NM_NID);
	ret_val = spi.read(NM_NSR);
	POWERSAVE;
	return(ret_val);
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
			category[i] = 0xFFFF;
			nid[i] = 0xFFFF;
		}
		else {
			recog_nbr++;
			category[i] = spi.read(NM_CAT);
			nid[i] = spi.read(NM_NID);
		}
	}
	POWERSAVE;
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
	POWERSAVE;
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
	POWERSAVE;
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
	POWERSAVE;
}

// --------------------------------------------------------
// Set the neurons in Radial Basis Function mode (default)
//---------------------------------------------------------
void NeuroShield::setRbfClassifier()
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, (temp_nsr & 0x00DF));
	POWERSAVE;
}

// --------------------------------------------------------
// Set the neurons in K-Nearest Neighbor mode
//---------------------------------------------------------
void NeuroShield::setKnnClassifier()
{
	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, (temp_nsr | 0x0020));
	POWERSAVE;
}

//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting at index 1
//-------------------------------------------------------------
void NeuroShield::readNeuron(uint16_t nid, uint16_t model[], uint16_t* ncr, uint16_t* aif, uint16_t* cat)
{
	if (nid == 0) {
		*ncr = 0xFFFF;
		*aif = 0xFFFF;
		*cat = 0xFFFF;
		return;
	}

	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	if (nid > 1) {
		// move to index in the chain of neurons
		for (int i = 1; i < nid; i++)
			spi.read(NM_CAT);
	}
	
	*ncr = spi.read(NM_NCR);
	if (support_burst_read == 1) {
		spi.readVector16(model, NEURON_SIZE);
	}
	else {
		for (int i = 0; i < NEURON_SIZE; i++)
			model[i] = spi.read(NM_COMP);
	}
	*aif = spi.read(NM_AIF);
	*cat = spi.read(NM_CAT);
	spi.write(NM_NSR, temp_nsr);		// set the NN back to its calling status
	POWERSAVE;
}

//-------------------------------------------------------------
// Read the contents of the neuron pointed by index in the chain of neurons
// starting index is 1
// Returns an array of (NEURON_SIZE + 4) words with the following format
// NCR, NEURON_SIZE * COMP, AIF, MINIF, CAT
//-------------------------------------------------------------
void NeuroShield::readNeuron(uint16_t nid, uint16_t neuron[])
{
	if (nid == 0) {
		for (int i = 0; i < (NEURON_SIZE + 4); i++) {
			neuron[i] = 0xFFFF;
		}
		return;
	}

	uint16_t temp_nsr = spi.read(NM_NSR);
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	if (nid > 1) {
		// move to index in the chain of neurons
		for (int i = 1; i < nid; i++)
			spi.read(NM_CAT);
	}

	neuron[0] = spi.read(NM_NCR);
	if (support_burst_read == 1) {
		spi.readVector16(&neuron[1], NEURON_SIZE);
	}
	else {
		for (int i = 0; i < NEURON_SIZE; i++)
			neuron[i + 1] = spi.read(NM_COMP);
	}
	neuron[NEURON_SIZE + 1] = spi.read(NM_AIF);
	neuron[NEURON_SIZE + 2] = spi.read(NM_MINIF);
	neuron[NEURON_SIZE + 3] = spi.read(NM_CAT);
	spi.write(NM_NSR, temp_nsr);		// set the NN back to its calling status
	POWERSAVE;
}

//----------------------------------------------------------------------------
// Read the contents of the committed neurons
// The output array has a dimension ncount * neurondata
// neurondata describes the content of a neuron and has a dimension (NEURON_SIZE + 4) words
// and with the following format NCR, NEURON_SIZE * COMP, AIF, MINIF, CAT
//----------------------------------------------------------------------------
uint16_t NeuroShield::readNeurons(uint16_t neurons[])
{
	uint32_t offset = 0;
	uint16_t ncount = spi.read(NM_NCOUNT);
	uint16_t temp_nsr = spi.read(NM_NSR);		// save value to restore NN status upon exit
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	for (int i = 1; i <= ncount; i++) {
		neurons[offset + 0] = spi.read(NM_NCR);
		if (support_burst_read == 1) {
			spi.readVector16(&neurons[offset + 1], NEURON_SIZE);
		}
		else {
			for (int j = 0; j < NEURON_SIZE; j++) {
				neurons[offset + 1 + j] = spi.read(NM_COMP);
			}
		}
		neurons[offset + 1 + NEURON_SIZE] = spi.read(NM_AIF);
		neurons[offset + 2 + NEURON_SIZE] = spi.read(NM_MINIF);
		neurons[offset + 3 + NEURON_SIZE] = spi.read(NM_CAT);
		offset += (NEURON_SIZE + 4);
	}
	spi.write(NM_NSR, temp_nsr);				// set the NN back to its calling status
	POWERSAVE;
	return(ncount);
}

void NeuroShield::readCompVector(uint16_t* data, uint16_t size)
{
	if (support_burst_read == 1) {
		spi.readVector16(data, size);
	}
	else {
		for (int i = 0; i < size; i++) {
			*data = spi.read(NM_COMP);
			data++;
		}
	}
	POWERSAVE;
}

//---------------------------------------------------------------------
// Clear the neurons and write their content from an input array
// The input array has a dimension ncount * neurondata
// neurondata describes the content of a neuron and has a dimension (NEURON_SIZE + 4) words
// and with the following format NCR, NEURON_SIZE * COMP, AIF, MINIF, CAT
//---------------------------------------------------------------------
void NeuroShield::writeNeurons(uint16_t neurons[], uint16_t ncount)
{
	uint32_t offset = 0;
	if (ncount > total_neurons)
		ncount = total_neurons;
	uint16_t temp_nsr = spi.read(NM_NSR);		// save value to restore NN status upon exit
	uint16_t temp_gcr = spi.read(NM_GCR);
	clearNeurons();
	spi.write(NM_NSR, 0x0010);
	spi.write(NM_RSTCHAIN, 0);
	for (int i = 0; i < ncount; i++) {
		spi.write(NM_NCR, neurons[offset + 0]);
		spi.writeVector16(&neurons[offset + 1], NEURON_SIZE);
		spi.write(NM_AIF, neurons[offset + 1 + NEURON_SIZE]);
		spi.write(NM_MINIF, neurons[offset + 2 + NEURON_SIZE]);
		spi.write(NM_CAT, neurons[offset + 3 + NEURON_SIZE]);
		offset += (NEURON_SIZE + 4);
	}
	spi.write(NM_NSR, temp_nsr);				// set the NN back to its calling status
	spi.write(NM_GCR, temp_gcr);
	POWERSAVE;
}

// --------------------------------------------------------
// Write N-component
//---------------------------------------------------------
void NeuroShield::writeCompVector(uint16_t* data, uint16_t size)
{
	spi.writeVector16(data, size);
	POWERSAVE;
}

// --------------------------------------------------------
// Get/Set the NM500 register value
//---------------------------------------------------------
uint16_t NeuroShield::testCommand(uint8_t read_write, uint8_t reg, uint16_t data)
{
	uint16_t ret_val = 0;
	if (read_write == 0) {
		ret_val = spi.read(reg);
	}
	else if (read_write == 1) {
		spi.write(reg, data);
	}
	POWERSAVE;
	return(ret_val);
}

// ----------------------------------------------------------------
// Get FPGA Version
// [15:8] board type : 00 = NeuroShield, 01 = Prodigy ...
// [ 7:4] board version
// [ 3:0] fpga version
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
