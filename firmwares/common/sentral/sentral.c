#include "FreeRTOS.h"
#include "i2c.h"
#include "sentral.h"
#include "registre_EM.h"
#include "SEGGER_RTT.h"
#include "reg_MPU.h"
#include "cmsis_os.h"
#include "stm32h7xx_hal.h"

#define abs(x) ((x>=0)?(x):-(x))



static I2C_HandleTypeDef *hi2c_sentral;

static void print_float(float f);

static void writeByte(uint8_t ADDRESS_S, uint8_t reg, uint8_t data){
  uint8_t regW[2];
  regW[0] = reg;
  regW[1] = data;
  HAL_I2C_Master_Transmit(hi2c_sentral, ADDRESS_S,regW , 2, HAL_MAX_DELAY);
}

static void readByte(uint8_t ADDRESS_S, uint8_t reg, uint8_t * rep, uint8_t nb_byte){
  uint8_t regR = reg;
  HAL_I2C_Mem_Read(hi2c_sentral, ADDRESS_S, regR, 1, rep, nb_byte,HAL_MAX_DELAY);
}

void sentral_device_info(void){
  uint8_t ROM1[2];
  uint8_t ROM2[2];
  uint8_t RAM1[2];
  uint8_t RAM2[2];
  uint8_t     RID;
  uint8_t     PID;
  SEGGER_RTT_printf(0,"device info\r\n");
  SEGGER_RTT_printf(0,"error hal state i2c %x\r\n",HAL_I2C_GetState(hi2c_sentral));

  SEGGER_RTT_printf(0, "EM7180 ProductID  : 0x: %x   \r\n",PID);


  SEGGER_RTT_printf(0,"error hal state i2c %x\r\n",HAL_I2C_GetState(hi2c_sentral));

  readByte(EM7180_ADDRESS,EM7180_ProductID,&PID,1);
  readByte(EM7180_ADDRESS,EM7180_RevisionID,&RID,1);
  readByte(EM7180_ADDRESS,EM7180_RAMVersion1,RAM1,2);
  readByte(EM7180_ADDRESS,EM7180_RAMVersion2,RAM2,2);
  readByte(EM7180_ADDRESS,EM7180_ROMVersion1,ROM1,2);
  readByte(EM7180_ADDRESS,EM7180_ROMVersion2,ROM2,2);

  SEGGER_RTT_printf(0, "EM7180 ROM Version: 0x: %x%x  %x%x\r\n"
                       "EM7180 RAM Version: 0x: %x%x  %x%x\r\n"
                       "EM7180 ProductID  : 0x: %x   \r\n"
                       "EM7180 REvisionID : 0x: %x   \r\n",
                       ROM1[0],ROM1[1],ROM2[0],ROM2[1],
                       RAM1[0],RAM1[1],RAM2[0],RAM2[1],
                       PID,RID);
}

void sentral_feature_info(void){
  uint8_t     flag;

  readByte(EM7180_ADDRESS,EM7180_FeatureFlags,&flag,1);

  if( flag && 0x01) SEGGER_RTT_printf(0,"A barometer is installed\r\n");
  if( flag & 0x02)  SEGGER_RTT_printf(0,"A humidity sensor is installed\r\n");
  if( flag & 0x04)  SEGGER_RTT_printf(0,"A temperature sensor is installed\r\n");
  if( flag & 0x08)  SEGGER_RTT_printf(0,"A custom sensor is installed\r\n");
  if( flag & 0x10)  SEGGER_RTT_printf(0,"A second custom sensor is installed\r\n");
  if( flag & 0x20)  SEGGER_RTT_printf(0,"A third custom sensor is installed\r\n");
}

