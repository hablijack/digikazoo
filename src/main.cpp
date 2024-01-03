#include "arduinoFFT.h"

#define SAMPLES 256              // SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 24576 // Ts = Based on Nyquist, must be 2 times the highest expected frequency.
#define log2(x) log(x) / log(2)

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES]; // create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; // create vector of size SAMPLES to hold imaginary values

unsigned long power;

String notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

void setup()
{
  Serial.begin(115200);                                         // Baud rate for the Serial Monitor
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); // Period in microseconds
  pinMode(A0, INPUT);
}

void loop()
{
  power = 0;
  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++)
  {
    microSeconds = micros(); // Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(0); // Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0;             // Makes imaginary term 0 always

    power += vReal[i];

    /*remaining wait time between samples if necessary*/
    while (micros() < (microSeconds + samplingPeriod))
    {
      // do nothing
    }
  }

  unsigned long threshold = 0;

  /*Perform FFT on samples*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*Find peak frequency and print peak*/
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  int midi_num = round(12.0 * log2(peak / 440.0) + 69.0);

  if (power >= threshold)
  {
    Serial.println(notes[midi_num % 12]); // Print out the most dominant frequency.
    Serial.println(String(power) + ": power value.");
  }
  delay(500);
}
