#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include "cookbook-biquad.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

float frequency = 5000.;
float gain_dB = 0.;

Parameter knob1;
Parameter knob2;

CookbookBiquad filter;

void UpdateOled()
{
    bluemchen.display.Fill(false);

    // Display cutoff frequency
    std::string str = "Freq: ";
    char *cstr = &str[0];

    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);
    str = std::to_string(static_cast<int>(frequency));
    bluemchen.display.SetCursor(30, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "Gain:";
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    std::sprintf(cstr, FLT_FMT3, FLT_VAR3(gain_dB));

    bluemchen.display.SetCursor(30, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    std::sprintf(cstr, FLT_FMT3, FLT_VAR3(filter.Process(0.)));

    bluemchen.display.SetCursor(0, 12);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.Update();
}

void UpdateControls()
{
  bluemchen.ProcessAllControls();

  knob1.Process();
  knob2.Process();

  frequency = exp(fclamp(knob1.Value(), 2.f, 10.f));
  gain_dB = fclamp(knob2.Value(), -18.f, 18.f);

  filter.SetFrequency(frequency);
  filter.SetGain(gain_dB);
  filter.CalcCoefficients();
}

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
  for (size_t i = 0; i < size; i++)
    {
      float sig_l = in[1][i];
      float sig_r = in[0][i];

      out[0][i] = filter.Process(sig_l);
      out[1][i] = 0.;
    }
}

int main(void)
{

  bluemchen.Init();
  bluemchen.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);

  bluemchen.StartAdc();

  knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 2.f, 10.f, Parameter::LINEAR);
  knob2.Init(bluemchen.controls[bluemchen.CTRL_2], -18.f, 18.0f, Parameter::LINEAR);

  filter.Init(bluemchen.AudioSampleRate());
  filter.SetResonance(0.7);
  filter.SetFrequency(500.);
  filter.SetGain(0.);

  bluemchen.StartAudio(AudioCallback);

  while (1)
    {
      UpdateControls();
      UpdateOled();
    }

}
