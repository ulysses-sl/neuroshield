/******************************************************************************
 *  NM500 NeuroShield Board and mpu6050 imu Test example
 *  revision 1.1.3, 01/03, 2018
 *  Copyright (c) 2017 nepes inc.
 *  
 *  Please use the NeuroShield library v1.1.3 or later.
 *  https://github.com/nepes-ai/neuroshield
 * 
 *  Dependencies
 *  https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
 *  https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/I2Cdev
 ******************************************************************************/

#include <Wire.h>
#include <MPU6050.h>
#include <NeuroShield.h>

// for NM500 
#define MOTION_REPEAT_COUNT 3  // number of samples to assemble a vector
#define MOTION_SIGNAL_COUNT 8  // d_ax, d_ay, d_az, d_gx, d_gy, d_gz, da, dg
#define MOTION_CAPTURE_COUNT 20

#define DEFAULT_MAXIF 500

#define NM500_SPI_SS 7

NeuroShield hnn;
MPU6050 mpu(0x68);

int16_t ax, ay, az, gx, gy, gz;

uint8_t learn_cat = 0;     // category to learn
uint8_t prev_cat = 0;  // previously recognized category
uint16_t dist, cat, nid, nsr, ncount;  // response from the neurons
uint16_t prev_ncount = 0;
uint16_t fpga_version;

int16_t min_a = 0xFFFF, max_a = 0, min_g = 0xFFFF, max_g = 0, da = 0, dg = 0;   // reset, or not, at each feature extraction

uint8_t vector[MOTION_REPEAT_COUNT * MOTION_SIGNAL_COUNT];       // vector holding the pattern to learn or recognize

void setup()
{
  // i2c initialize
  Wire.begin();
  
  // serial initialize
  Serial.begin(9600);
  while (!Serial);
  
  // NM500 initialize
  Serial.print("\nStart NM500 initialization...");
  if (hnn.begin(NM500_SPI_SS) != 0) {
    fpga_version = hnn.fpgaVersion();
    if ((fpga_version & 0xFF00) == 0x0000) {
      Serial.print("\n  NeuroShield Board");
    }
    else if ((fpga_version & 0xFF00) == 0x0100) {
      Serial.print("\n  Prodigy Board");
    }
    else {
      Serial.print("\n  Unknown Board");
    }
    Serial.print(" (Board v"); Serial.print((fpga_version >> 4) & 0x000F); Serial.print(".0");
    Serial.print(" / FPGA v"); Serial.print(fpga_version & 0x000F); Serial.print(".0)");
    Serial.print("\n  NM500 is initialized!");
    Serial.print("\n  There are "); Serial.print(hnn.total_neurons); Serial.print(" neurons");
  }
  else {
    Serial.print("\n  NM500 is NOT properly connected!!");
    Serial.print("\n  Check the connection and Reboot again!");
    while (1);
  }

  // initialize mpu6050
  Serial.print("\n\nStart MPU-6050 initialization...");
  mpu.initialize();
  // set gyro & accel range
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);

  // verify connection
  for (int i = 0; i < 10; i++) {
    if (mpu.testConnection()) {
      Serial.print("\n  MPU-6050 connection successful!");
      break;
    }
    else if (i == 9) {
      Serial.print("\n  MPU-6050 connection failed");
      Serial.print("\n  Check the connection and Reboot again!");
      while (1);
    }
    delay(100);
  }
  
  // wait for ready
  Serial.print("\n  About to calibrate. Make sure your board is stable and upright");
  Serial.print("\n  Send any character to start calibration");
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available()){
    Serial.print(".");
    delay(1000);
  }                
  while (Serial.available() && Serial.read()); // empty buffer again

  // start message
  Serial.print("\n  Start calibration, don't touch it until you see a finish message");
  // reset offsets
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  mpu6050Calibration();
  
  // end message
  Serial.print("\n  MPU-6050 calibration end!!\n");

  Serial.print("\nEntering loop...");
  Serial.print("\nMove the module vertically or horizontally...");
  Serial.print("\ntype 1 + Enter if up <-> down motion");
  Serial.print("\ntype 2 + Enter if left <-> right motion");
  Serial.print("\ntype 3 + Enter if front <-> back motion");
  Serial.print("\ntype 0 + Enter for any other motion");
}

