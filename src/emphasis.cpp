#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include "cookbook-biquad.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

CookbookBiquad filt_l;
CookbookBiquad filt_r;

float cutoff = 5000.;
float emphasis = 0.;

float scale_l = 1.;
float scale_r = 1.;

Parameter knob1;
Parameter knob2;
Parameter cv1;
Parameter cv2;


void UpdateOled()
{
    bluemchen.display.Fill(false);

    // Display cutoff frequency
    std::string str = "Freq: ";
    char *cstr = &str[0];

    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);
    str = std::to_string(static_cast<int>(cutoff));
    bluemchen.display.SetCursor(30, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "Emph:";
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    std::sprintf(cstr, "%+1d.%02d", static_cast<int>(emphasis),
                 static_cast<int>(abs(fmod(emphasis, 1.f) * 100)));

    bluemchen.display.SetCursor(30, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.Update();
}

void UpdateControls()
{
  bluemchen.ProcessAllControls();

  knob1.Process();
  knob2.Process();
  cv1.Process();
  cv2.Process();

  // Offset frequency by exp(5) so that with 0 CV Freq control gives full range:
  // clamp to this range.
  cutoff = exp(fclamp(knob1.Value() + cv1.Value(), 2.f, 10.f));
  emphasis = fclamp(knob2.Value() + cv2.Value(), -18.f, 18.f);

  filt_l.SetFrequency(cutoff);
  filt_l.SetGain(emphasis);
  filt_l.CalcCoefficients();
  filt_r.SetFrequency(cutoff);
  filt_r.SetGain(-emphasis);
  filt_r.CalcCoefficients();
}

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size)
{
  UpdateControls();
  for (size_t i = 0; i < size; i++)
    {
      float sig_l = in[1][i];
      float sig_r = in[0][i];

      out[0][i] = filt_l.Process(sig_l);
      out[1][i] = filt_r.Process(sig_r);
    }
}

int main(void)
{

  bluemchen.Init();
  bluemchen.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);

  bluemchen.StartAdc();

  knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 2.f, 10.f, Parameter::LINEAR);
  knob2.Init(bluemchen.controls[bluemchen.CTRL_2], -18.f, 18.0f, Parameter::LINEAR);

  cv1.Init(bluemchen.controls[bluemchen.CTRL_3], -5.f, 5.f, Parameter::LINEAR);
  cv2.Init(bluemchen.controls[bluemchen.CTRL_4], -18.f, 18.f, Parameter::LINEAR);

  filt_l.Init(bluemchen.AudioSampleRate());
  filt_l.SetFrequency(cutoff);
  filt_l.SetResonance(0.8f);
  filt_l.SetGain(0.f);

  filt_r.Init(bluemchen.AudioSampleRate());
  filt_r.SetFrequency(cutoff);
  filt_r.SetResonance(0.8f);
  filt_r.SetGain(0.f);

  bluemchen.StartAudio(AudioCallback);

  while (1)
    {
      UpdateControls();
      UpdateOled();
    }

}
