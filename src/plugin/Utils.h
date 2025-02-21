#ifndef UTILS_H
#define UTILS_H

#include <cmath>

namespace Utils {
    inline float tau2pole(float tau, float sr) {
        return std::exp(-1.0f / (tau * sr));
    }

    inline float lerp(float x1, float x2, float alpha) {
        return x1 + alpha * (x2 - x1);
    }
}

#endif // UTILS_H
