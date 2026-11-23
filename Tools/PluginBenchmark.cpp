#include "PluginProcessor.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>

// Offline CPU benchmark of the full JUCE processor (parameter reads, tempo,
// smoothing, meters, activity history) for every factory preset. Not a test.
int main(int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    const double sampleRate = argc > 1 ? std::atof(argv[1]) : 48000.0;
    const int blockSize = argc > 2 ? std::atoi(argv[2]) : 512;
    const double seconds = argc > 3 ? std::atof(argv[3]) : 30.0;
    if (sampleRate < 8000.0 || blockSize < 1 || seconds <= 0.0)
    {
        std::puts("Usage: drmsmrtvrb_plugin_benchmark [sampleRate=48000] [blockSize=512] [seconds=30]");
        return 2;
    }

    DRMsmrtvrbAudioProcessor processor;
    processor.setPlayConfigDetails(2, 2, sampleRate, blockSize);
    processor.prepareToPlay(sampleRate, blockSize);

    const int totalBlocks = static_cast<int>(seconds * sampleRate / blockSize);
    juce::AudioBuffer<float> buffer(2, blockSize);
    juce::MidiBuffer midi;
    unsigned seed = 1;
    bool allFinite = true;
    double worstCpu = 0.0;
    volatile double sink = 0.0; // keeps the processed output observable

    std::printf("sampleRate=%.0f blockSize=%d seconds=%.1f (stereo, factory presets, no host playhead)\n",
                sampleRate, blockSize, seconds);
    std::printf("%-20s %9s %12s %s\n", "preset", "CPU %", "x realtime", "finite");

    for (int program = 0; program < processor.getNumPrograms(); ++program)
    {
        processor.setCurrentProgram(program);
        processor.reset();
        bool finite = true;
        double checksum = 0.0;
        const auto start = std::chrono::steady_clock::now();
        for (int b = 0; b < totalBlocks; ++b)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const double time = (static_cast<double>(b) * blockSize + i) / sampleRate;
                seed = seed * 1664525u + 1013904223u;
                const float noise = std::fmod(time, 1.0) < 0.1
                    ? (static_cast<float>(seed >> 8) / 8388608.0f - 1.0f) * 0.5f : 0.0f;
                buffer.setSample(0, i, noise);
                buffer.setSample(1, i, noise);
            }
            processor.processBlock(buffer, midi);
            for (int i = 0; i < blockSize; ++i)
            {
                finite = finite && std::isfinite(buffer.getSample(0, i)) && std::isfinite(buffer.getSample(1, i));
                checksum += buffer.getSample(0, i);
            }
        }
        const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        const double audio = static_cast<double>(totalBlocks) * blockSize / sampleRate;
        const double cpu = 100.0 * elapsed / audio;
        worstCpu = cpu > worstCpu ? cpu : worstCpu;
        allFinite = allFinite && finite;
        sink = sink + checksum;
        std::printf("%-20s %9.3f %12.1f %s\n", processor.getProgramName(program).toRawUTF8(), cpu, audio / elapsed,
                    finite ? "yes" : "NO");
    }
    std::printf("worst-case CPU: %.3f %% of one core\n", worstCpu);
    return allFinite ? 0 : 1;
}
