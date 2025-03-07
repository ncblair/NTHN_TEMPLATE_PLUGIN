#include <cmath>
#include <cstring>

namespace nthn_utils {
static inline float tau2pole(const float tau, const float sr) {
  return std::exp(-1.0f / (tau * sr));
}

static inline float lerp(const float x1, const float x2, const float alpha) {
  return x1 + alpha * (x2 - x1);
}

/**
 * Midi To Frequency
 * Conversion===========================================================================
 */
template <typename T> static constexpr inline T mtof(const T note_number) {
  return std::pow(T(2), note_number * T(0.08333333333) + T(3.03135971352));
}
/**
 * Frequency To Midi Function
 */
template <typename T> static constexpr inline T ftom(const T hertz) {
  return T(12) * std::log2(hertz) - T(36.376316562295983618);
}
// https://gist.github.com/jrade/293a73f89dfef51da6522428c857802d
static inline float fastPowerOfTwo(float x) {
  x = (1 << 23) * (x + 127);
  uint32_t n = static_cast<uint32_t>(x);
  memcpy(&x, &n, 4);
  return x;
}
// gain to db conversion
static inline float gain2db(const float gain) {
  return 20.0f * std::log10f(std::max(gain, 0.000001f));
}
static inline float db2gain(const float db) {
  return fastPowerOfTwo(db * 0.1660964047);
} // 2^db/(20 * log2(10))
static inline float db2gainAccurate(const float db) { return std::pow(10.0f, db / 20.0f); }
>>>>>>> Stashed changes
} // namespace nthn_utils
// nthn_utils
