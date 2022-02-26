#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

Svf filt_l;
Svf filt_r;

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

  scale_l = 2. * (pow10f(emphasis / 20) - 1);
  scale_r = 2. * (pow10f(-emphasis / 20) - 1);

  filt_l.SetFreq(cutoff);
  filt_r.SetFreq(cutoff);  
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

      filt_l.Process(sig_l);
      filt_r.Process(sig_r);

      out[0][i] = sig_l + scale_l * filt_l.Band();
      out[1][i] = sig_r + scale_r * filt_r.Band();
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
  filt_l.SetFreq(cutoff);
  filt_l.SetRes(0.0f);
  filt_l.SetDrive(2.5f);

  filt_r.Init(bluemchen.AudioSampleRate());
  filt_r.SetFreq(cutoff);
  filt_r.SetRes(0.0f);
  filt_r.SetDrive(2.5f);

  bluemchen.StartAudio(AudioCallback);

  while (1)
    {
      UpdateControls();
      UpdateOled();
    }

}
