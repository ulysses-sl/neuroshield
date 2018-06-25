/******************************************************************************
 *  NM500 NeuroShield Board SaveKnowledge
 *  Save and Restore the Knowledge data to sd-card
 *  revision 1.1.3, 01/03, 2018
 *  Copyright (c) 2017 nepes inc.
 *  
 *  Please use the NeuroShield library v1.1.3 or later
 *  https://github.com/nepes-ai/neuroshield
 * 
 *  Dependencies
 *  https://github.com/greiman/SdFat
 ******************************************************************************/

#include <NeuroShield.h>
#include <SdFat.h>

#define SD_SS 6
#define NM500_SPI_SS 7

#define VECTOR_LENGTH 4

#define KNOWLEDGE_FILE "knowledge.hex"

NeuroShield hnn;
SdFat SD;
File write_file, read_file;
bool sd_detected = false;
uint16_t ncount, ncr, aif, minif, cat;
uint16_t fpga_version;
uint8_t vector[VECTOR_LENGTH];
uint16_t vector16[VECTOR_LENGTH];
#ifndef ARDUINO_AVR_UNO
uint16_t neuron[NEURON_SIZE + 4];
#endif

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
    Serial.print("\nThere are "); Serial.print(hnn.total_neurons); Serial.print(" neurons");
  }
  else {
    Serial.print("\nNM500 is NOT properly connected!!");
    Serial.print("\nCheck the connection and Reboot again!\n");
    while (1);
  }

  if (SD.begin(SD_SS)) {
    sd_detected = true;
  }
  else {
    Serial.print("\n\nSD.begin() error");
  }

  Serial.print("\n\nSave and Restore the Knowledge data to sd-card");
  Serial.print("\nLearning three patterns");
  for (int i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 11;
  hnn.learn(vector, VECTOR_LENGTH, 55);
  for (int i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 15;
  hnn.learn(vector, VECTOR_LENGTH, 33);
  for (int i = 0; i < VECTOR_LENGTH; i++)
    vector[i] = 20;
  hnn.learn(vector, VECTOR_LENGTH, 100);
  displayNeurons();
  saveKnowledge();
  
  Serial.print("\n\nClear all neurons");
  hnn.clearNeurons();
  // display the content of the committed neurons
  ncount = hnn.getNcount();
  Serial.print("\n  Display the neurons, ncount="); Serial.print(ncount);
  
  Serial.print("\n\nRestore neuron data");
  loadKnowledge();
  displayNeurons();
}

void loop()
{
  ;
}

void displayNeurons()
{
  // display the content of the committed neurons
  ncount = hnn.getNcount();
  Serial.print("\n  Display the neurons, ncount="); Serial.print(ncount);
  uint16_t temp_nsr = hnn.getNsr();
  hnn.setNsr(0x0010);
  hnn.resetChain();
  for (int i = 1; i <= ncount; i++) {
      ncr = hnn.getNcr();
      hnn.readCompVector(vector16, VECTOR_LENGTH);
      aif = hnn.getAif();
      cat = hnn.getCat();
      Serial.print("\n  neuron#"); Serial.print(i); Serial.print("\tvector=");
      for (int j = 0; j < VECTOR_LENGTH; j++) {
        Serial.print(vector16[j]); Serial.print(", ");
      }
      Serial.print("\tncr="); Serial.print(ncr);  
      Serial.print("\taif="); Serial.print(aif);     
      Serial.print("\tcat="); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
  }
  hnn.setNsr(temp_nsr);
}

int saveKnowledge()
{
  if (!sd_detected) {
    Serial.print("\nSD.begin() error");
    return(0);
  }

  write_file = SD.open(KNOWLEDGE_FILE, (O_READ | O_WRITE | O_CREAT | O_TRUNC));
  if (write_file == 0) {
    Serial.print("\nSD.open() error");
    return(0);
  }
  
  uint16_t ncount, header[4] = {0x1704, 0, 0, 0};
  header[1] = NEURON_SIZE;
  header[2] = ncount = hnn.getNcount();
  //header[3] = reserved for upper byte of ncount
  write_file.write(header, ((sizeof(uint16_t)) * 4));

#ifdef ARDUINO_AVR_UNO
  uint16_t data, temp_nsr = hnn.getNsr();
  hnn.setNsr(0x0010);
  hnn.resetChain();
  for (int i = 1; i <= ncount; i++) {
    ncr = hnn.getNcr();
    write_file.write(&ncr, sizeof(uint16_t));
    for (int j = 0; j < NEURON_SIZE; j++) {
      data = hnn.getComp();
      write_file.write(&data, sizeof(uint16_t));
    }
    aif = hnn.getAif();
    write_file.write(&aif, sizeof(uint16_t));
    minif = hnn.getMinif();
    write_file.write(&minif, sizeof(uint16_t));
    cat = hnn.getCat();
    write_file.write(&cat, sizeof(uint16_t));
  }
  hnn.setNsr(temp_nsr);
  hnn.setPowerSave();
#else
  for (int i = 1; i <= ncount; i++) {
    hnn.readNeuron(i, neuron);    // starting index is 1
    write_file.write(neuron, (sizeof(neuron)));
  }
#endif

  write_file.close();
  return(1);
}

int loadKnowledge()
{
  if (!sd_detected) {
    Serial.print("\nSD.begin() error");
    return(0);
  }

  read_file = SD.open(KNOWLEDGE_FILE);
  if (read_file == 0) {
    Serial.print("\nSD.open() error");
    return(0);
  }

  uint16_t ncount, header[4];
  read_file.read(header, ((sizeof(uint16_t)) * 4));
  if (header[0] != 0x1704) {
    Serial.print("\nHeader error");
    return(0);
  }
  if (header[1] != NEURON_SIZE) {
    Serial.print("\nNeuron size error");
    return(0);
  }
  if (header[2] > hnn.total_neurons) {
    Serial.print("\nNcount error");
    return(0);
  }
  else {
    ncount = header[2];
  }

#ifdef ARDUINO_AVR_UNO
  uint16_t data, temp_nsr = hnn.getNsr();
  hnn.forget();
  hnn.setNsr(0x0010);
  hnn.resetChain();
  while (read_file.available()) {
    for (int i = 1; i <= ncount; i++) {
      read_file.read(&data, sizeof(uint16_t));
      hnn.setNcr(data);
      for (int j = 0; j < NEURON_SIZE; j++) {
        read_file.read(&data, sizeof(uint16_t));
        hnn.setComp(data);
      }
      read_file.read(&data, sizeof(uint16_t));
      hnn.setAif(data);
      read_file.read(&data, sizeof(uint16_t));
      hnn.setMinif(data);
      read_file.read(&data, sizeof(uint16_t));
      hnn.setCat(data);
    }
    break;
  }
  hnn.setNsr(temp_nsr);
#else
  uint16_t temp_nsr = hnn.getNsr();
  hnn.forget();
  hnn.setNsr(0x0010);
  hnn.resetChain();
  while(read_file.available()) {
    for (int i = 1; i <= ncount; i++) {
      if (read_file.read(neuron, ((sizeof(uint16_t)) * (NEURON_SIZE + 4))) == -1) {
        Serial.print("\nread() error");
        hnn.setNsr(temp_nsr);
        return(0);
      }
      hnn.setNcr(neuron[0]);
      hnn.writeCompVector(&neuron[1], NEURON_SIZE);
      hnn.setAif(neuron[1 + NEURON_SIZE]);
      hnn.setMinif(neuron[2 + NEURON_SIZE]);
      hnn.setCat(neuron[3 + NEURON_SIZE]);
    }
  }
  hnn.setNsr(temp_nsr);
#endif

  read_file.close();
  return(1);
}

