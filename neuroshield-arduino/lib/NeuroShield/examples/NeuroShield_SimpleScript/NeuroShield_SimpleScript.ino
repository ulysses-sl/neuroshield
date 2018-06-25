/******************************************************************************
 *  NM500 NeuroShield Board SimpleScript
 *  Simple Test Script to understand how the neurons learn and recognize
 *  revision 1.1.3, 01/03, 2018
 *  Copyright (c) 2017 nepes inc.
 *  
 *  Please use the NeuroShield library v1.1.3 or later.
 *  https://github.com/nepes-ai/neuroshield 
 ******************************************************************************/

#include <NeuroShield.h>

#define NM500_SPI_SS 7

#define VECTOR_LENGTH 4
#define READ_COUNT 3

NeuroShield hnn;

uint8_t value = 11;
uint8_t vector[VECTOR_LENGTH];
uint16_t ncr, aif, cat, dist, nid, nsr, ncount, minif, response_nbr, norm_lsup = 0;
uint16_t dists[READ_COUNT], cats[READ_COUNT], nids[READ_COUNT];
uint16_t fpga_version;
int i, j;

void setup() {
  Serial.begin(9600);
  while (!Serial);    // wait for the serial port to open
  
  if (hnn.begin(NM500_SPI_SS) != 0) {
    fpga_version = hnn.fpgaVersion();
    if ((fpga_version & 0xFF00) == 0x0000) {
      Serial.print("\n#### NeuroShield Board");
    }
    else if ((fpga_version & 0xFF00) == 0x0100) {
      Serial.print("\n#### Prodigy Board");
    }
    else {
      Serial.print("\n#### Unknown Board");
    }
    Serial.print(" (Board v"); Serial.print((fpga_version >> 4) & 0x000F); Serial.print(".0");
    Serial.print(" / FPGA v"); Serial.print(fpga_version & 0x000F); Serial.print(".0)"); Serial.print(" ####\n");
    Serial.print("\nNM500 is initialized!");
    Serial.print("\nThere are "); Serial.print(hnn.total_neurons); Serial.print(" neurons\n");
  }
  else {
    Serial.print("\nNM500 is NOT properly connected!!");
    Serial.print("\nCheck the connection and Reboot again!\n");
    while (1);
  }

  // if you want to run in lsup mode, uncomment below
  //norm_lsup = 0x80;
  hnn.setGcr(1 + norm_lsup);
  
  // build knowledge by learning 3 patterns with each constant values (respectively 11, 15 and 20)
  Serial.print("\nLearning three patterns...");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 11;
  hnn.learn(vector, VECTOR_LENGTH, 55);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 15;
  hnn.learn(vector, VECTOR_LENGTH, 33);
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 20;
  ncount = hnn.learn(vector, VECTOR_LENGTH, 100);
  // display the content of the committed neurons
  Serial.print("\nDisplay the neurons, count="); Serial.print(ncount);
  uint16_t model[NEURON_SIZE];
  for (i = 1; i <= ncount; i++) {
      hnn.readNeuron(i, model, &ncr, &aif, &cat);
      Serial.print("\nneuron#"); Serial.print(i); Serial.print("\tmodel=");
      for (j = 0; j < VECTOR_LENGTH; j++) {
        Serial.print(model[j]); Serial.print(", ");
      }
      Serial.print("\tncr="); Serial.print(ncr);  
      Serial.print("\taif="); Serial.print(aif);     
      Serial.print("\tcat="); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
  }
  
  for (value = 12; value < 16; value++) {
      Serial.print("\n\nRecognizing a new pattern: ");
      for (i = 0; i < VECTOR_LENGTH; i++)
        vector[i] = value;     
      for (i = 0; i < VECTOR_LENGTH; i++) {
        Serial.print(vector[i]); Serial.print(", ");
      }
      response_nbr = hnn.classify(vector, VECTOR_LENGTH, READ_COUNT, dists, cats, nids);
      for (i = 0; i < response_nbr; i++) {
          Serial.print("\nFiring neuron "); Serial.print(nids[i]);
          Serial.print(", Category="); Serial.print(cats[i] & 0x7FFF); if (cats[i] & 0x8000) Serial.print(" (degenerated)");
          Serial.print(", at Distance="); Serial.print(dists[i]);
      }
  }

  Serial.print("\n\nRecognizing a new pattern using KNN classifier: ");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 20;
  for (i = 0; i < VECTOR_LENGTH; i++) {
    Serial.print(vector[i]); Serial.print(", ");
  }
  hnn.setKnnClassifier();
  response_nbr = hnn.classify(vector, VECTOR_LENGTH, READ_COUNT, dists, cats, nids);
  hnn.setRbfClassifier();
  for (i = 0; i < READ_COUNT; i++) {
      Serial.print("\nFiring neuron "); Serial.print(nids[i]);
      Serial.print(", Category="); Serial.print(cats[i] & 0x7FFF); if (cats[i] & 0x8000) Serial.print(" (degenerated)");
      Serial.print(", at Distance="); Serial.print(dists[i]);
  }

  Serial.print("\n\nLearning a new example (13) falling between neuron1 and neuron2");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 13;
  ncount = hnn.learn(vector, VECTOR_LENGTH, 100);
  // display the content of the committed neurons
  Serial.print("\nDisplay the neurons, count="); Serial.print(ncount);
  for (i = 1; i <= ncount; i++) {
      hnn.readNeuron(i, model, &ncr, &aif, &cat);
      Serial.print("\nneuron#"); Serial.print(i); Serial.print("\tmodel=");
      for (j = 0; j < VECTOR_LENGTH; j++) {
        Serial.print(model[j]); Serial.print(", ");
      }
      Serial.print("\tncr="); Serial.print(ncr);  
      Serial.print("\taif="); Serial.print(aif);     
      Serial.print("\tcat="); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
  }
  Serial.print("\n=> Notice the addition of neuron 4 and the shrinking of the influence fields of neuron1 and 2");

  Serial.print("\n\nLearning a same example (13) using a different category 77");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 13;
  ncount = hnn.learn(vector, VECTOR_LENGTH, 77);
  // display the content of the committed neurons
  Serial.print("\nDisplay the neurons, count="); Serial.print(ncount);
  for (i = 1; i <= ncount; i++) {
      hnn.readNeuron(i, model, &ncr, &aif, &cat);
      Serial.print("\nneuron#"); Serial.print(i); Serial.print("\tmodel=");
      for (j = 0; j < VECTOR_LENGTH; j++) {
        Serial.print(model[j]); Serial.print(", ");
      }
      Serial.print("\tncr="); Serial.print(ncr);  
      Serial.print("\taif="); Serial.print(aif);     
      Serial.print("\tcat="); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
  }
  Serial.print("\n=> Notice if the AIF of a neuron reaches the MINIF, the neuron will be degenerated");

  Serial.print("\n\nLearning a new example (12) using context 5, category 200");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 12;
  hnn.setContext(5);
  ncount = hnn.learn(vector, VECTOR_LENGTH, 200);
  hnn.setContext(1);
  // display the content of the committed neurons
  Serial.print("\nDisplay the neurons, count="); Serial.print(ncount);
  for (i = 1; i <= ncount; i++) {
      hnn.readNeuron(i, model, &ncr, &aif, &cat);
      Serial.print("\nneuron#"); Serial.print(i); Serial.print("\tmodel=");
      for (j = 0; j < VECTOR_LENGTH; j++) {
        Serial.print(model[j]); Serial.print(", ");
      }
      Serial.print("\tncr="); Serial.print(ncr);  
      Serial.print("\taif="); Serial.print(aif);     
      Serial.print("\tcat="); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
  }

  Serial.print("\n\nRecognizing a new pattern using context 5: ");
  for (i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 20;
  for (i = 0; i < VECTOR_LENGTH; i++) {
    Serial.print(vector[i]); Serial.print(", ");
  }
  hnn.setContext(5);
  response_nbr = hnn.classify(vector, VECTOR_LENGTH, READ_COUNT, dists, cats, nids);
  hnn.setContext(1);
  for (i = 0; i < response_nbr; i++) {
      Serial.print("\nFiring neuron "); Serial.print(nids[i]);
      Serial.print(", Category="); Serial.print(cats[i] & 0x7FFF); if (cats[i] & 0x8000) Serial.print(" (degenerated)");
      Serial.print(", at Distance="); Serial.print(dists[i]);
  }
  
  Serial.print("\n=> Notice the neurons will not be recognize and shrink if the value of context is not equal");
  
  //Prompt User for input
  Serial.print("\n\n\nEdit a value between [10 and 30] as the amplitude of a new pattern + Enter");
}

void loop()
{
  if (Serial.available() == 3) {
    value = Serial.read() - 48;
    value = value * 10 + Serial.read() - 48;
    Serial.read(); // to read the newline

    for (i = 0; i < VECTOR_LENGTH; i++)
      vector[i] = value;
    Serial.print("\npattern=");
    for (i = 0; i < VECTOR_LENGTH; i++) {
      Serial.print(vector[i]); Serial.print(", ");
    }
 
    response_nbr = hnn.classify(vector, VECTOR_LENGTH, READ_COUNT, dists, cats, nids);
    for (i = 0; i < response_nbr; i++) {
      Serial.print("\nFiring neuron "); Serial.print(nids[i]);
      Serial.print(", Category="); Serial.print(cats[i] & 0x7FFF); if (cats[i] & 0x8000) Serial.print(" (degenerated)");
      Serial.print(", at Distance="); Serial.print(dists[i]);
    }
    Serial.print("\n\nEdit a value between [10 and 30] as the amplitude of a new pattern + Enter");
  }
}
