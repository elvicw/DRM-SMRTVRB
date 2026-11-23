#include "SpaceMacro.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

// Offline CPU benchmark for the SPACE macro processor (all source targets x
// engines). Reports real-time CPU use for stereo noise bursts followed by silence
// so the decaying tail (denormal risk) is included. Not a pass/fail test.
int main(int argc, char** argv)
{
    const double sampleRate = argc > 1 ? std::atof(argv[1]) : 48000.0;
    const int blockSize = argc > 2 ? std::atoi(argv[2]) : 512;
    const double seconds = argc > 3 ? std::atof(argv[3]) : 30.0;
    if (sampleRate < 8000.0 || blockSize < 1 || seconds <= 0.0)
    {
        std::puts("Usage: drmsmrtvrb_benchmark [sampleRate=48000] [blockSize=512] [seconds=30]");
        return 2;
    }

    const char* targets[] = { "shells", "snare", "room", "overheads" };
    const char* engines[] = { "room", "spring", "gated" };
    const int totalBlocks = static_cast<int>(seconds * sampleRate / blockSize);
    std::vector<float> inL(static_cast<size_t>(blockSize)), inR(inL), outL(inL), outR(inL);

    std::printf("sampleRate=%.0f blockSize=%d seconds=%.1f (stereo, SPACE 65)\n", sampleRate, blockSize, seconds);
    std::printf("%-10s %-7s %9s %12s %s\n", "target", "engine", "CPU %", "x realtime", "finite");

    unsigned seed = 1;
    volatile double sink = 0.0; // keeps the processed output observable
    bool allFinite = true;
    double worstCpu = 0.0;
    for (int t = 0; t < 4; ++t)
        for (int e = 0; e < 3; ++e)
        {
            drmspace::SpaceMacroProcessor processor;
            processor.prepare(sampleRate, blockSize, static_cast<drmspace::SourceTarget>(t),
                              static_cast<drmspace::Engine>(e));
            processor.setSpace(65.0f);
            const float* in[] = { inL.data(), inR.data() };
            float* out[] = { outL.data(), outR.data() };
            bool finite = true;
            double checksum = 0.0;
            const auto start = std::chrono::steady_clock::now();
            for (int b = 0; b < totalBlocks; ++b)
            {
                // One 100 ms noise burst per second; silence otherwise.
                for (int i = 0; i < blockSize; ++i)
                {
                    const double time = (static_cast<double>(b) * blockSize + i) / sampleRate;
                    const bool burst = std::fmod(time, 1.0) < 0.1;
                    seed = seed * 1664525u + 1013904223u;
                    const float noise = burst ? (static_cast<float>(seed >> 8) / 8388608.0f - 1.0f) * 0.5f : 0.0f;
                    inL[static_cast<size_t>(i)] = noise;
                    inR[static_cast<size_t>(i)] = noise;
                }
                processor.process(in, out, 2, blockSize);
                for (int i = 0; i < blockSize; ++i)
                {
                    finite = finite && std::isfinite(outL[static_cast<size_t>(i)]) && std::isfinite(outR[static_cast<size_t>(i)]);
                    checksum += outL[static_cast<size_t>(i)];
                }
            }
            const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
            const double audio = static_cast<double>(totalBlocks) * blockSize / sampleRate;
            const double cpu = 100.0 * elapsed / audio;
            worstCpu = cpu > worstCpu ? cpu : worstCpu;
            allFinite = allFinite && finite;
            sink = sink + checksum;
            std::printf("%-10s %-7s %9.3f %12.1f %s\n", targets[t], engines[e], cpu, audio / elapsed,
                        finite ? "yes" : "NO");
        }
    std::printf("worst-case CPU: %.3f %% of one core\n", worstCpu);
    return allFinite ? 0 : 1;
}
