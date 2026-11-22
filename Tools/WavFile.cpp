#include "WavFile.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <cstring>

namespace drmspace::wav
{
namespace
{
std::uint16_t u16(const unsigned char* p) noexcept
{
    return static_cast<std::uint16_t>(p[0] | (static_cast<unsigned>(p[1]) << 8));
}

std::uint32_t u32(const unsigned char* p) noexcept
{
    return static_cast<std::uint32_t>(p[0])
         | (static_cast<std::uint32_t>(p[1]) << 8)
         | (static_cast<std::uint32_t>(p[2]) << 16)
         | (static_cast<std::uint32_t>(p[3]) << 24);
}

void put16(std::ostream& out, std::uint16_t v)
{
    const unsigned char b[] { static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8) };
    out.write(reinterpret_cast<const char*>(b), 2);
}

void put32(std::ostream& out, std::uint32_t v)
{
    const unsigned char b[] { static_cast<unsigned char>(v), static_cast<unsigned char>(v >> 8),
                              static_cast<unsigned char>(v >> 16), static_cast<unsigned char>(v >> 24) };
    out.write(reinterpret_cast<const char*>(b), 4);
}

bool readBytes(std::istream& in, std::vector<unsigned char>& out, std::size_t size)
{
    out.resize(size);
    in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
    return in.good() || in.gcount() == static_cast<std::streamsize>(size);
}
}

bool read(const std::string& path, Audio& audio, std::string& error)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) { error = "Could not open input WAV: " + path; return false; }

    unsigned char header[12] {};
    in.read(reinterpret_cast<char*>(header), sizeof(header));
    if (!in || std::string(reinterpret_cast<char*>(header), 4) != "RIFF"
        || std::string(reinterpret_cast<char*>(header + 8), 4) != "WAVE")
    { error = "Input is not a RIFF/WAVE file"; return false; }

    std::uint16_t format = 0, channels = 0, bits = 0, blockAlign = 0;
    std::uint32_t sampleRate = 0;
    std::vector<unsigned char> sampleData;
    bool foundFormat = false, foundData = false;
    while (in && (!foundFormat || !foundData))
    {
        unsigned char chunkHeader[8] {};
        in.read(reinterpret_cast<char*>(chunkHeader), sizeof(chunkHeader));
        if (!in) break;
        const std::string name(reinterpret_cast<char*>(chunkHeader), 4);
        const auto size = u32(chunkHeader + 4);
        if (size > 0x7fffffffu) { error = "WAV chunk is too large"; return false; }
        if (name == "fmt ")
        {
            if (size < 16) { error = "WAV fmt chunk is too short"; return false; }
            std::vector<unsigned char> fmt;
            if (!readBytes(in, fmt, size)) { error = "Truncated WAV fmt chunk"; return false; }
            format = u16(fmt.data());
            channels = u16(fmt.data() + 2);
            sampleRate = u32(fmt.data() + 4);
            blockAlign = u16(fmt.data() + 12);
            bits = u16(fmt.data() + 14);
            if (format == 0xfffe && size >= 40)
            {
                const auto validBits = u16(fmt.data() + 18);
                format = u16(fmt.data() + 24);
                if (validBits != 0 && validBits != bits)
                { error = "WAV extensible valid-bit depth is unsupported"; return false; }
            }
            foundFormat = true;
        }
        else if (name == "data")
        {
            if (!readBytes(in, sampleData, size)) { error = "Truncated WAV data chunk"; return false; }
            foundData = true;
        }
        else
        {
            in.seekg(static_cast<std::streamoff>(size), std::ios::cur);
            if (!in) { error = "Truncated WAV chunk"; return false; }
        }
        if ((size & 1u) != 0u) in.seekg(1, std::ios::cur);
    }

    if (!foundFormat || !foundData) { error = "WAV is missing fmt or data chunk"; return false; }
    const bool pcm = format == 1 && (bits == 16 || bits == 24 || bits == 32);
    const bool float32 = format == 3 && bits == 32;
    if ((!pcm && !float32) || channels < 1 || channels > 2 || sampleRate < 8000
        || sampleRate > 384000 || blockAlign != channels * (bits / 8))
    { error = "WAV format unsupported (need mono/stereo PCM16/24/32 or float32)"; return false; }
    if (sampleData.size() % blockAlign != 0)
    { error = "WAV data size is not a whole number of frames"; return false; }

    audio.sampleRate = sampleRate;
    audio.channels = channels;
    audio.interleaved.resize(sampleData.size() / (bits / 8));
    const auto bytesPerSample = bits / 8;
    for (std::size_t i = 0; i < audio.interleaved.size(); ++i)
    {
        const auto* p = sampleData.data() + i * bytesPerSample;
        float sample = 0.0f;
        if (float32)
        {
            const auto raw = u32(p);
            std::memcpy(&sample, &raw, sizeof(sample));
        }
        else if (bits == 16)
            sample = static_cast<float>(static_cast<std::int16_t>(u16(p))) / 32768.0f;
        else if (bits == 24)
        {
            std::int32_t raw = static_cast<std::int32_t>(p[0] | (p[1] << 8) | (p[2] << 16));
            if ((raw & 0x00800000) != 0) raw |= static_cast<std::int32_t>(0xff000000u);
            sample = static_cast<float>(raw) / 8388608.0f;
        }
        else
            sample = static_cast<float>(static_cast<std::int32_t>(u32(p)) / 2147483648.0);
        audio.interleaved[i] = std::isfinite(sample) ? sample : 0.0f;
    }
    return true;
}

bool writeFloat32(const std::string& path, const Audio& audio, std::string& error)
{
    if (audio.channels < 1 || audio.channels > 2 || audio.sampleRate < 8000 || audio.sampleRate > 384000
        || audio.interleaved.size() % audio.channels != 0)
    { error = "Invalid audio format for WAV output"; return false; }
    const auto dataBytes64 = static_cast<std::uint64_t>(audio.interleaved.size()) * sizeof(float);
    if (dataBytes64 > std::numeric_limits<std::uint32_t>::max() - 36u)
    { error = "WAV output exceeds RIFF size limit"; return false; }
    const auto dataBytes = static_cast<std::uint32_t>(dataBytes64);
    std::ofstream out(path, std::ios::binary);
    if (!out) { error = "Could not open output WAV: " + path; return false; }
    out.write("RIFF", 4); put32(out, dataBytes + 36); out.write("WAVE", 4);
    out.write("fmt ", 4); put32(out, 16); put16(out, 3); put16(out, audio.channels);
    put32(out, audio.sampleRate); put32(out, audio.sampleRate * audio.channels * 4);
    put16(out, static_cast<std::uint16_t>(audio.channels * 4)); put16(out, 32);
    out.write("data", 4); put32(out, dataBytes);
    for (const auto sample : audio.interleaved)
    {
        const float finite = std::isfinite(sample) ? sample : 0.0f;
        std::uint32_t raw = 0;
        std::memcpy(&raw, &finite, sizeof(raw));
        put32(out, raw);
    }
    if (!out) { error = "Failed while writing WAV output"; return false; }
    return true;
}
}
