
//Hardware Connections (Breakoutboard to Arduino):

// #Environmental Combo
//3.3V to 3.3V pin
//GND to GND pin
//SDA to A4
//SCL to A5
// #LPG Sensor MQ6
//VCC 5V
//GND GND
//AD A3
//D0 D6
// #Alc Sensor MQ3
//VCC 5V
//GND GND
//AD A2
//D0 D7
// Grove HCHO AT A0
//SIG A0
//NC A1
// Grove Multi Channel
// I2C Same as Environmental Combo
// VCC 5V


#include <math.h>
#include <Wire.h>
#include "SparkFunCCS811.h" //Click here to get the library: http://librarymanager/All#SparkFun_CCS811
#include "SparkFunBME280.h" //Click here to get the library: http://librarymanager/All#SparkFun_BME280
#include "MutichannelGasSensor.h"

#define CCS811_ADDR 0x5B //Default I2C Address
//#define CCS811_ADDR 0x5A //Alternative I2C Address

#define PIN_NOT_WAKE 5
#define data_vars 12
#define no_return -1

class MQ3{
  // Reference: https://www.dfrobot.com/blog-903.html
  public:
  MQ3(int error_value){
    this->is_init = false;
    this->pin = error_value;
    this->R0 = error_value;
  }

  bool isInit(){
    return this->is_init;
  }
  bool calibrate(){
    float sensor_value = 0;
    for(int i=0; i<100; i++){
      sensor_value = sensor_value + (float)analogRead(this->pin);
    }
    sensor_value = sensor_value/100.0;
    float volt = sensor_value/1024*5.0;
    float RS = (5.0-volt)/volt;
    this->R0 = RS/60.0;
    return true;
  }
  void init(int pin_f){
    delay(20);
    this->pin = pin_f;
//    float sensor_value = 0;
//    for(int i=0; i<100; i++){
//      sensor_value = sensor_value + (float)analogRead(this->pin);
//    }
//    sensor_value = sensor_value/100.0;
//    float volt = sensor_value/1024*5.0;
//    float RS = (5.0-volt)/volt;
//    this->R0 = RS/60.0;
    this->calibrate();
    this->is_init = true;
  }

  int get_alcohol_raw(){
  //  Assume the mod analog input is at A1
    return analogRead(this->pin);
  }
  float get_alcohol_volt(){
    return (float)analogRead(this->pin)*1024/5.0;
  }
  float get_alcohol_ppm(){
  //  Assume the mod analog input is at A1
    float volt = (float)analogRead(this->pin)/1024*5.0;
    float RS = (5.0-volt)/volt;
    float ratio = RS/this->R0;
    float bac = 0.189*ratio*ratio - 8.6178/10 + 1.0792;
    float ppm = bac*0.0001;
    return ppm;
  }

  private:
  bool is_init;
  int pin;
  float R0;  
};

class MQ6{// Reference: https://www.dfrobot.com/blog-896.html
  public:
  MQ6(int error_value){
    this->is_init = false;
    this->pin = error_value;
    this->R0 = error_value;
  }
  bool calibrate(){
    if(!this->is_init) return this->is_init;
    float sensor_value = 0;
    for(int i=0; i<100; i++){
      sensor_value = sensor_value+(float)analogRead(this->pin);
    }
    float sensor_volt = 0;
    float RS = 0;
    sensor_value = sensor_value/100.0;
    sensor_volt = sensor_value/1024*5.0;
    RS = (5.0-sensor_volt)/sensor_volt;
    this->R0 = RS/10.0;
    return true;
  }
  void init(int pin_f){
    this->pin = pin_f;
    this->is_init = true;
    this->calibrate();
  }
  bool isInit(){
    return this->is_init;
  }
  int get_lpg_raw(){
    if(!this->is_init) return this->R0;
    return analogRead(this->pin);
  }
  float get_lpg_volt(){
    if(!this->is_init) return this->R0;
    return (float)analogRead(this->pin)*1024/5.0;
  }
  float get_lpg_ppm(){
    if(!this->is_init) return this->R0;
    float sensor_volt = (float)analogRead(this->pin)/1024*5.0;
    float RS = (5.0-sensor_volt)/sensor_volt;
    float ratio = RS/this->R0;
    float ppm = (-3000)*ratio+3100;
    return ppm;
  }

