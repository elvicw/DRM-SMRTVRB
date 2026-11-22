# Build Instructions

## Current status

No DRM-SMRTVRB build files or plugin target exist yet. Initial machine inspection found macOS, Apple clang 21.0.1, CMake 4.5.0, and Ninja. No Windows cross-compiler or JUCE Homebrew package was detected. Windows is the only supported build/release target for V1.

## Framework investigation

The neighboring DRM-RESHAPE source uses CMake and fetches JUCE 8.0.6. Before adopting it, review license obligations and pin dependencies for reproducible builds. Windows VST3 build prerequisites and exact commands will be documented after the project build system exists.

## Future Windows build

The intended artifact is a Windows VST3, built and verified on a Windows environment. Document compiler/toolchain versions and exact configure, build, test, and packaging commands when established. Do not report a build as successful without its actual output.