void loop() {
  // WARNING : make sure the serial printer settings is "new line"

  // learn upon keyboard entry of 1 digit between 0~3
  // otherwise continuously report any change of motion
  if (Serial.available() == 2) {
      learn_cat = Serial.read();
      Serial.read();    // to empty serial buffer of the newline clear
      learn_cat = learn_cat - '0';
      if (learn_cat < 4) {    // learn_cat 0 ~ 3
        Serial.print("\nLearning motion category "); Serial.print(learn_cat);
        for (int i = 0; i < 5; i++) {
          extractFeatureVector();
          ncount = hnn.learn(vector, MOTION_REPEAT_COUNT * MOTION_SIGNAL_COUNT, learn_cat);
          if (ncount != prev_ncount) {
            prev_cat = learn_cat;
            prev_ncount = ncount;
            // Optional display of the vector in the serial monitor
            //Serial.print("\nncount #"); Serial.print(learn_cat);
            //Serial.print(", "); custom_print(ncount); Serial.print("        ");
            //Serial.print(", Vector = "); for (int i = 0; i < MOTION_SIGNAL_COUNT; i++) custom_print(vector[i]);
          }
        }
        Serial.print("\tNeurons="); Serial.print(ncount);
      }
  }
  else {
    // recognize
    extractFeatureVector();
    hnn.classify(vector, MOTION_REPEAT_COUNT * MOTION_SIGNAL_COUNT, &dist, &cat, &nid);
    if (cat != 0xFFFF) {
      prev_cat = cat;
      Serial.print("\nMotion #"); Serial.print(cat & 0x7FFF); if (cat & 0x8000) Serial.print(" (degenerated)");
    }
    else if (prev_cat != 0xFFFF) {
      prev_cat = cat;
    }
  }
}

////////////////////////////////////////////////////////////
// extract feature
////////////////////////////////////////////////////////////
void extractFeatureVector()
{
  int i;
  int16_t min_ax, min_ay, min_az, max_ax, max_ay, max_az;
  int16_t min_gx, min_gy, min_gz, max_gx, max_gy, max_gz;
  uint32_t norm_ax, norm_ay, norm_az, norm_gx, norm_gy, norm_gz;
  int32_t d_ax, d_ay, d_az, d_gx, d_gy, d_gz;
  int32_t da_local, dg_local;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  max_ax = min_ax = ax;
  max_ay = min_ay = ay;
  max_az = min_az = az;
  max_gx = min_gx = gx;
  max_gy = min_gy = gy;
  max_gz = min_gz = gz;
  
  for (i = 0; i < MOTION_CAPTURE_COUNT; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    if (ax < min_ax)
      min_ax = ax;
    else if (ax > max_ax)
      max_ax = ax;
    
    if (ay < min_ay)
      min_ay = ay;
    else if(ay > max_ay)
      max_ay = ay;
    
    if (az < min_az)
      min_az = az;
    else if (az > max_az)
      max_az = az;

    if (gx < min_gx)
      min_gx = gx;
    else if (gx > max_gx)
      max_gx = gx;
    
    if (gy < min_gy)
      min_gy = gy;
    else if (gy > max_gy)
      max_gy = gy;
    
    if (gz < min_gz)
      min_gz = gz;
    else if (gz > max_gz)
      max_gz = gz;
  }
    
  d_ax = max_ax - min_ax;
  d_ay = max_ay - min_ay;
  d_az = max_az - min_az;

  d_gx = max_gx - min_gx;
  d_gy = max_gy - min_gy;
  d_gz = max_gz - min_gz;

  da_local = d_ax;
  if (d_ay > da_local)
    da_local = d_ay;
  if (d_az > da_local)
    da_local = d_az;

  dg_local = d_gx;
  if (d_gy > dg_local)
    dg_local = d_gy;
  if (d_gz > dg_local)
    dg_local = d_gz;

  norm_ax = d_ax; norm_ax = norm_ax * 255 / da_local;
  norm_ay = d_ay; norm_ay = norm_ay * 255 / da_local;
  norm_az = d_az; norm_az = norm_az * 255 / da_local;

  norm_gx = d_gx; norm_gx = norm_gx * 255 / dg_local;
  norm_gy = d_gy; norm_gy = norm_gy * 255 / dg_local;
  norm_gz = d_gz; norm_gz = norm_gz * 255 / dg_local;

  for (i = 0; i < MOTION_REPEAT_COUNT; i++) {
    vector[i * MOTION_SIGNAL_COUNT] = norm_ax & 0x00ff;
    vector[(i * MOTION_SIGNAL_COUNT) + 1] = norm_ay & 0x00ff;
    vector[(i * MOTION_SIGNAL_COUNT) + 2] = norm_az & 0x00ff;
    vector[(i * MOTION_SIGNAL_COUNT) + 3] = norm_gx & 0x00ff;
    vector[(i * MOTION_SIGNAL_COUNT) + 4] = norm_gy & 0x00ff;
    vector[(i * MOTION_SIGNAL_COUNT) + 5] = norm_gz & 0x00ff;
    if (da_local >= 4096)
      vector[(i * MOTION_SIGNAL_COUNT) + 6] = 0xff;
    else
      vector[(i * MOTION_SIGNAL_COUNT) + 6] = ((da_local >> 4) & 0x00ff);
    if (dg_local >= 4096)
      vector[(i * MOTION_SIGNAL_COUNT) + 7] = 0xff;
    else
      vector[(i * MOTION_SIGNAL_COUNT) + 7] = ((dg_local >> 4) & 0x00ff);
  }
}

