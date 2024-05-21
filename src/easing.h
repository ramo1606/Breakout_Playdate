#ifndef EASING_H
#define EASING_H

#include <math.h>

#ifndef PI
#define PI 3.1415926545
#endif

inline float easeInSine(float t) {
    return sin(1.5707963 * t);
}

inline float easeOutSine(float t) {
    return 1 + sin(1.5707963 * (--t));
}

inline float easeInOutSine(float t) {
    return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}

inline float easeInQuad(float t) {
    return t * t;
}

inline float easeOutQuad(float t) {
    return t * (2 - t);
}

inline float easeInOutQuad(float t) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

inline float easeInCubic(float t) {
    return t * t * t;
}

inline float easeOutCubic(float t) {
    return 1 + (--t) * t * t;
}

inline float easeInOutCubic(float t) {
    return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}

inline float easeInQuart(float t) {
    t *= t;
    return t * t;
}

inline float easeOutQuart(float t) {
    t = (--t) * t;
    return 1 - t * t;
}

inline float easeInOutQuart(float t) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    }
    else {
        t = (--t) * t;
        return 1 - 8 * t * t;
    }
}

inline float easeInQuint(float t) {
    float t2 = t * t;
    return t * t2 * t2;
}

inline float easeOutQuint(float t) {
    float t2 = (--t) * t;
    return 1 + t * t2 * t2;
}

inline float easeInOutQuint(float t) {
    float t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    }
    else {
        t2 = (--t) * t;
        return 1 + 16 * t * t2 * t2;
    }
}

inline float easeInExpo(float t) {
    return (pow(2, 8 * t) - 1) / 255;
}

inline float easeOutExpo(float t) {
    return 1 - pow(2, -8 * t);
}

inline float easeInOutExpo(float t) {
    if (t < 0.5) {
        return (pow(2, 16 * t) - 1) / 510;
    }
    else {
        return 1 - 0.5 * pow(2, -16 * (t - 0.5));
    }
}

inline float easeInCirc(float t) {
    return 1 - sqrt(1 - t);
}

inline float easeOutCirc(float t) {
    return sqrt(t);
}

inline float easeInOutCirc(float t) {
    if (t < 0.5) {
        return (1 - sqrt(1 - 2 * t)) * 0.5;
    }
    else {
        return (1 + sqrt(2 * t - 1)) * 0.5;
    }
}

inline float easeInBack(float t) {
    return t * t * (2.70158 * t - 1.70158);
}

inline float easeOutBack(float t) {
    return 1 + (--t) * t * (2.70158 * t + 1.70158);
}

inline float easeInOutBack(float t) {
    if (t < 0.5) {
        return t * t * (7 * t - 2.5) * 2;
    }
    else {
        return 1 + (--t) * t * 2 * (7 * t + 2.5);
    }
}

inline float easeInElastic(float t) {
    float t2 = t * t;
    return t2 * t2 * sin(t * PI * 4.5);
}

inline float easeOutElastic(float t) {
    float t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos(t * PI * 4.5);
}

inline float easeInOutElastic(float t) {
    float t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sin(t * PI * 9);
    }
    else if (t < 0.55) {
        return 0.5 + 0.75 * sin(t * PI * 4);
    }
    else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin(t * PI * 9);
    }
}

inline float easeInBounce(float t) {
    return pow(2, 6 * (t - 1)) * abs(sin(t * PI * 3.5));
}

inline float easeOutBounce(float t) {
    return 1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5));
}

inline float easeInOutBounce(float t) {
    if (t < 0.5) {
        return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * PI * 7));
    }
    else {
        return 1 - 8 * pow(2, -8 * t) * abs(sin(t * PI * 7));
    }
}

#endif