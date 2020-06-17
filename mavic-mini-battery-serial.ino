/*
thanks to:
PowerCartel for smart battery routines - https://github.com/PowerCartel/PackProbe
Bodmer for fast TFT library - https://github.com/Bodmer/TFT_ST7735
Alain Aeropic - for BatMan inspiration - https://www.thingiverse.com/thing:4235767 

This is a modification for read a battery info in serial port
*/

#define VERSION   "v1.1"

#include <Wire.h>
byte deviceAddress = 11;

// Standard and common non-standard Smart Battery commands
#define BATTERY_MODE             0x03
#define TEMPERATURE              0x08
#define VOLTAGE                  0x09
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define REMAINING_CAPACITY       0x0F
#define FULL_CHARGE_CAPACITY     0x10
#define TIME_TO_FULL             0x13
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define CYCLE_COUNT              0x17
#define DESIGN_CAPACITY          0x18
#define DESIGN_VOLTAGE           0x19
#define SPEC_INFO                0x1A
#define MFG_DATE                 0x1B
#define SERIAL_NUM               0x1C
#define MFG_NAME                 0x20   // String
#define DEV_NAME                 0x21   // String
#define CELL_CHEM                0x22   // String
#define MFG_DATA                 0x23   // String
#define CELL4_VOLTAGE            0x3C
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F
#define STATE_OF_HEALTH          0x4F
#define DJI_SERIAL               0xD8  // String

#define bufferLen 32
uint8_t i2cBuffer[bufferLen];


#define TFT_BACKGROUND  0xD6BB

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(100000);

  delay(2000);
}

uint8_t read_byte()
{
  while (1)
  {
    if (Wire.available())
    {
      return Wire.read();
    }
  }
}

void loop()
{
  uint8_t length_read = 0;
  

  Serial.print("Manufacturer Name: ");
  length_read = i2c_smbus_read_block(MFG_NAME, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Device Name: ");
  length_read = i2c_smbus_read_block(DEV_NAME, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Chemistry ");
  length_read = i2c_smbus_read_block(CELL_CHEM, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Manufacturer Data: ");
  length_read = i2c_smbus_read_block(MFG_DATA, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Design Capacity: " );
  Serial.println(fetchWord(DESIGN_CAPACITY));

  Serial.print("Design Voltage: " );
  Serial.println(fetchWord(DESIGN_VOLTAGE));

  Serial.print("Manufacture Date (D.M.Y): " );

  String formatted_date;
  int mdate = fetchWord(MFG_DATE);
  int mday = B00011111 & mdate;
  int mmonth = mdate >> 5 & B00001111;
  int myear = 1980 + (mdate >> 9 & B01111111);
  formatted_date += mday;
  formatted_date += ".";
  formatted_date += mmonth;
  formatted_date += ".";
  formatted_date += myear;
  Serial.println(formatted_date);
  int str_len = formatted_date.length() + 1; 
  char mfg_date[str_len];
  formatted_date.toCharArray(mfg_date, str_len);


  Serial.print("Serial Number: ");
  length_read = i2c_smbus_read_block(DJI_SERIAL, i2cBuffer, bufferLen);
  char *djiserial = i2cBuffer;
  Serial.println(djiserial); 

  Serial.print("Specification Info: ");
  Serial.println(fetchWord(SPEC_INFO));

  Serial.print("Cycle Count: " );
  uint8_t cycles = fetchWord(CYCLE_COUNT);
  Serial.println(cycles);

  Serial.print("Voltage: ");
  float voltage = (float)fetchWord(VOLTAGE) / 1000;
  Serial.println(voltage);

  Serial.print("Full Charge Capacity: " );
  Serial.println(fetchWord(FULL_CHARGE_CAPACITY));

  Serial.print("Remaining Capacity: " );
  Serial.println(fetchWord(REMAINING_CAPACITY));

  Serial.print("Relative Charge(%): ");
  uint8_t charge = fetchWord(RELATIVE_SOC);
  Serial.println(charge);

  Serial.print("Absolute Charge(%): ");
  Serial.println(fetchWord(ABSOLUTE_SOC));

  Serial.print("Minutes remaining for full charge: ");
  Serial.println(fetchWord(TIME_TO_FULL));

  Serial.print("Cell 1 Voltage: ");
  float cell1 = (float)fetchWord(CELL1_VOLTAGE)/1000;
  Serial.println(cell1);
  Serial.print("Cell 2 Voltage: ");
  float cell2 = (float)fetchWord(CELL2_VOLTAGE)/1000;
  Serial.println(cell2);
  char buffer[5];
  String v = dtostrf(voltage, 4, 2, buffer);
  String c1 = dtostrf(cell1, 4, 2, buffer);
  String c2 = dtostrf(cell2, 4, 2, buffer);
  String cells = v + " " + c1 + '/' + c2;
  str_len = cells.length() + 1; 
  char cellsV[str_len];
  cells.toCharArray(cellsV, str_len);


  Serial.print("State of Health: ");
  Serial.println(fetchWord(STATE_OF_HEALTH));

  Serial.print("Battery Mode (BIN): 0b");
  Serial.println(fetchWord(BATTERY_MODE), BIN);

  Serial.print("Battery Status (BIN): 0b");
  Serial.println(fetchWord(BATTERY_STATUS), BIN);

  Serial.print("Charging Current: ");
  Serial.println(fetchWord(CHARGING_CURRENT));

  Serial.print("Charging Voltage: ");
  Serial.println(fetchWord(CHARGING_VOLTAGE));

  Serial.print("Temp: ");
  unsigned int tempk = fetchWord(TEMPERATURE);
  float temp = tempk / 10.0 - 273.15;
  Serial.println(temp);

  Serial.print("Current (mA): " );
  Serial.println(fetchWord(CURRENT));

  Serial.println(".");

  delay(1000);
}
