#include "daisy_seed.h"
#include "daisysp.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

Oscillator osc;
Oscillator oscB;

Svf filter;

AdEnv env;

struct sequenceStep {
    int note;
    bool modulation;
    int filter;
};

sequenceStep sequence[] = {
    {60, false, 56}, 
    {62, false, 80},
    {57, true, 20},
    {55, false, 40},
    {60, false, 55},
    {70, true, 110}};

const int arraySize = 6; 
int sequencePosition = 0;
bool didChangePosition = false;

void MyCallback(AudioHandle::InputBuffer in,
                AudioHandle::OutputBuffer out,
                size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        // The oscillator's Process function synthesizes, and
        // returns the next sample.
        
        if (didChangePosition) {
            didChangePosition = false;
            env.Trigger();
        }

        float amp = env.Process();

        int currentNote = sequence[sequencePosition].note;
        bool useModulation =  sequence[sequencePosition].modulation;
        int currentFilterPitch =  sequence[sequencePosition].filter;

        float mod = oscB.Process();

        float pitch = currentNote;
        if (useModulation) {
            pitch += mod*5;
        } 

        osc.SetFreq(mtof(pitch));

        float mixAmp = osc.Process() * amp;

        filter.SetFreq(mtof(currentFilterPitch));
        filter.Process(mixAmp);

        float outMix = filter.Low();

        out[0][i] = outMix;
        out[1][i] = out[0][i];
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

    double sampleRate = hardware.AudioSampleRate();

    osc.Init(sampleRate);
    osc.SetFreq(mtof(60));
    osc.SetWaveform(Oscillator::WAVE_SQUARE);

    oscB.Init(sampleRate);
    oscB.SetFreq(2);

    filter.Init(sampleRate);
    filter.SetFreq(800);
    filter.SetRes(0.5);

    env.Init(sampleRate);
    env.SetTime(ADENV_SEG_ATTACK, 0.002);
    env.SetTime(ADENV_SEG_DECAY, 0.3);
    //env.SetTime()

    hardware.StartAudio(MyCallback);

    didChangePosition = true;

    // Loop forever
    for(;;)
    {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        System::Delay(300);

        didChangePosition = true;
        
        sequencePosition = (sequencePosition + 1) %  arraySize;
        
    }
}
