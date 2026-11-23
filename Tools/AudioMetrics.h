#pragma once

#include "WavFile.h"

namespace drmspace::metrics
{
struct Metrics
{
    double peakDb = -200.0;
    double rmsDb = -200.0;
    // Fractions of total energy (0..1).
    double lowFraction = 0.0;   // below lowCutoffHz
    double highFraction = 0.0;  // above highCutoffHz
    // Decay of the 10 ms RMS envelope after its peak. Seconds, or -1 if the
    // envelope never falls that far within the file.
    double secondsToMinus20Db = -1.0;
    double secondsToMinus40Db = -1.0;
    double secondsToMinus60Db = -1.0;
    double rt60FromMinus20Db = -1.0; // 3 x secondsToMinus20Db
    // Stereo (zero-valued for mono input).
    double correlation = 0.0;           // overall L/R correlation
    double windowCorrelationMean = 0.0; // 100 ms windows within 40 dB of the loudest
    double windowCorrelationMin = 0.0;
    double windowCorrelationStdDev = 0.0;
    double sideToMidDb = -200.0;
    bool stereo = false;
};

constexpr double lowCutoffHz = 150.0;
constexpr double highCutoffHz = 6000.0;

Metrics analyze(const wav::Audio& audio);

// Low-frequency energy share of wet relative to dry, in dB (positive means the
// wet signal has proportionally more low end than the source).
double lowFractionChangeDb(const Metrics& dry, const Metrics& wet);
}