void sentral_EEPROM_upload(void){
  uint8_t  status;

  readByte(EM7180_ADDRESS,EM7180_SentralStatus,&status,1);

  if( status & 0x01)  SEGGER_RTT_printf(0,"EEPROM detected on the sensor bus!\r\n");
  if( status & 0x02)  SEGGER_RTT_printf(0,"EEPROM uploaded config file!\r\n");
  if( status & 0x04)  SEGGER_RTT_printf(0,"EEPROM CRC incorrect!\r\n");
  if( status & 0x08)  SEGGER_RTT_printf(0,"EM7180 in initialized state!\r\n");
  if( status & 0x10)  SEGGER_RTT_printf(0,"No EEPROM detected!\r\n");

  // Check SENtral status, make sure EEPROM upload of firmware was accomplished
  while(!(status &0x01)) {

    writeByte(EM7180_ADDRESS, EM7180_ResetRequest , 0x01);
    osDelay(500);

    readByte(EM7180_ADDRESS,EM7180_SentralStatus,&status,1);
    if( status & 0x01)  SEGGER_RTT_printf(0,"EEPROM detected on the sensor bus!\r\n");
    if( status & 0x02)  SEGGER_RTT_printf(0,"EEPROM uploaded config file!\r\n");
    if( status & 0x04)  SEGGER_RTT_printf(0,"EEPROM CRC incorrect!\r\n");
    if( status & 0x08)  SEGGER_RTT_printf(0,"EM7180 in initialized state!\r\n");
    if( status & 0x10)  SEGGER_RTT_printf(0,"No EEPROM detected!\r\n");

  }

  readByte(EM7180_ADDRESS,EM7180_SentralStatus,&status,1);
  if( !(status & 0x04))  SEGGER_RTT_printf(0,"EEPROM upload successful!\r\n");

}


void sentral_config_operating_mode(void){

  // set SENtral in initialized state to configure registers
  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00);
  // make sure pass through mode is off
  writeByte(EM7180_ADDRESS, EM7180_PassThruControl, 0x00);
  //  initialize
  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01);
  // set SENtral in initialized state to configure registers
  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x00);

  //Setup LPF bandwidth (BEFORE setting ODR's)
  writeByte(EM7180_ADDRESS, EM7180_ACC_LPF_BW, 0x03); // 41Hz
  writeByte(EM7180_ADDRESS, EM7180_GYRO_LPF_BW, 0x03); // 41Hz

  // Set accel/gyro/mag desired ODR rates
  writeByte(EM7180_ADDRESS, EM7180_QRateDivisor, 0x02); // 100 Hz
  writeByte(EM7180_ADDRESS, EM7180_MagRate, 0x64); // 100 Hz
  writeByte(EM7180_ADDRESS, EM7180_AccelRate, 0x14); // 200/10 Hz
  writeByte(EM7180_ADDRESS, EM7180_GyroRate, 0x14); // 200/10 Hz

  // Configure operating mode
  writeByte(EM7180_ADDRESS, EM7180_AlgorithmControl, 0x00);
  /* Enable interrupt to host upon certain events
     choose host interrupts when any sensor updated (0x40),new gyro data (0x20),
     new accel data (0x10),new mag data (0x08),quaternions updated (0x04),
     an error occurs (0x02), or the SENtral needs to be reset(0x01)*/
  writeByte(EM7180_ADDRESS, EM7180_EnableEvents, 0x07);
  // Enable EM7180 run mode
  writeByte(EM7180_ADDRESS, EM7180_HostControl, 0x01);
}