////////////////////////////////////////////////////////////
// custom formatted printf
// ex) -32156
//     -00023
//       1024
//         34
////////////////////////////////////////////////////////////
void custom_print(int value)
{
  int tmp1;
  int start_print = 0;
  
  if (value < 0) {
    value = 0 - value;
    start_print = 1;
    Serial.print("-");
  }
  else {
    Serial.print(" ");
  }

  if (value >= 10000) {
    tmp1 = value / 10000;
    value -= (tmp1 * 10000);
    start_print = 1;
    Serial.print(tmp1);
  }
  else if (start_print == 1) {
    Serial.print("0");
  }
  else {
    Serial.print(" ");
  }

  if (value >= 1000) {
    tmp1 = value / 1000;
    value -= (tmp1 * 1000);
    start_print = 1;
    Serial.print(tmp1);
  }
  else if (start_print == 1) {
    Serial.print("0");
  }
  else {
    Serial.print(" ");
  }

  if (value >= 100) {
    tmp1 = value / 100;
    value -= (tmp1 * 100);
    start_print = 1;
    Serial.print(tmp1);
  }
  else if (start_print == 1) {
    Serial.print("0");
  }
  else {
    Serial.print(" ");
  }

  if (value >= 10) {
    tmp1 = value / 10;
    value -= (tmp1 * 10);
    start_print = 1;
    Serial.print(tmp1);
  }
  else if (start_print == 1) {
    Serial.print("0");
  }
  else {
    Serial.print(" ");
  }

  Serial.print(value);
}

////////////////////////////////////////////////////////////
// for mpu6050 calibration
// az => 0 (for vector feature extraction)
////////////////////////////////////////////////////////////
void mpu6050Calibration()
{
  int i, j;
  int32_t sum_ax = 0, sum_ay = 0, sum_az = 0, sum_gx = 0, sum_gy = 0, sum_gz = 0;
  int16_t mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
  int16_t ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
  
  for (i = 0; i < 100; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  }

  for (j = 0; j < 3; j++) {
    for (i = 0; i < 100; i++) {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      sum_ax += ax;
      sum_ay += ay;
      sum_az += az;
      sum_gx += gx;
      sum_gy += gy;
      sum_gz += gz;
    }
  
    mean_ax = sum_ax / 100;
    mean_ay = sum_ay / 100;
    mean_az = sum_az / 100;
    mean_gx = sum_gx / 100;
    mean_gy = sum_gy / 100;
    mean_gz = sum_gz / 100;
  
    // MPU6050_GYRO_FS_1000 : offset = (-1) * mean_g
    // MPU6050_ACCEL_FS_8   : offset = (-0.5) * mean_a
    ax_offset = (-mean_ax) / 2;
    ay_offset = (-mean_ay) / 2;
    az_offset = (-mean_az) / 2;
    gx_offset = -mean_gx;
    gy_offset = -mean_gy;
    gz_offset = -mean_gz;
  
    // set...Offset function does not work -> using software offset
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);
    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);
  }
}
