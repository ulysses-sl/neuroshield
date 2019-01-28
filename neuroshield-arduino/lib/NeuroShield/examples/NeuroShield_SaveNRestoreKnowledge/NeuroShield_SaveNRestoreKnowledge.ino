/******************************************************************************
 *  NM500 NeuroShield Board SaveNRestoreKnowledge
 *  Simple Test Script to understand how the neurons learn and recognize
 *  revision 1.1.5, 01/25, 2019
 *  Copyright (c) 2017 nepes inc.
 *  
 *  Please use the NeuroShield library v1.1.5 or later.
 *  https://github.com/nepes-ai/neuroshield 
 ******************************************************************************/

#include <NeuroShield.h>

#define NM500_SPI_SS 7

#define VECTOR_LENGTH 4
#define READ_COUNT 3

NeuroShield hnn;

uint8_t vector[VECTOR_LENGTH];
uint16_t vector16[VECTOR_LENGTH];
uint16_t dists[READ_COUNT], cats[READ_COUNT], nids[READ_COUNT];
uint16_t fpga_version;
uint16_t ncr, aif, cat, dist, nid, nsr, ncount, minif, response_nbr, norm_lsup = 0;
int i, j;
void displayNeurons();

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ; // wait for the serial port to open

  // NeuroShield Begin
  uint16_t begin = hnn.begin(NM500_SPI_SS);
  if (begin != 0) {
    fpga_version = hnn.fpgaVersion();
    if ((fpga_version & 0xFF00) == 0x0000) {
      Serial.print("\n#### NeuroShield Board");
    } else if ((fpga_version & 0xFF00) == 0x0100) {
      Serial.print("\n#### Prodigy Board");
    } else {
      Serial.print("\n#### Unknown Board");
    }
    Serial.print(" (Board v");
    Serial.print((fpga_version >> 4) & 0x000F);
    Serial.print(".0");
    Serial.print(" / FPGA v");
    Serial.print(fpga_version & 0x000F);
    Serial.print(".0)");
    Serial.print(" ####\n");
    Serial.print("\nNM500 is initialized!");
    Serial.print("\nThere are ");
    Serial.print(hnn.total_neurons);
    Serial.print(" neurons\n");
  } else {
    Serial.print("\nNM500 is NOT properly connected!!");
    Serial.print("\nCheck the connection and Reboot again!\n");
    while (1)
      ;
  }

  // if you want to run in lsup mode, uncomment below
  //norm_lsup = 0x80;
  hnn.setGcr(1 + norm_lsup);

  // build knowledge by learning 3 patterns with each constant values (respectively 11, 15 and 20)
  Serial.print("\nLearning three patterns...");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 11;
  hnn.learn(vector, VECTOR_LENGTH, 1);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 15;
  hnn.learn(vector, VECTOR_LENGTH, 2);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 20;
  hnn.learn(vector, VECTOR_LENGTH, 3);

  displayNeurons();

  // save knowledge to SD card (1)
  Serial.print("\n\nSave Neurons To SD card (save_1.knf)");

  uint16_t sdResult = hnn.saveKnowledgeToSDcard("save_1.knf");

  // Check SD card Error
  if (sdResult > 0) {
    Serial.print("\n ** SD card Error. SD card Functions not available."); 
    Serial.print("(Error Code: "); Serial.print(sdResult); Serial.print(")");
  }

  // Clear all neurons
  Serial.print("\n\nClear all neurons");
  hnn.clearNeurons();
  Serial.print("\n  Display the neurons, ncount=");
  Serial.print(hnn.getNcount());

  // build knowledge by learning 3 patterns with each constant values (respectively 100, 180 and 200)
  Serial.print("\n\nLearning three patterns...");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 100;
  hnn.learn(vector, VECTOR_LENGTH, 100);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 180;
  hnn.learn(vector, VECTOR_LENGTH, 200);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 200;
  hnn.learn(vector, VECTOR_LENGTH, 300);

  displayNeurons();

  // save knowledge to SD card (2)
  Serial.print("\n\nSave Neurons To SD card (save_2.knf)");
  sdResult = hnn.saveKnowledgeToSDcard("save_2.knf");

  // Check SD card Error
  if (sdResult > 0) {
    Serial.print("\n ** SD card Error. SD card Functions not available."); 
    Serial.print("(Error Code: "); Serial.print(sdResult); Serial.print(")");
  }

  // Clear all neurons
  Serial.print("\n\nClear all neurons");
  hnn.clearNeurons();
  Serial.print("\n  Display the neurons, ncount=");
  Serial.print(hnn.getNcount());

  // restore knowledge (save_1.knf)
  Serial.print("\n\nRestore neuron data (save_1.knf)");
  sdResult = hnn.loadKnowledgeFromSDcard("save_1.knf");

  // Check SD card Error
  if (sdResult > 0) {
    Serial.print("\n ** SD card Error. SD card Functions not available."); 
    Serial.print("(Error Code: "); Serial.print(sdResult); Serial.print(")");
  }

  // check the restore result
  displayNeurons();
}

// --------------------------------------------------------
// display the content of the committed neurons
// --------------------------------------------------------
void displayNeurons() {
  ncount = hnn.getNcount();
  Serial.print("\n  Display the neurons, ncount=");
  Serial.print(ncount);
  uint16_t temp_nsr = hnn.getNsr();
  hnn.setNsr(0x0010);
  hnn.resetChain();
  for (i = 1; i <= ncount; i++) {
    ncr = hnn.getNcr();
    hnn.readCompVector(vector16, VECTOR_LENGTH);
    aif = hnn.getAif();
    cat = hnn.getCat();
    Serial.print("\n  neuron#");
    Serial.print(i);
    Serial.print("\tvector=");
    for (j = 0; j < VECTOR_LENGTH; j++) {
      Serial.print(vector16[j]);
      Serial.print(", ");
    }
    Serial.print("\tncr=");
    Serial.print(ncr);
    Serial.print("\taif=");
    Serial.print(aif);
    Serial.print("\tcat=");
    Serial.print(cat & 0x7FFF);
    if (cat & 0x8000)
      Serial.print(" (degenerated)");
  }
  hnn.setNsr(temp_nsr);
}

void loop() {
  ;
}