void sentral_read_status(void){
  // Read EM7180 status
  uint8_t runStatus;
  readByte(EM7180_ADDRESS, EM7180_RunStatus,&runStatus,1);
  if(runStatus & 0x01) SEGGER_RTT_printf(0," EM7180 run status = normal mode\r\n");


  uint8_t algoStatus;
  readByte(EM7180_ADDRESS, EM7180_AlgorithmStatus,&algoStatus,1);

  if(algoStatus & 0x01) SEGGER_RTT_printf(0,"EM7180 standby status\r\n");
  if(algoStatus & 0x02) SEGGER_RTT_printf(0,"EM7180 algorithm slow\r\n");
  if(algoStatus & 0x04) SEGGER_RTT_printf(0,"EM7180 in stillness mode\r\n");
  if(algoStatus & 0x08) SEGGER_RTT_printf(0,"EM7180 mag calibration completed\r\n");
  if(algoStatus & 0x10) SEGGER_RTT_printf(0,"EM7180 magnetic anomaly detected\r\n");
  if(algoStatus & 0x20) SEGGER_RTT_printf(0,"EM7180 unreliable sensor data\r\n");

  uint8_t passthruStatus;
  readByte(EM7180_ADDRESS, EM7180_PassThruStatus,&passthruStatus,1);
  if(passthruStatus & 0x01) SEGGER_RTT_printf(0," EM7180 in passthru mode!\r\n");

  uint8_t eventStatus;
  readByte(EM7180_ADDRESS, EM7180_EventStatus, &eventStatus,1);
  if(eventStatus & 0x01) SEGGER_RTT_printf(0," EM7180 CPU reset\r\n");
  if(eventStatus & 0x02) SEGGER_RTT_printf(0," EM7180 Error\r\n");
  if(eventStatus & 0x04) SEGGER_RTT_printf(0," EM7180 new quaternion result\r\n");
  if(eventStatus & 0x08) SEGGER_RTT_printf(0," EM7180 new mag result\r\n");
  if(eventStatus & 0x10) SEGGER_RTT_printf(0," EM7180 new accel result\r\n");
  if(eventStatus & 0x20) SEGGER_RTT_printf(0," EM7180 new gyro result\r\n");

  // Check sensor status
  uint8_t sensorStatus;
  readByte(EM7180_ADDRESS, EM7180_SensorStatus,&sensorStatus,1);
  SEGGER_RTT_printf(0," EM7180 sensor status = %x\r\n", sensorStatus);
  if(sensorStatus & 0x01) SEGGER_RTT_printf(0,"Magnetometer not acknowledging!\r\n");
  if(sensorStatus & 0x02) SEGGER_RTT_printf(0,"Accelerometer not acknowledging!\r\n");
  if(sensorStatus & 0x04) SEGGER_RTT_printf(0,"Gyro not acknowledging!\r\n");
  if(sensorStatus & 0x10) SEGGER_RTT_printf(0,"Magnetometer ID not recognized!\r\n");
  if(sensorStatus & 0x20) SEGGER_RTT_printf(0,"Accelerometer ID not recognized!\r\n");
  if(sensorStatus & 0x40) SEGGER_RTT_printf(0,"Gyro ID not recognized!\r\n");

  uint8_t rate ;
  readByte(EM7180_ADDRESS, EM7180_ActualMagRate,&rate,1);
  SEGGER_RTT_printf(0,"Actual MagRate = %d Hz\r\n",rate);

  readByte(EM7180_ADDRESS, EM7180_ActualAccelRate,&rate,1);
  SEGGER_RTT_printf(0,"Actual AccelRate = %d Hz\r\n", rate);

  readByte(EM7180_ADDRESS, EM7180_ActualGyroRate,&rate,1);
  SEGGER_RTT_printf(0,"Actual GyroRate = %d Hz\r\n",rate);

}

static float uint32_reg_to_float (uint8_t *buf)
{
  union {
    uint32_t ui32;
    float f;
  } u;

  u.ui32 =     (((uint32_t)buf[0]) +
               (((uint32_t)buf[1]) <<  8) +
               (((uint32_t)buf[2]) << 16) +
               (((uint32_t)buf[3]) << 24));
  return u.f;
}

