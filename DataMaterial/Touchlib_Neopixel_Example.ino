
#include <TouchLib.h>


/*
 * Code generated by TouchLib SemiAutoTuning.
 *
 * Hardware configuration:
 *   sensor 0: type: capacitive (CVD method), analog pin A0
 *   sensor 1: type: capacitive (CVD method), analog pin A5
 *   sensor 2: type: resistive (analogRead() method),  analog pin A0, ground pin: 2
 */

/*
 *  Number of sensors. For capacitive sensors: needs to be a minimum of 2. When
 * using only one sensor, set N_SENSORS to 2 and use an unused analog input pin for the second
 * sensor. For 2 or more sensors you don't need to add an unused analog input.
 */
#define N_SENSORS                       3

/*
 * Number of measurements per sensor to take in one cycle. More measurements
 * means more noise reduction / spreading, but is also slower.
 */
#define N_MEASUREMENTS_PER_SENSOR       100

/* tlSensors is the actual object that contains all the sensors */
TLSensors<N_SENSORS, N_MEASUREMENTS_PER_SENSOR> tlSensors;
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
int RedNeo = 0; // colors for the neopixels
int BlueNeo = 0;
int GreenNeo = 0;
void setup()
{
        Serial.begin(9600);

        #if IS_ATMEGA32U4
        while(!Serial); /* Required for ATmega32u4 processors */
        #endif

        /* Delay to make sure serial monitor receives first message */
        delay(500);
        Serial.println();
        Serial.println();
        Serial.println("Switching baudrate to 115200. Make sure to adjust baudrate in serial monitor as well!");
        Serial.println();
        Serial.println();
        Serial.end();

        /*
         * Switch baudrate to highest baudrate available. With higher baudrate,
         * CPU has more time left to do capacitive sensing and thus get better
         * signal quality.
         */
        Serial.begin(115200);
        delay(500);
        Serial.println();
        Serial.println();

        /*
         * Configuration for sensor 0:
         * Type: capacitive (CVD method)
         * Analog pin: A0
         */
        tlSensors.initialize(0, TLSampleMethodCVD);
        tlSensors.data[0].tlStructSampleMethod.CVD.pin =               A0;
        tlSensors.data[0].releasedToApproachedThreshold =              39;
        tlSensors.data[0].approachedToReleasedThreshold =              36;
        tlSensors.data[0].approachedToPressedThreshold =               142;
        tlSensors.data[0].pressedToApproachedThreshold =               128;
        tlSensors.data[0].calibratedMaxDelta =                         679;
        tlSensors.data[0].enableSlewrateLimiter =                      true;

        /*
         * Configuration for sensor 1:
         * Type: capacitive (CVD method)
         * Analog pin: A5
         */
        tlSensors.initialize(1, TLSampleMethodCVD);
        tlSensors.data[1].tlStructSampleMethod.CVD.pin =               A5;
        tlSensors.data[1].releasedToApproachedThreshold =              356;
        tlSensors.data[1].approachedToReleasedThreshold =              321;
        tlSensors.data[1].approachedToPressedThreshold =               3560;
        tlSensors.data[1].pressedToApproachedThreshold =               3204;
        tlSensors.data[1].calibratedMaxDelta =                         3916;
        tlSensors.data[1].enableSlewrateLimiter =                      true;

        /*
         * Configuration for sensor 2:
         * Type: resistive (analogRead() method)
         * Analog pin: A0
         * Ground pin: 2
         */
        tlSensors.initialize(2, TLSampleMethodResistive);
        tlSensors.data[2].tlStructSampleMethod.resistive.pin =         A0;
        tlSensors.data[2].tlStructSampleMethod.resistive.gndPin =      2;
        tlSensors.data[2].releasedToApproachedThreshold =              29;
        tlSensors.data[2].approachedToReleasedThreshold =              26;
        tlSensors.data[2].approachedToPressedThreshold =               241;
        tlSensors.data[2].pressedToApproachedThreshold =               217;
        tlSensors.data[2].calibratedMaxDelta =                         750;
        tlSensors.data[2].enableSlewrateLimiter =                      false;

        if (tlSensors.error) {
                Serial.println("Error detected during initialization of TouchLib. This is "
                       "probably a bug; please notify the author.");
                while (1);
        }

        Serial.println("Calibrating sensors...");
        while(tlSensors.anyButtonIsCalibrating()) {
                tlSensors.sample();
        }
        Serial.println("Calibration done...");
}

void print_sensor_state(int n)
{
        char s[32] = {'\0'};

        Serial.print(" #");
        Serial.print(n);
        Serial.print(": ");
        Serial.print(tlSensors.isCalibrating(n));
        Serial.print(" ");
        Serial.print(tlSensors.isReleased(n));
        Serial.print(" ");
        Serial.print(tlSensors.isApproached(n));
        Serial.print(" ");
        Serial.print(tlSensors.isPressed(n));
        Serial.print(" ");
        Serial.print(tlSensors.getState(n));
        Serial.print(" ");
        Serial.print(tlSensors.getStateLabel(n));
        memset(s, '\0', sizeof(s));
        memset(s, ' ', 22 - strlen(tlSensors.getStateLabel(n)));
        Serial.print(s);
}

#define BAR_LENGTH                     58 /* <-- Change this to print longer or shorter visualizations */

void loop(void)
{
        int k;

        int n = 0; /* <-- Change this number to view a different sensor */

        tlSensors.sample(); /* <-- Take a series of new samples for all sensors */

        tlSensors.printBar(n, BAR_LENGTH); /* <-- Print the visualization */

        print_sensor_state(n); /* <-- Print summary of sensor n */

        /* For capacitive + resistive sensors: k is the sensor that sensor n is paired with */
        k = tlSensors.findSensorPair(n, (n + 1) % N_SENSORS);
        if (k > -1) {
                print_sensor_state(k); /* <-- Print summary of sensor k */
        }
        Serial.println("");

          pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if (tlSensors.getState(n) == 3) { //Released  Capacitive 0
             RedNeo = 150;
          }
    if (tlSensors.getState(n) == 5) { //Released  Capacitive 0
             GreenNeo = 255;
          }
    if (tlSensors.getState(n) == 8) { //Released  Capacitive 0
             BlueNeo = 255;
          }
    if (tlSensors.getState(k) == 3) { //Released  Capacitive 0
             RedNeo = 50;
          }
    if (tlSensors.getState(k) == 5) { //Released  Capacitive 0
             GreenNeo = 255;
          }
    if (tlSensors.getState(k) == 8) { //Released  Capacitive 0
             RedNeo = 255;
          }     
    pixels.setPixelColor(0, pixels.Color(RedNeo, GreenNeo, BlueNeo));

    pixels.show();   // Send the updated pixel colors to the hardware.
  
}