  private:
  bool is_init;
  int pin;
  float R0;
};

class Grove_HCHO{// Reference: http://wiki.seeedstudio.com/Grove-HCHO_Sensor/
  public:
  Grove_HCHO(int error_value){
    this->is_init = false;
    this->pin = error_value;
    this->R0 = error_value;
  }
  bool calibrate(){
    if(!this->is_init) return this->is_init;
    float sensor_value = 0;
    for(int i=0; i<100; i++){
      sensor_value = sensor_value+(float)analogRead(this->pin);
    }
    sensor_value = sensor_value/100.0;
    this->R0 = (1023.0/sensor_value)-1;
    return true;
  }
  void init(int pin_f){
    this->pin = pin_f;
    this->is_init = true;
    this->calibrate();
  }
  bool isInit(){
    return this->is_init;
  }
  int get_hcho_raw(){
    if(!this->is_init) return this->R0;
    return analogRead(this->pin);
  }
  float get_hcho_volt(){
    if(!this->is_init) return this->R0;
    return (float)analogRead(this->pin)*1024/5.0;
  }
  float get_hcho_ppm(){
    if(!this->is_init) return this->R0;
    float RS = (1023.0/(float)analogRead(this->pin))-1;
    float ppm = pow(10.0, ((log10(RS/this->R0)-0.0827)/(-0.4807)));
    return ppm;
  }

  private:
  bool is_init;
  int pin;
  float R0;
};

CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;
MQ3 myMQ3 = new MQ3(no_return);
MQ6 myMQ6 = new MQ6(no_return);
Grove_HCHO myGroveHCHO = new Grove_HCHO(no_return);

int last_co2 = no_return;
int last_tvoc = no_return;

int gen_number(){
  return random(100);
}

int* gen_array(int len){
  int *arr = (int *)malloc(len * 4);
  for(int i=0; i<len; i++){
    arr[i] = gen_number();
  }
  return arr;
}

float get_temp_c(){
  return myBME280.readTempC();
}

float get_temp_f(){
  return myBME280.readTempF();
}

float get_pressure_pa(){
  return myBME280.readFloatPressure();
}

float get_pressure_inhg(){
  return myBME280.readFloatPressure() * 0.0002953;
}

float get_altitude_m(){
  return myBME280.readFloatAltitudeMeters();
}

float get_altitude_ft(){
  return myBME280.readFloatAltitudeFeet();
}

float get_humidity_rh(){
  return myBME280.readFloatHumidity();
}

int get_co2_ppm(){
//  myCCS811.setEnvironmentalData(myBME280.readFloatHumidity(), myBME280.readTempC());
  if(myCCS811.dataAvailable()){
    myCCS811.readAlgorithmResults();
    last_co2 = myCCS811.getCO2();
//    return myCCS811.getCO2(); // In ppm
  }
  return last_co2;
}

int get_tvoc_ppb(){
//  myCCS811.setEnvironmentalData(myBME280.readFloatHumidity(), myBME280.readTempC());
  if(myCCS811.dataAvailable()){
    myCCS811.readAlgorithmResults();
    last_tvoc = myCCS811.getTVOC();
//    return myCCS811.getTVOC(); // IN ppb
  }
  return last_tvoc;
}

int get_alcohol_raw(){
  //  Assume the mod analog input is at A2
  return analogRead(A2);
}

int get_lpg_raw(){
//  Assume the mod analog input is at A3
  return analogRead(A3);
}

int get_hcho_raw(){
//  Assume the mod analog input is at A0
  return analogRead(A0);
}

float get_nh3_ppm(){// Really Dectect
  float c = gas.measure_NH3();
  if(c >= 0)return c;
  else return no_return;
}