static void readSENtralQuatData(quaternion_t *quat)
{
  uint8_t rawData[16];
  // Read the sixteen raw data registers into data array
  readByte(EM7180_ADDRESS, EM7180_QX, &rawData[0],16);
  quat->x = uint32_reg_to_float (&rawData[0]);
  quat->y = uint32_reg_to_float (&rawData[4]);
  quat->z = uint32_reg_to_float (&rawData[8]);
  quat->w = uint32_reg_to_float (&rawData[12]);
  // SENtral stores quats as qx, qy, qz, q0!
}

static void readSENtralAccelData(int16_t * destination)
{
  uint8_t rawData[6];
  readByte(EM7180_ADDRESS, EM7180_AX,&rawData[0],6);
  // Turn the MSB and LSB into a signed 16-bit value
  destination[0] = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);
  destination[1] = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);
  destination[2] = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]);
}

static void readSENtralGyroData(int16_t * destination)
{
  uint8_t rawData[6];
  readByte(EM7180_ADDRESS, EM7180_GX,&rawData[0],6);
  // Turn the MSB and LSB into a signed 16-bit value
  destination[0] = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);
  destination[1] = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);
  destination[2] = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]);
}

static void readSENtralMagData(int16_t * destination)
{
  uint8_t rawData[6];
  readByte(EM7180_ADDRESS, EM7180_MX, &rawData[0],6);
  // Turn the MSB and LSB into a signed 16-bit value
  destination[0] = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);
  destination[1] = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);
  destination[2] = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]);
}


void sentral_data(quaternion_t *quat){

  // Check event status register, way to check data ready by polling rather than interrupt
  uint8_t eventStatus;
  readByte(EM7180_ADDRESS, EM7180_EventStatus,&eventStatus,1);

  // Check for errors
  if(eventStatus & 0x02) {

    uint8_t errorStatus;
    readByte(EM7180_ADDRESS, EM7180_ErrorRegister,&errorStatus,1);
    if(errorStatus != 0x00) {
      SEGGER_RTT_printf(0," EM7180 sensor status = %d \r\n", errorStatus);
      if(errorStatus == 0x11) SEGGER_RTT_printf(0,"Magnetometer failure!\r\n");
      if(errorStatus == 0x12) SEGGER_RTT_printf(0,"Accelerometer failure!\r\n");
      if(errorStatus == 0x14) SEGGER_RTT_printf(0,"Gyro failure!\r\n");
      if(errorStatus == 0x21) SEGGER_RTT_printf(0,"Magnetometer initialization failure!\r\n");
      if(errorStatus == 0x22) SEGGER_RTT_printf(0,"Accelerometer initialization failure!\r\n");
      if(errorStatus == 0x24) SEGGER_RTT_printf(0,"Gyro initialization failure!\r\n");
      if(errorStatus == 0x30) SEGGER_RTT_printf(0,"Math error!\r\n");
      if(errorStatus == 0x80) SEGGER_RTT_printf(0,"Invalid sample rate!\r\n");
    }
  }


  if(eventStatus & 0x04) {
    readSENtralQuatData(quat);
    print_float(quat->w);
    SEGGER_RTT_printf(0, ",");
    print_float(quat->x);
    SEGGER_RTT_printf(0, ",");
    print_float(quat->y);
    SEGGER_RTT_printf(0, ",");
    print_float(quat->z);
    SEGGER_RTT_printf(0, "\n");

  }
}

static void print_float(float f) {
  int integral_part = (int) abs(f);
  float fractionnal_part = (abs(f) - (float) integral_part);
  int i;
  for (i = 0; i < 4; i++)
    fractionnal_part *= 10;
  if (f < 0)
    SEGGER_RTT_printf(0, "-");
  SEGGER_RTT_printf(0, "%d.%04d", integral_part, (int) fractionnal_part);
}

void sentral_init(I2C_HandleTypeDef *dev)
{
	hi2c_sentral = dev;
  sentral_device_info();
  HAL_Delay(500);
	sentral_feature_info();
	sentral_EEPROM_upload();
	sentral_device_info();
	sentral_config_operating_mode();
	sentral_read_status();
}
