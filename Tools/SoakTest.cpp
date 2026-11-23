#include "PluginProcessor.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>

#if defined(__unix__) || defined(__APPLE__)
 #include <sys/resource.h>
#endif

// Stability soak test for the full JUCE processor: random block sizes, several
// sample rates and layouts, random parameter changes (every exposed parameter),
// preset switching, repeated prepareToPlay, and loud/silent/DC input. Fails on
// non-finite or unbounded output, or on resident-memory growth in a repeated re-prepare phase (POSIX only).
namespace
{
constexpr float outputBound = 16.0f;

long residentKilobytes()
{
#if defined(__unix__) || defined(__APPLE__)
    rusage usage {};
    getrusage(RUSAGE_SELF, &usage);
   #if defined(__APPLE__)
    return usage.ru_maxrss / 1024; // bytes on macOS
   #else
    return usage.ru_maxrss;
   #endif
#else
    return 0;
#endif
}
}

int main(int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    const double secondsPerConfig = argc > 1 ? std::atof(argv[1]) : 20.0;
    const unsigned seed = argc > 2 ? static_cast<unsigned>(std::atoi(argv[2])) : 12345u;
    if (secondsPerConfig <= 0.0)
    {
        std::puts("Usage: drmsmrtvrb_soak_test [secondsPerConfig=20] [seed=12345]");
        return 2;
    }

    const double sampleRates[] = { 22050.0, 44100.0, 48000.0, 88200.0, 96000.0, 192000.0 };
    const int layouts[] = { 2, 1 };
    constexpr int maximumBlock = 4096;
    std::mt19937 rng(seed);
    auto uniform = [&rng](float low, float high) { return std::uniform_real_distribution<float>(low, high)(rng); };

    int failures = 0;
    float overallPeak = 0.0f;
    double totalAudioSeconds = 0.0;

    for (const double sampleRate : sampleRates)
        for (const int channels : layouts)
        {
            DRMsmrtvrbAudioProcessor processor;
            processor.setPlayConfigDetails(channels, channels, sampleRate, maximumBlock);
            processor.prepareToPlay(sampleRate, maximumBlock);
            juce::AudioBuffer<float> buffer(channels, maximumBlock);
            juce::MidiBuffer midi;
            const long totalSamples = static_cast<long>(secondsPerConfig * sampleRate);
            long processed = 0, nextEvent = 0;
            float peak = 0.0f;
            bool finite = true;
            int inputMode = 0;

            while (processed < totalSamples)
            {
                if (processed >= nextEvent)
                {
                    nextEvent = processed + static_cast<long>(sampleRate * uniform(0.02f, 0.5f));
                    inputMode = std::uniform_int_distribution<int>(0, 4)(rng);
                    const int action = std::uniform_int_distribution<int>(0, 9)(rng);
                    if (action == 0)
                        processor.setCurrentProgram(std::uniform_int_distribution<int>(0, processor.getNumPrograms() - 1)(rng));
                    else if (action == 1)
                        processor.prepareToPlay(sampleRate, std::uniform_int_distribution<int>(1, maximumBlock)(rng));
                    else
                    {
                        auto& params = processor.getParameters();
                        const int count = std::uniform_int_distribution<int>(1, 4)(rng);
                        for (int n = 0; n < count; ++n)
                        {
                            auto* p = params[std::uniform_int_distribution<int>(0, params.size() - 1)(rng)];
                            // Bias towards range ends to exercise extremes.
                            const float v = uniform(0.0f, 1.0f);
                            p->setValueNotifyingHost(v < 0.15f ? 0.0f : v > 0.85f ? 1.0f : v);
                        }
                    }
                }

                const int block = std::min<long>(std::uniform_int_distribution<int>(1, maximumBlock)(rng),
                                                 totalSamples - processed);
                buffer.setSize(channels, block, false, false, true);
                for (int c = 0; c < channels; ++c)
                    for (int i = 0; i < block; ++i)
                    {
                        const double t = static_cast<double>(processed + i) / sampleRate;
                        float x = 0.0f;
                        switch (inputMode)
                        {
                            case 0: x = 0.0f; break;
                            case 1: x = uniform(-1.0f, 1.0f); break;
                            case 2: x = static_cast<float>(std::sin(6.283185307 * 110.0 * t)); break;
                            case 3: x = 1.0f; break; // full-scale DC
                            default: x = std::fmod(t, 0.5) < 0.01 ? uniform(-1.0f, 1.0f) : 0.0f; break;
                        }
                        buffer.setSample(c, i, x);
                    }
                processor.processBlock(buffer, midi);
                for (int c = 0; c < channels; ++c)
                    for (int i = 0; i < block; ++i)
                    {
                        const float y = buffer.getSample(c, i);
                        finite = finite && std::isfinite(y);
                        peak = std::max(peak, std::abs(y));
                    }
                processed += block;
            }

            totalAudioSeconds += secondsPerConfig;
            overallPeak = std::max(overallPeak, peak);
            const bool bounded = peak <= outputBound;
            std::printf("%7.0f Hz  %d ch  peak %.3f  finite %s  bounded %s\n", sampleRate, channels, peak,
                        finite ? "yes" : "NO", bounded ? "yes" : "NO");
            if (!finite || !bounded) ++failures;
        }

    // Leak phase: peak RSS is a high-water mark, so repeat the heaviest
    // configuration (largest buffers) many times after a warm-up and compare.
    long leakGrowthKb = 0;
    {
        constexpr double leakRate = 192000.0;
        DRMsmrtvrbAudioProcessor processor;
        processor.setPlayConfigDetails(2, 2, leakRate, maximumBlock);
        juce::AudioBuffer<float> buffer(2, maximumBlock);
        juce::MidiBuffer midi;
        auto cycle = [&]
        {
            processor.prepareToPlay(leakRate, maximumBlock);
            processor.setCurrentProgram(std::uniform_int_distribution<int>(0, processor.getNumPrograms() - 1)(rng));
            for (int n = 0; n < 20; ++n)
            {
                for (int c = 0; c < 2; ++c)
                    for (int i = 0; i < maximumBlock; ++i) buffer.setSample(c, i, uniform(-1.0f, 1.0f));
                processor.processBlock(buffer, midi);
            }
        };
        for (int n = 0; n < 5; ++n) cycle();
        const long before = residentKilobytes();
        for (int n = 0; n < 200; ++n) cycle();
        leakGrowthKb = residentKilobytes() - before;
    }
    std::printf("audio simulated: %.0f s, overall peak %.3f; leak phase (200 re-prepare cycles at 192 kHz) RSS growth %ld kB\n",
                totalAudioSeconds, overallPeak, leakGrowthKb);
    if (leakGrowthKb > 16384)
    {
        std::puts("FAIL: resident memory grew by more than 16 MB during the leak phase");
        ++failures;
    }
    std::puts(failures == 0 ? "PASS" : "FAIL");
    return failures == 0 ? 0 : 1;
}
