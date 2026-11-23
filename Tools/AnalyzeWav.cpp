#include "AudioMetrics.h"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{
void usage()
{
    std::puts("Usage: drmsmrtvrb_analyze --wet wet.wav [--dry dry.wav] [--csv]\n"
              "Objective measurements of a rendered wet signal (from drmsmrtvrb_render) to support\n"
              "listening auditions: level, low/high energy share, decay times, stereo stability.\n"
              "These numbers do not replace listening.");
}

void print(const char* label, const drmspace::metrics::Metrics& m, bool csv)
{
    if (csv)
    {
        std::printf("%s,%.2f,%.2f,%.4f,%.4f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", label, m.peakDb, m.rmsDb,
                    m.lowFraction, m.highFraction, m.secondsToMinus20Db, m.secondsToMinus40Db, m.secondsToMinus60Db,
                    m.rt60FromMinus20Db, m.correlation, m.windowCorrelationMean, m.windowCorrelationMin, m.sideToMidDb);
        return;
    }
    std::printf("[%s]\n", label);
    std::printf("  peak %.1f dBFS, RMS %.1f dBFS\n", m.peakDb, m.rmsDb);
    std::printf("  energy below %.0f Hz: %.1f%%, above %.0f Hz: %.1f%%\n", drmspace::metrics::lowCutoffHz,
                m.lowFraction * 100.0, drmspace::metrics::highCutoffHz, m.highFraction * 100.0);
    std::printf("  time to -20/-40/-60 dB after peak (s, -1 = not reached): %.3f / %.3f / %.3f; RT60 est. from -20 dB: %.3f\n",
                m.secondsToMinus20Db, m.secondsToMinus40Db, m.secondsToMinus60Db, m.rt60FromMinus20Db);
    if (m.stereo)
        std::printf("  stereo: correlation %.3f; 100 ms windows mean %.3f, min %.3f, stddev %.3f; side/mid %.1f dB\n",
                    m.correlation, m.windowCorrelationMean, m.windowCorrelationMin, m.windowCorrelationStdDev,
                    m.sideToMidDb);
}
}

int main(int argc, char** argv)
{
    std::string wetPath, dryPath;
    bool csv = false;
    for (int i = 1; i < argc; ++i)
    {
        const std::string option = argv[i];
        if (option == "--help" || option == "-h") { usage(); return 0; }
        if (option == "--csv") { csv = true; continue; }
        if ((option == "--wet" || option == "--dry") && i + 1 < argc)
            (option == "--wet" ? wetPath : dryPath) = argv[++i];
        else { std::fprintf(stderr, "Unknown or incomplete option: %s\n", option.c_str()); usage(); return 2; }
    }
    if (wetPath.empty()) { usage(); return 2; }

    drmspace::wav::Audio wet, dry;
    std::string error;
    if (!drmspace::wav::read(wetPath, wet, error)) { std::fprintf(stderr, "%s\n", error.c_str()); return 1; }
    const auto wetMetrics = drmspace::metrics::analyze(wet);
    if (csv)
        std::puts("label,peak_db,rms_db,low_fraction,high_fraction,t_minus20_s,t_minus40_s,t_minus60_s,rt60_est_s,"
                  "correlation,window_corr_mean,window_corr_min,side_to_mid_db");
    print("wet", wetMetrics, csv);
    if (!dryPath.empty())
    {
        if (!drmspace::wav::read(dryPath, dry, error)) { std::fprintf(stderr, "%s\n", error.c_str()); return 1; }
        const auto dryMetrics = drmspace::metrics::analyze(dry);
        print("dry", dryMetrics, csv);
        if (!csv)
            std::printf("  low-frequency share, wet vs dry: %+.1f dB\n",
                        drmspace::metrics::lowFractionChangeDb(dryMetrics, wetMetrics));
    }
    return 0;
}
