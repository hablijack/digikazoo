#include <Arduino.h>
#include <BLEMidi.h>
#include <approx_fft.h>

#define SAMPLES 512              // SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 24000 // Ts = Based on Nyquist, must be 2 times the highest expected frequency.
#define log2(x) log(x) / log(2)
#define THRESHOLD 0
#define SAMPLING_PERIOD round(1000000 * (1.0 / SAMPLING_FREQUENCY)) // Period in microseconds
#define MIDI_CHANNEL 0

unsigned long microSeconds;
unsigned long power;
unsigned int old_midi_note = 0;
unsigned int midi_note;
float peak;

int vReal[SAMPLES]; // create vector of size SAMPLES to hold real values
String notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

void setup()
{
  Serial.begin(115200); // Baud rate for the Serial Monitor
  BLEMidiServer.begin("Digikazoo");
  pinMode(A0, INPUT);
}

void loop()
{
  power = 0;
  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++)
  {
    microSeconds = micros();  // Returns the number of microseconds since the Arduino board began running the current script.
    vReal[i] = analogRead(0); // Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    power += vReal[i];
    /*remaining wait time between samples if necessary*/
    while (micros() < (microSeconds + SAMPLING_PERIOD))
    {
      // do nothing
    }
  }

  if (power >= THRESHOLD)
  {
    peak = Approx_FFT(vReal, SAMPLES, SAMPLING_FREQUENCY);
    midi_note = round(12.0 * log2(peak / 440.0) + 69.0);
    if (midi_note != old_midi_note)
    {
      Serial.println("Note: " + notes[midi_note % 12] + ", Power: " + String(power));
      BLEMidiServer.noteOff(MIDI_CHANNEL, old_midi_note, 127);
      BLEMidiServer.noteOn(MIDI_CHANNEL, midi_note, 127);
      old_midi_note = midi_note;
    }
  }
  BLEMidiServer.noteOn(0, 69, 127);
  delay(1000);
  BLEMidiServer.noteOff(0, 69, 127); // Then we make a delay of one second before returning to the beginning of the loop
  delay(1000);
}
