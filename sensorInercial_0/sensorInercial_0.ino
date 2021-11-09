#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include <SD.h>
const int ledPin =  LED_BUILTIN;
const int chipSelect = 4;
File archivo;
LSM9DS1 imu;

#define PRINT_CALCULATED
//#define PRINT_RAW
#define PRINT_SPEED 20 // 250 ms between prints
static unsigned long lastPrint = 0; // Keep track of print time
unsigned long time1;
unsigned long time2=0;
unsigned long dTime;

// Earth's magnetic field varies by location. Add or subtract
// a declination to get a more accurate heading. Calculate
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -3.28 // Declination (degrees) in Cuenca Ecuador.
String dataLine;
int cont = 0;
int sample = 0;
#define FILE_BASE_NAME "Data"
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[] = FILE_BASE_NAME "000.csv";
bool inicio = true;
const int buzzPin = 8;
void setup() {
  Serial.begin(9600);
  pinMode(buzzPin, OUTPUT);
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(4))
  {
    Serial.println(F("Error al iniciar"));
    return;
  }
  Serial.println(F("Iniciado correctamente"));

  while (SD.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 2] != '9') {//entre 00 y 99
      fileName[BASE_NAME_SIZE + 2]++;
    } else if (fileName[BASE_NAME_SIZE+1] != '9') {
      fileName[BASE_NAME_SIZE + 2] = '0';
      fileName[BASE_NAME_SIZE + 1]++;

    }else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 2] = '0';
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;

    } else {
      Serial.println(F("Can't create file name"));
      return;
    }
  }

  // Abrir fichero y mostrar el resultado
  archivo = SD.open(fileName, FILE_WRITE);
  Serial.print("Archivo: ");
  Serial.println(fileName);
  if (archivo)
  {
    archivo.print("timeStamp");
    archivo.print(", ");
    archivo.print("sample");
    archivo.print(", ");
    archivo.print("ax(g)");
    archivo.print(", ");
    archivo.print("ay(g)");
    archivo.print(", ");
    archivo.print("az(g)");
    archivo.print(", ");
    archivo.print("gx(deg/s)");
    archivo.print(", ");
    archivo.print("gy(deg/s)");
    archivo.print(", ");
    archivo.println("gz(deg/s)");
    
    
    

  }
  else
  {
    Serial.println(F("Error al abrir el archivo"));
  }


  Wire.begin();

  if (imu.begin() == false) // with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
  {
    digitalWrite(buzzPin, HIGH);
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                   "work for an out of the box LSM9DS1 " \
                   "Breakout, but may need to be modified " \
                   "if the board jumpers are.");
    while (1);
  }


}

void loop() {
  // Update the sensor values whenever new data is available
  if (inicio) {
    for (int sound = 0; sound < 3; sound ++) {
      digitalWrite(buzzPin, HIGH);
      delay(400);
      digitalWrite(buzzPin, LOW);
      delay(400);
    }
    digitalWrite(buzzPin, LOW);
    inicio = false;
    Serial.println("inicio  grabado");
  }

  //Serial.print("milis 1: ");
  //Serial.println(millis());
  time1 = millis();
  //Serial.println(time1);

  if ( imu.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    imu.readGyro();
  }
  if ( imu.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    imu.readAccel();
  }


  //if ((lastPrint + PRINT_SPEED) < millis())//cada 20 ms
  //{



  archivo.print(time2);
  archivo.print(", ");
  archivo.print(sample);
  archivo.print(", ");
  printAccel(); // Print "A: ax, ay, az"
  archivo.print(", ");
  printGyro();  // Print "G: gx, gy, gz"
  archivo.println();
  sample++;

  


  if (cont > 400) {
    archivo.close();


    archivo = SD.open(fileName, FILE_WRITE);
    cont = 0;
    digitalWrite(buzzPin, HIGH);
    delay(2000);
    digitalWrite(buzzPin, LOW);
    
  }
  cont++;
  time2 = millis();
  dTime=time2-time1;
  if (dTime<20){
    Serial.println(dTime);
    delay(20-dTime);
  }
  //}


}

void printGyro()
{
  // Now we can use the gx, gy, and gz variables as we please.
  // Either print them as raw ADC values, or calculated in DPS.
  //Serial.print("G: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcGyro helper function to convert a raw ADC value to
  // DPS. Give the function the value that you want to convert.
  //Serial.print("G, ");
  //Serial.print(imu.calcGyro(imu.gx), 2);
  //Serial.print(", ");
  //Serial.print(imu.calcGyro(imu.gy), 2);
  //Serial.print(", ");
  //Serial.print(imu.calcGyro(imu.gz), 2);
  //Serial.println(" deg/s");

  archivo.print(imu.calcGyro(imu.gx), 4);
  archivo.print(", ");
  archivo.print(imu.calcGyro(imu.gy), 4);
  archivo.print(", ");
  archivo.print(imu.calcGyro(imu.gz), 4);
  //archivo.println(" deg/s");

#elif defined PRINT_RAW
  Serial.print(imu.gx);
  Serial.print(", ");
  Serial.print(imu.gy);
  Serial.print(", ");
  Serial.println(imu.gz);
#endif
}

void printAccel()
{
  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  //Serial.print("A: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
  //Serial.println("A");
  //Serial.print(imu.calcAccel(imu.ax), 2);
  //Serial.print(", ");
  //Serial.print(imu.calcAccel(imu.ay), 2);
  //Serial.print(", ");
  //Serial.print(imu.calcAccel(imu.az), 2);
  //Serial.println(" g");

  archivo.print(imu.calcAccel(imu.ax), 4);
  archivo.print(", ");
  archivo.print(imu.calcAccel(imu.ay), 4);
  archivo.print(", ");
  archivo.print(imu.calcAccel(imu.az), 4);
  //archivo.println(" g");

#elif defined PRINT_RAW
  Serial.print(imu.ax);
  Serial.print(", ");
  Serial.print(imu.ay);
  Serial.print(", ");
  Serial.println(imu.az);
#endif

}

void printMag()
{
  // Now we can use the mx, my, and mz variables as we please.
  // Either print them as raw ADC values, or calculated in Gauss.
  Serial.print("M: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcMag helper function to convert a raw ADC value to
  // Gauss. Give the function the value that you want to convert.
  Serial.print(imu.calcMag(imu.mx), 2);
  Serial.print(", ");
  Serial.print(imu.calcMag(imu.my), 2);
  Serial.print(", ");
  Serial.print(imu.calcMag(imu.mz), 2);
  Serial.println(" gauss");

  archivo.print(imu.calcMag(imu.mx), 2);
  archivo.print(", ");
  archivo.print(imu.calcMag(imu.my), 2);
  archivo.print(", ");
  archivo.print(imu.calcMag(imu.mz), 2);
  archivo.println(" gauss");

#elif defined PRINT_RAW
  Serial.print(imu.mx);
  Serial.print(", ");
  Serial.print(imu.my);
  Serial.print(", ");
  Serial.println(imu.mz);
#endif
}

// Calculate pitch, roll, and heading.
// Pitch/roll calculations take from this app note:
// https://web.archive.org/web/20190824101042/http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf
// Heading calculations taken from this app note:
// https://web.archive.org/web/20150513214706/http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));

  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);

  heading -= DECLINATION * PI / 180;

  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);

  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  Serial.print("Pitch, Roll: ");
  Serial.print(pitch, 2);
  Serial.print(", ");
  Serial.println(roll, 2);
  Serial.print("Heading: "); Serial.println(heading, 2);
}
