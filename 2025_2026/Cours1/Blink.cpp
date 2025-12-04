#include "daisy_seed.h"
#include "daisysp.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

// Create 2 oscillators
Oscillator osc;
Oscillator oscB;

// Create 1 filter
Svf filter;

// Create 1 envelope with attack and decay
AdEnv env;

// Our sequence step struct
struct sequenceStep {
    int note;
    bool modulation;
    int filter;
};

// A sequence with a set of variables for note, modulation and filter value per step
sequenceStep sequence[] = {
    {60, false, 56}, 
    {62, false, 80},
    {57, true, 20},
    {55, false, 40},
    {60, false, 55},
    {70, true, 110}};

// The size of our sequence
const int sequenceSize = 6; 

// The current position of the sequence
int sequencePosition = 0;

// This tells that our position just changed, in order to trig the envelope
bool didChangePosition = false;

// Our Audio callback
void MyCallback(AudioHandle::InputBuffer in,
                AudioHandle::OutputBuffer out,
                size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        // The oscillator's Process function synthesizes, and
        // returns the next sample.
        
        //The position has changed, so we trig the envelope
        //We don't forget the set the flag back at false !
        if (didChangePosition) {
            didChangePosition = false;
            env.Trigger();
        }

        //We process the envelope
        float amp = env.Process();

        //We retrieve the values of our current step
        int currentNote = sequence[sequencePosition].note;
        bool useModulation =  sequence[sequencePosition].modulation;
        int currentFilterPitch =  sequence[sequencePosition].filter;

        //We process the oscillator dedicated to the modulation
        float mod = oscB.Process();

        //Create a pitch variable
        //We store this as a float in order to get a more precise value in case of a modulation
        float pitch = currentNote;
        if (useModulation) {
            //If we use the modulation, we apply this to the current pitch
            pitch += mod*5;
        } 

        //Set the frequency of the oscillator
        osc.SetFreq(mtof(pitch));

        //Apply the amplitude from the envelope to the output of the oscillator
        float mixAmp = osc.Process() * amp;

        //Apply the filter
        filter.SetFreq(mtof(currentFilterPitch));
        filter.Process(mixAmp);
        float outMix = filter.Low(); //We get the Low pass value

        out[0][i] = outMix; 
        out[1][i] = out[0][i]; //Copy left channel into right channel
    }
}


int main(void)
{
    // Declare a variable to store the state we want to set for the LED.
    bool led_state;
    led_state = true;

    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hardware.Configure();
    hardware.Init();

    //Get the samplerate from the hardware
    double sampleRate = hardware.AudioSampleRate();

    //Initialize our oscillators
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_SQUARE);

    oscB.Init(sampleRate);
    oscB.SetFreq(2);

    //Initialize our filter
    filter.Init(sampleRate);
    filter.SetFreq(800);
    filter.SetRes(0.5);

    //Initialize our envelope
    env.Init(sampleRate);
    env.SetTime(ADENV_SEG_ATTACK, 0.002); //Attack time = 2ms
    env.SetTime(ADENV_SEG_DECAY, 0.3); //Decay time = 300ms

    //Start the audio
    hardware.StartAudio(MyCallback);

    didChangePosition = true;

    // Loop forever
    for(;;)
    {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 300ms
        System::Delay(300);

        // Change sequence position
        didChangePosition = true;
        
        sequencePosition = (sequencePosition + 1) % sequenceSize;
        
    }
}
