#include "ReverbEngines.h"
#include "WavFile.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace
{
void usage()
{
    std::puts("Usage: drmsmrtvrb_render --engine room|spring|gated --input in.wav --output wet.wav [options]\n"
              "Options: --decay seconds --predelay ms --damping 0..1 --width 0..2 --gate seconds --tail seconds\n"
              "Renders wet-only float32 WAV. Input: mono/stereo PCM16/24/32 or float32.");
}

bool number(const char* text, float& value)
{
    char* end = nullptr;
    value = std::strtof(text, &end);
    return end != text && *end == '\0' && std::isfinite(value);
}
}

int main(int argc, char** argv)
{
    drmspace::wav::Audio audio;
    std::string inputPath, outputPath, error;
    std::string engineName;
    drmspace::ReverbParameters parameters;
    float tailSeconds = 4.0f;
    for (int i = 1; i < argc; ++i)
    {
        const std::string option = argv[i];
        if (option == "--help" || option == "-h") { usage(); return 0; }
        if (i + 1 >= argc) { std::fprintf(stderr, "Missing value for %s\n", option.c_str()); usage(); return 2; }
        const char* value = argv[++i];
        if (option == "--engine") engineName = value;
        else if (option == "--input") inputPath = value;
        else if (option == "--output") outputPath = value;
        else if (option == "--decay") { if (!number(value, parameters.decaySeconds)) return 2; }
        else if (option == "--predelay") { if (!number(value, parameters.preDelayMs)) return 2; }
        else if (option == "--damping") { if (!number(value, parameters.damping)) return 2; }
        else if (option == "--width") { if (!number(value, parameters.width)) return 2; }
        else if (option == "--gate") { if (!number(value, parameters.gateSeconds)) return 2; }
        else if (option == "--tail") { if (!number(value, tailSeconds) || tailSeconds < 0.0f || tailSeconds > 60.0f) return 2; }
        else { std::fprintf(stderr, "Unknown option: %s\n", option.c_str()); usage(); return 2; }
    }

    drmspace::Engine engine;
    if (engineName == "room") engine = drmspace::Engine::room;
    else if (engineName == "spring") engine = drmspace::Engine::spring;
    else if (engineName == "gated") engine = drmspace::Engine::gated;
    else { std::fprintf(stderr, "Choose --engine room, spring, or gated\n"); return 2; }
    if (inputPath.empty() || outputPath.empty()) { usage(); return 2; }
    if (!drmspace::wav::read(inputPath, audio, error)) { std::fprintf(stderr, "%s\n", error.c_str()); return 1; }
    if (audio.frames() == 0) { std::fprintf(stderr, "Input WAV has no samples\n"); return 1; }

    drmspace::ReverbEngines reverb;
    constexpr int blockSize = 256;
    reverb.prepare(audio.sampleRate, blockSize);
    reverb.setEngine(engine);
    reverb.setParameters(parameters);
    std::vector<float> inLeft(blockSize), inRight(blockSize), outLeft(blockSize), outRight(blockSize);
    const float* inputs[] { inLeft.data(), inRight.data() };
    float* outputs[] { outLeft.data(), outRight.data() };
    drmspace::wav::Audio wet;
    wet.sampleRate = audio.sampleRate;
    wet.channels = audio.channels;
    const auto totalFrames = audio.frames() + static_cast<std::size_t>(tailSeconds * audio.sampleRate);
    wet.interleaved.resize(totalFrames * audio.channels);
    float peak = 0.0f;
    double sumSquares = 0.0;
    for (std::size_t offset = 0; offset < totalFrames; offset += blockSize)
    {
        const auto n = static_cast<int>(std::min<std::size_t>(blockSize, totalFrames - offset));
        for (int i = 0; i < n; ++i)
        {
            const auto frame = offset + static_cast<std::size_t>(i);
            inLeft[static_cast<std::size_t>(i)] = frame < audio.frames() ? audio.interleaved[frame * audio.channels] : 0.0f;
            inRight[static_cast<std::size_t>(i)] = audio.channels == 2 && frame < audio.frames()
                ? audio.interleaved[frame * 2 + 1] : 0.0f;
        }
        reverb.process(inputs, outputs, audio.channels, n);
        for (int i = 0; i < n; ++i)
        {
            for (int ch = 0; ch < audio.channels; ++ch)
            {
                const auto sample = ch == 0 ? outLeft[static_cast<std::size_t>(i)] : outRight[static_cast<std::size_t>(i)];
                const auto index = (offset + static_cast<std::size_t>(i)) * audio.channels + static_cast<std::size_t>(ch);
                wet.interleaved[index] = sample;
                peak = std::max(peak, std::abs(sample));
                sumSquares += static_cast<double>(sample) * sample;
            }
        }
    }
    if (!drmspace::wav::writeFloat32(outputPath, wet, error)) { std::fprintf(stderr, "%s\n", error.c_str()); return 1; }
    const auto sampleCount = wet.interleaved.size();
    const auto rms = sampleCount == 0 ? 0.0 : std::sqrt(sumSquares / sampleCount);
    std::printf("Engine: %s\nInput: %s\nOutput: %s\nFormat: %u Hz, %u channel(s), %.2f seconds\n"
                "Wet peak: %.6f (%.2f dBFS), RMS: %.6f\n",
                engineName.c_str(), inputPath.c_str(), outputPath.c_str(), audio.sampleRate, audio.channels,
                static_cast<double>(wet.frames()) / audio.sampleRate, peak,
                peak > 0.0f ? 20.0 * std::log10(peak) : -120.0, rms);
    return 0;
}
