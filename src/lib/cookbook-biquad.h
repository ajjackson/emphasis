/** Basic implementation of a biquad with RBJ cookbook coefficients

C++ is not my first language, wish me luck!

Loosely based on the DaisySP Biquad filter, but that one doesn't seem
to work for me.

**/
 
class CookbookBiquad
{
private:
  int magic;
  float two_pi_d_sr_;
  float frequency_, gain_dB_, resonance_;
  float a_[3], b_[3], x_[3], y_[3];
  bool param_changed_;

public:
  CookbookBiquad() {}
  ~CookbookBiquad() {}

  void Init(float sample_rate);

  /* Process one audio sample */
  float Process(float in);

  /* Recalculate biquad coefficients using current values of frequency, q etc. */
  void CalcCoefficients();

/* Set target frequency in Hz */
  inline void SetFrequency(float frequency) {
    frequency_ = frequency;
    param_changed_ = true;
  };

  /* Set target resonance */
  inline void SetResonance(float resonance) {
    resonance_ = resonance;
    param_changed_ = true;
  };

  /* Set band/shelf gain in dB */
  inline void SetGain(float gain_dB) {
    gain_dB_ = gain_dB;
    param_changed_ = true;
  };

};
