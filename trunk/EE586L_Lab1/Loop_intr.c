// Ring modulator lab template.

// Must include for sine sinf(x) function.
#include <math.h>

// Must include for DSK functionality.
#include "dsk6713_aic23.h"
#include "dsk6713_led.h"
#include "dsk6713_dip.h"

Uint32 fs=DSK6713_AIC23_FREQ_44KHZ;		// Set codec sampling rate to 44100.
#define DSK6713_AIC32_INPUT_LINE 0x0011
Uint16 inputsource=DSK6713_AIC32_INPUT_LINE;

// Defines **************************************************.
// Add your own... Frequency of LED blinking, Carrier waveform (sine), Length of sine-table, etc.
// For example: #define SAMPLE_RATE 44100				// Sample rate of AIC23 codec.
// #define ...
#define LED_BLINK_FREQ    2
#define SAMPLING_FREQ     44100
#define SINE_TABLE_LENGTH 25000

#define STEP_2 (float)(2 * SINE_TABLE_LENGTH)/SAMPLING_FREQ
#define STEP_100 (float)(100 * SINE_TABLE_LENGTH)/SAMPLING_FREQ

float PI = 3.14159; 

// Sine lookup table ****************************************. 
// <<<<YOU MUST HAVE ONE>>>>
short sine_table[SINE_TABLE_LENGTH];
#define SINE_AMPLITUDE 10000
float loopindex_LED = 0.0;
float loopindex_100 = 0.0;
float loopindex_modulator = 0.0;
short i;

// Flags for program functionality **************************.
// 0 for off, >0 for on...
// For example: short g_input_line_in = 0;				// Line-in vs. Modulator source.
// etc.
#define USE_INPUT_LINE_IN 0
#define DOUBLE_MOD_FREQ 0
#define APPLY_MODULATION 0
#define APPLY_PHASE_SHIFT 0

// Interrupt service routine, called on audio codec sample 
// clock trigger.
// **********************************************************.
interrupt void c_int11()         		
{
   // The majority of your code should go in this function.

   // 32bit audio IO. 16 bits per channel, highword|lowword.
   int   in_sample;
   int   out_sample;
   int   output;
   short sample_data;   
   
   // Define more variables here. ***************************.
   short sine_LED_value;
   short sine_100_value;
   short modulator_value;

   // Get the state of the DIP switches. ********************.
   int dip_1_state = DSK6713_DIP_get(0);
   int dip_2_state = DSK6713_DIP_get(1);
   int dip_3_state = DSK6713_DIP_get(2);
   int dip_4_state = DSK6713_DIP_get(3);

   // Update your counters for the sine lookup table, LED. **.
   sine_LED_value = sine_table[(short)loopindex_LED];
   loopindex_LED += STEP_2;
   if (loopindex_LED > (float)SINE_TABLE_LENGTH )
      loopindex_LED -= (float)SINE_TABLE_LENGTH;

   // Handle the LED toggling. ******************************.
   // Question 1: LED blinking at LED_BLINK_FREQ Hz
   if (sine_LED_value >= 0)
      DSK6713_LED_off(0);
   else
      DSK6713_LED_on(0);
 
   // Do your processing and program functionality here. ****.
   sine_100_value = sine_table[(short)loopindex_100];
   loopindex_100 += STEP_100;
   if (loopindex_100 > (float)SINE_TABLE_LENGTH )
      loopindex_100 -= (float)SINE_TABLE_LENGTH;

   // Question 3: 1x or 2x modulation frequency
   modulator_value = sine_table[(short)loopindex_modulator];
   if (dip_2_state == DOUBLE_MOD_FREQ)
      loopindex_modulator += 2*STEP_100;
   else
      loopindex_modulator += STEP_100;
   if (loopindex_modulator > (float)SINE_TABLE_LENGTH )
      loopindex_modulator -= (float)SINE_TABLE_LENGTH;
   
   // Question 2: Line in or Sine Wave?
   if (dip_1_state == USE_INPUT_LINE_IN) {
      in_sample   = input_sample();
      sample_data =  (0xFFFF & in_sample);
   } else {
      sample_data = sine_100_value;
   }

   // Question 4: Apply modulation?
   if (dip_3_state == APPLY_MODULATION)
      output = sample_data * modulator_value / SINE_AMPLITUDE;
   else
      output = sample_data;

   // Question 5: 180 degree phase shift of one channel
   if (dip_4_state == APPLY_PHASE_SHIFT) {
      out_sample = (0x0000FFFF & -1*output) | 
    	           (0xFFFF0000 & (output << 16));
   } else {
      out_sample = (0x0000FFFF & output) | 
    	           (0xFFFF0000 & (output << 16));
   }

   output_sample(out_sample);

   return;
}

void gen_sine_table()
{
	// Generate your sine lookup table here. ****************.
	// Be aware of the data type you use to hold the data.
	// Get sine data using the sinf(x) function, x is in radians.
	for (i=0; i < SINE_TABLE_LENGTH; i++) 
		sine_table[i] = SINE_AMPLITUDE*sin(2*PI*i/SINE_TABLE_LENGTH);
	i=0;
}

void main()
{
  // Generate sine lookup table.
  gen_sine_table();

  // Init DSK, codec, McBSP.
  comm_intr();                   

  // Init DSK functionality. ********************************.
  // Call the init functions for the LEDs and DIPs 
  DSK6713_LED_init();
  DSK6713_DIP_init();
  while(1); // Inf. loop. Interrupt-driven at this point.
}
