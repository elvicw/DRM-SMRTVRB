#include "AudioMetrics.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAILED: %s\n", message); std::abort(); }
}

constexpr std::uint32_t rate = 48000;

drmspace::wav::Audio make(int channels, double seconds)
{
    drmspace::wav::Audio audio;
    audio.sampleRate = rate;
    audio.channels = static_cast<std::uint16_t>(channels);
    audio.interleaved.assign(static_cast<std::size_t>(seconds * rate) * channels, 0.0f);
    return audio;
}

drmspace::wav::Audio sine(double hz)
{
    auto audio = make(2, 1.0);
    for (std::size_t i = 0; i < audio.frames(); ++i)
        for (int c = 0; c < 2; ++c)
            audio.interleaved[i * 2 + c] = static_cast<float>(0.5 * std::sin(2.0 * 3.14159265358979 * hz * i / rate));
    return audio;
}
}

int main()
{
    using namespace drmspace::metrics;

    // Band shares.
    const auto low = analyze(sine(60.0));
    require(low.lowFraction > 0.95 && low.highFraction < 0.01, "60 Hz sine is low-band energy");
    const auto high = analyze(sine(10000.0));
    require(high.lowFraction < 0.01 && high.highFraction > 0.95, "10 kHz sine is high-band energy");
    require(std::abs(low.peakDb - 20.0 * std::log10(0.5)) < 0.1, "peak level in dBFS");
    require(lowFractionChangeDb(high, low) > 10.0, "low-share change positive for more bass");

    // Known decay: noise whose amplitude falls 60 dB in 1.0 s.
    {
        auto audio = make(2, 3.0);
        std::mt19937 rng(7);
        std::uniform_real_distribution<float> noise(-1.0f, 1.0f);
        for (std::size_t i = 0; i < audio.frames(); ++i)
        {
            const float gain = static_cast<float>(std::pow(10.0, -3.0 * static_cast<double>(i) / rate));
            for (int c = 0; c < 2; ++c) audio.interleaved[i * 2 + c] = noise(rng) * gain;
        }
        const auto m = analyze(audio);
        require(std::abs(m.secondsToMinus20Db - 0.333) < 0.06, "-20 dB time near 1/3 s");
        require(std::abs(m.secondsToMinus40Db - 0.667) < 0.06, "-40 dB time near 2/3 s");
        require(std::abs(m.rt60FromMinus20Db - 1.0) < 0.15, "RT60 estimate near 1 s");
        require(m.secondsToMinus60Db > 0.9 && m.secondsToMinus60Db < 1.15, "-60 dB time near 1 s");
    }

    // Envelope that never decays is reported as not reached.
    require(analyze(sine(440.0)).secondsToMinus20Db < 0.0, "steady tone does not decay");

    // Stereo correlation.
    {
        auto same = make(2, 1.0), opposite = make(2, 1.0), independent = make(2, 1.0);
        std::mt19937 rng(3);
        std::uniform_real_distribution<float> noise(-1.0f, 1.0f);
        for (std::size_t i = 0; i < same.frames(); ++i)
        {
            const float a = noise(rng), b = noise(rng);
            same.interleaved[i * 2] = a; same.interleaved[i * 2 + 1] = a;
            opposite.interleaved[i * 2] = a; opposite.interleaved[i * 2 + 1] = -a;
            independent.interleaved[i * 2] = a; independent.interleaved[i * 2 + 1] = b;
        }
        const auto s = analyze(same), o = analyze(opposite), n = analyze(independent);
        require(s.correlation > 0.999 && s.sideToMidDb < -60.0, "identical channels are fully correlated");
        require(o.correlation < -0.999 && o.sideToMidDb > 60.0, "inverted channels anti-correlate");
        require(std::abs(n.correlation) < 0.1 && std::abs(n.windowCorrelationMean) < 0.1, "independent noise decorrelates");
        require(s.windowCorrelationStdDev < 0.001, "steady correlation has near-zero spread");
    }

    // Mono input and silence are handled.
    {
        auto mono = make(1, 0.5);
        mono.interleaved[100] = 0.5f;
        require(!analyze(mono).stereo, "mono is not stereo");
        const auto silent = analyze(make(2, 0.5));
        require(silent.peakDb <= -199.0 && silent.lowFraction == 0.0, "silence yields floor values");
        require(analyze(drmspace::wav::Audio {}).peakDb <= -199.0, "empty audio is safe");
    }

    std::puts("AudioMetrics tests passed");
    return 0;
}
