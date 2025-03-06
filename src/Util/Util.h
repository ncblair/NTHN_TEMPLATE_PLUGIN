#include <cmath>

namespace nthn_utils {
static inline float tau2pole(const float tau, const float sr) {
  return std::exp(-1.0f / (tau * sr));
}

static inline float lerp(const float x1, const float x2, const float alpha) {
  return x1 + alpha * (x2 - x1);
}
} // namespace nthn_utils
// nthn_utils