float get_co_ppm(){// Really Dectect
  float c = gas.measure_CO();
  if(c >= 0) return c;
  else return no_return;
}

float get_no2_ppm(){// Really Dectect
  float c = gas.measure_NO2();
  if(c >= 0) return c;
  else return no_return;
}

float get_c3h8_ppm(){
  float c = gas.measure_C3H8();
  if(c >= 0) return c;
  else return no_return;
}

float get_c4h10_ppm(){
  float c = gas.measure_C4H10();
  if(c >= 0)return c;
  else return no_return;
}

float get_ch4_ppm(){
  float c = gas.measure_CH4();
  if(c >= 0) return c;
  else return no_return;
}

float get_h2_ppm(){
  float c = gas.measure_H2();
  if(c >= 0) return c;
  else return no_return;
}

float get_c2h5oh_ppm(){
  float c = gas.measure_C2H5OH();
  if(c >= 0) return c;
  else return no_return;
}

float* form_array_all(){
  float *arr = (float *)malloc(4 * data_vars);
  
  arr[0] = (float)get_co2_ppm();
  arr[1] = (float)get_tvoc_ppb();
  arr[2] = get_co_ppm();
  arr[3] = get_no2_ppm();
  arr[4] = get_nh3_ppm();
  arr[5] = (float)myMQ3.get_alcohol_raw();
  arr[6] = (float)myGroveHCHO.get_hcho_raw();
  arr[7] = (float)myMQ6.get_lpg_raw();
  arr[8] = get_temp_c();
  arr[9] = get_pressure_pa();
  arr[10] = get_altitude_m();
  arr[11] = get_humidity_rh();

  return arr;
}

float* form_array_gas(){
  float *arr = (float *)malloc(16);
  
  arr[0] = (float)get_co2_ppm();
  arr[1] = (float)get_tvoc_ppb();
  arr[2] = (float)get_alcohol_raw();
  arr[3] = (float)get_lpg_raw();

  return arr;
}

float* form_array_env(){
  float *arr = (float *)malloc(16);
  
  arr[0] = get_temp_c();
  arr[1] = get_pressure_pa();
  arr[2] = get_altitude_m();
  arr[3] = get_humidity_rh();

  return arr;
}

void print_array(int *array_p, int len){
  for(int i=0; i<len-1; i++){
    Serial.print(array_p[i]);
    Serial.print(",");
  }
  Serial.print(array_p[len-1]);
}

void print_array_float(float *array_p, int len){
  for(int i=0; i<len-1; i++){
    Serial.print(array_p[i]);
    Serial.print(",");
  }
  Serial.print(array_p[len-1]);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  Serial.println("Test Generate Data");
  Wire.begin();
  
// Initialize CCS811
  CCS811Core::status returnCode = myCCS811.begin();
  if(returnCode != CCS811Core::SENSOR_SUCCESS){
    Serial.println("CCS811.begin() return an Error");
    while(1);
  }

// Enable the following and disable SPI Section
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x77;
  
// Initialize BME280
  myBME280.settings.commInterface = I2C_MODE;
  myBME280.settings.I2CAddress = 0x77;
  myBME280.settings.runMode = 3;
  myBME280.settings.tStandby = 0;
  myBME280.settings.filter = 4;
  myBME280.settings.tempOverSample = 5;
  myBME280.settings.pressOverSample = 5;
  myBME280.settings.humidOverSample = 5;
  
// BME280 need 2ms to turn on
  delay(10);
  myBME280.begin();
  
// Grove - Multichannel Gas Sensor Turn on
  gas.begin(0x04);//the default I2C address of the slave is 0x04
  gas.powerOn();

// MQ3 init
  myMQ3.init(A2);
//  MQ6 Init
  myMQ6.init(A3);
//  Grove HCHO Sensor Init
  myGroveHCHO.init(A0);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<100; i++){
//    int* a = gen_array(5);
    float *a = form_array_all();
    print_array_float(a, data_vars);
    free(a);
//    float t = myMQ3.get_alcohol_ppm();
//    Serial.print(t);
    Serial.print("\n");
  }
}
