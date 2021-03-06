#include "cookbook-biquad.h"
#include <math.h>
#include "Utility/dsp.h"

using namespace daisysp;

/** Basic implementation of a biquad with RBJ cookbook coefficients

C++ is not my first language, wish me luck!

Loosely based on the DaisySP Biquad filter, but that one doesn't seem
to work for me.

**/

const char* CookbookBiquad::mode_text[3] = { "PEAK", "LOW-SHELF" , "HIGH-SHELF" };

void CookbookBiquad::Init(float sample_rate) {
  mode_ = 0;
  param_changed_ = true;
  two_pi_d_sr_ = 2. * PI_F / sample_rate;

  /* No need to initialise x0, y0 as these are set by Process */
  for (int i = 1; i < 3; i++) {
    x_[i] = y_[i] = 0.f;
  }

  resonance_ = 0.7;
  frequency_ = 500.;
  gain_dB_ = 0.;

  CalcCoefficients();
}

/* Process one audio sample */
float CookbookBiquad::Process(float in) {
  // Direct form I

  // Shift delay taps
  y_[2] = y_[1];
  y_[1] = y_[0];
  x_[2] = x_[1];
  x_[1] = x_[0];

  // Calculate new output
  x_[0] = in;

  y_[0] = b_[0] * x_[0];
  for (int i = 1; i < 3; i++) {y_[0] += b_[i] * x_[i] - a_[i] * y_[i];}
  y_[0] /= a_[0];

  return y_[0];
}

/* Recalculate biquad coefficients using current values of frequency, q etc. */
void CookbookBiquad::CalcCoefficients() {
  if ( param_changed_ ) {
    // RBJ Cookbook recipe for peaking EQ filter
    float two_alpha_sqrt_amp;
    float amp = pow10f(gain_dB_ / 40.f);
    float omega0 = frequency_ * two_pi_d_sr_;
    float cos_omega0 = cosf(omega0);
    float alpha = sinf(omega0) / (2 * resonance_);

    switch (mode_) {
    case peak :
      b_[0] = 1 + alpha * amp;
      b_[1] = -2 * cos_omega0;
      b_[2] = 2 - b_[0]; // = 1 - alpha . A
      a_[0] = 1 + alpha / amp;
      a_[1] = b_[1];
      a_[2] = 2 - a_[0]; // = 1 - alpha / A
      break;

    case lowShelf :
      two_alpha_sqrt_amp = 2. * fastroot(amp, 2) * alpha;
      b_[0] = amp * ((amp + 1.) - (amp - 1.) * cos_omega0 + two_alpha_sqrt_amp);
      b_[1] = 2 * amp * ((amp - 1) - (amp + 1) * cos_omega0);
      b_[2] = b_[0] - amp * 2 * two_alpha_sqrt_amp;
         // = amp * ((amp + 1) - (amp - 1) * cos_omega0 - two_alpha_sqrt_amp);
      a_[0] = (amp + 1) + (amp - 1) * cos_omega0 + two_alpha_sqrt_amp;
      a_[1] = -2 * ((amp - 1) + (amp + 1) * cos_omega0);
      a_[2] = a_[0] - 2 * two_alpha_sqrt_amp;
      break;

  case highShelf :
      two_alpha_sqrt_amp = 2. * fastroot(amp, 2) * alpha;
      b_[0] = amp * ((amp + 1.) + (amp - 1.) * cos_omega0 + two_alpha_sqrt_amp);
      b_[1] = -2 * amp * ((amp - 1) + (amp + 1) * cos_omega0);    
      b_[2] = b_[0] - amp * 2 * two_alpha_sqrt_amp;
      a_[0] = (amp + 1) - (amp - 1) * cos_omega0 + two_alpha_sqrt_amp;
      a_[1] = 2 * ((amp - 1) - (amp + 1) * cos_omega0);
      a_[2] = a_[0] - 2 * two_alpha_sqrt_amp;
      break;

    }


    param_changed_ = false;
  }
}
