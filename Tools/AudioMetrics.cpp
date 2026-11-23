#include "AudioMetrics.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace drmspace::metrics
{
namespace
{
constexpr double pi = 3.14159265358979323846;

double toDb(double power)
{
    return power > 1.0e-20 ? 10.0 * std::log10(power) : -200.0;
}

struct Biquad
{
    double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;

    static Biquad make(bool highpass, double sampleRate, double cutoff, double q)
    {
        const double w = 2.0 * pi * std::min(cutoff, sampleRate * 0.45) / sampleRate;
        const double cosw = std::cos(w), alpha = std::sin(w) / (2.0 * q);
        const double a0 = 1.0 + alpha;
        Biquad f;
        if (highpass) { f.b0 = (1 + cosw) / 2 / a0; f.b1 = -(1 + cosw) / a0; f.b2 = f.b0; }
        else { f.b0 = (1 - cosw) / 2 / a0; f.b1 = (1 - cosw) / a0; f.b2 = f.b0; }
        f.a1 = -2.0 * cosw / a0;
        f.a2 = (1.0 - alpha) / a0;
        return f;
    }

    double process(double x)
    {
        const double y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }
};

// 4th-order Butterworth (two cascaded biquads).
struct Filter4
{
    std::array<Biquad, 2> stages;
    Filter4(bool highpass, double sampleRate, double cutoff)
    {
        stages[0] = Biquad::make(highpass, sampleRate, cutoff, 0.5411961);
        stages[1] = Biquad::make(highpass, sampleRate, cutoff, 1.3065630);
    }
    double process(double x) { return stages[1].process(stages[0].process(x)); }
};

double bandEnergy(const wav::Audio& audio, bool highpass, double cutoff)
{
    double energy = 0.0;
    for (std::size_t channel = 0; channel < audio.channels; ++channel)
    {
        Filter4 filter(highpass, audio.sampleRate, cutoff);
        for (std::size_t i = 0; i < audio.frames(); ++i)
        {
            const double y = filter.process(audio.interleaved[i * audio.channels + channel]);
            energy += y * y;
        }
    }
    return energy;
}

double sample(const wav::Audio& audio, std::size_t frame, std::size_t channel)
{
    return audio.interleaved[frame * audio.channels + channel];
}

double correlationOf(const wav::Audio& audio, std::size_t begin, std::size_t end)
{
    double ll = 0, rr = 0, lr = 0;
    for (std::size_t i = begin; i < end; ++i)
    {
        const double l = sample(audio, i, 0), r = sample(audio, i, 1);
        ll += l * l; rr += r * r; lr += l * r;
    }
    const double denominator = std::sqrt(ll * rr);
    return denominator > 1.0e-20 ? lr / denominator : 0.0;
}
}

Metrics analyze(const wav::Audio& audio)
{
    Metrics m;
    const std::size_t frames = audio.frames();
    if (frames == 0 || audio.sampleRate == 0 || audio.channels == 0) return m;

    double peak = 0.0, total = 0.0;
    for (const float v : audio.interleaved)
    {
        peak = std::max(peak, static_cast<double>(std::abs(v)));
        total += static_cast<double>(v) * v;
    }
    m.peakDb = toDb(peak * peak);
    m.rmsDb = toDb(total / static_cast<double>(audio.interleaved.size()));
    if (total > 1.0e-20)
    {
        m.lowFraction = bandEnergy(audio, false, lowCutoffHz) / total;
        m.highFraction = bandEnergy(audio, true, highCutoffHz) / total;
    }

    // 10 ms RMS envelope decay after its peak.
    const std::size_t window = std::max<std::size_t>(1, static_cast<std::size_t>(audio.sampleRate * 0.010));
    std::vector<double> envelope;
    for (std::size_t start = 0; start + window <= frames; start += window)
    {
        double energy = 0.0;
        for (std::size_t i = start; i < start + window; ++i)
            for (std::size_t c = 0; c < audio.channels; ++c)
            {
                const double v = sample(audio, i, c);
                energy += v * v;
            }
        envelope.push_back(toDb(energy / static_cast<double>(window * audio.channels)));
    }
    if (!envelope.empty())
    {
        const auto peakIt = std::max_element(envelope.begin(), envelope.end());
        const std::size_t peakIndex = static_cast<std::size_t>(peakIt - envelope.begin());
        const auto crossing = [&](double drop)
        {
            for (std::size_t i = peakIndex + 1; i < envelope.size(); ++i)
                if (envelope[i] <= *peakIt - drop)
                    return static_cast<double>(i - peakIndex) * window / audio.sampleRate;
            return -1.0;
        };
        m.secondsToMinus20Db = crossing(20.0);
        m.secondsToMinus40Db = crossing(40.0);
        m.secondsToMinus60Db = crossing(60.0);
        m.rt60FromMinus20Db = m.secondsToMinus20Db >= 0.0 ? m.secondsToMinus20Db * 3.0 : -1.0;
    }

    if (audio.channels == 2)
    {
        m.stereo = true;
        m.correlation = correlationOf(audio, 0, frames);
        double mid = 0.0, side = 0.0;
        for (std::size_t i = 0; i < frames; ++i)
        {
            const double l = sample(audio, i, 0), r = sample(audio, i, 1);
            mid += (l + r) * (l + r) * 0.25;
            side += (l - r) * (l - r) * 0.25;
        }
        m.sideToMidDb = toDb(side) - toDb(mid);

        const std::size_t span = std::max<std::size_t>(1, static_cast<std::size_t>(audio.sampleRate * 0.100));
        std::vector<double> energies, correlations;
        for (std::size_t start = 0; start + span <= frames; start += span)
        {
            double energy = 0.0;
            for (std::size_t i = start; i < start + span; ++i)
                energy += sample(audio, i, 0) * sample(audio, i, 0) + sample(audio, i, 1) * sample(audio, i, 1);
            energies.push_back(energy);
            correlations.push_back(correlationOf(audio, start, start + span));
        }
        const double loudest = energies.empty() ? 0.0 : *std::max_element(energies.begin(), energies.end());
        std::vector<double> active;
        for (std::size_t i = 0; i < energies.size(); ++i)
            if (loudest > 0.0 && energies[i] >= loudest * 1.0e-4) active.push_back(correlations[i]);
        if (!active.empty())
        {
            double sum = 0.0;
            for (const double c : active) sum += c;
            const double mean = sum / static_cast<double>(active.size());
            double variance = 0.0;
            for (const double c : active) variance += (c - mean) * (c - mean);
            m.windowCorrelationMean = mean;
            m.windowCorrelationMin = *std::min_element(active.begin(), active.end());
            m.windowCorrelationStdDev = std::sqrt(variance / static_cast<double>(active.size()));
        }
    }
    return m;
}

double lowFractionChangeDb(const Metrics& dry, const Metrics& wet)
{
    if (dry.lowFraction <= 0.0 || wet.lowFraction <= 0.0) return 0.0;
    return 10.0 * std::log10(wet.lowFraction / dry.lowFraction);
}
}
