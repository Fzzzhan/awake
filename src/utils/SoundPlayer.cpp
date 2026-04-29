#include "SoundPlayer.h"
#include "Logger.h"
#include <cstdlib>
#include <array>
#include <memory>
#include <iostream>

#ifdef __linux__
    #define OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MACOS
#elif defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
#endif

bool SoundPlayer::playSound(SoundType type) {
    std::string soundPath = getSoundPath(type);

    if (soundPath.empty()) {
        LOG_WARN("No sound path found for the requested sound type");
        return beep(); // Fallback to beep
    }

#ifdef OS_LINUX
    // Try multiple audio players in order of preference
    std::array<std::string, 3> players = {"paplay", "aplay", "play"};

    for (const auto& player : players) {
        // Check if player exists
        std::string checkCmd = "which " + player + " > /dev/null 2>&1";
        if (system(checkCmd.c_str()) == 0) {
            std::string cmd = player + " " + soundPath + " > /dev/null 2>&1 &";
            LOG_DEBUG("Playing sound with {}: {}", player, soundPath);
            return executeCommand(cmd);
        }
    }

    LOG_WARN("No audio player found (paplay, aplay, play)");
    return beep();

#elif defined(OS_MACOS)
    std::string cmd = "afplay " + soundPath + " &";
    LOG_DEBUG("Playing sound on macOS: {}", soundPath);
    return executeCommand(cmd);

#elif defined(OS_WINDOWS)
    // Use PowerShell to play system sounds
    std::string psCmd = "powershell -c \"" + soundPath + "\" > nul 2>&1";
    LOG_DEBUG("Playing sound on Windows: {}", soundPath);
    return executeCommand(psCmd);

#else
    LOG_WARN("Unsupported operating system for sound playback");
    return false;
#endif
}

bool SoundPlayer::beep() {
#ifdef OS_LINUX
    // ASCII BEL character or use beep command
    std::string cmd = "paplay /usr/share/sounds/freedesktop/stereo/bell.oga > /dev/null 2>&1 &";
    if (executeCommand(cmd)) {
        return true;
    }

    // Fallback: try beep command
    if (system("which beep > /dev/null 2>&1") == 0) {
        return executeCommand("beep > /dev/null 2>&1 &");
    }

    // Last resort: print bell character
    std::cout << "\a" << std::flush;
    return true;

#elif defined(OS_MACOS)
    std::string cmd = "afplay /System/Library/Sounds/Ping.aiff &";
    return executeCommand(cmd);

#elif defined(OS_WINDOWS)
    std::string cmd = "powershell -c \"[console]::beep(750,300)\" > nul 2>&1";
    return executeCommand(cmd);

#else
    std::cout << "\a" << std::flush;
    return true;
#endif
}

std::string SoundPlayer::getOSName() {
#ifdef OS_LINUX
    return "Linux";
#elif defined(OS_MACOS)
    return "macOS";
#elif defined(OS_WINDOWS)
    return "Windows";
#else
    return "Unknown";
#endif
}

bool SoundPlayer::executeCommand(const std::string& command) {
    int result = system(command.c_str());
    return result == 0;
}

std::string SoundPlayer::getSoundPath(SoundType type) {
#ifdef OS_LINUX
    // FreeDesktop sound theme paths
    const std::string soundBase = "/usr/share/sounds/freedesktop/stereo/";

    switch (type) {
        case SoundType::Success:
            return soundBase + "complete.oga";
        case SoundType::Error:
            return soundBase + "dialog-error.oga";
        case SoundType::Notify:
            return soundBase + "message.oga";
        case SoundType::Beep:
            return soundBase + "bell.oga";
        default:
            return "";
    }

#elif defined(OS_MACOS)
    // macOS system sounds
    const std::string soundBase = "/System/Library/Sounds/";

    switch (type) {
        case SoundType::Success:
            return soundBase + "Glass.aiff";
        case SoundType::Error:
            return soundBase + "Sosumi.aiff";
        case SoundType::Notify:
            return soundBase + "Tink.aiff";
        case SoundType::Beep:
            return soundBase + "Ping.aiff";
        default:
            return "";
    }

#elif defined(OS_WINDOWS)
    // Windows PowerShell SystemSounds
    switch (type) {
        case SoundType::Success:
            return "[System.Media.SystemSounds]::Exclamation.Play()";
        case SoundType::Error:
            return "[System.Media.SystemSounds]::Hand.Play()";
        case SoundType::Notify:
            return "[System.Media.SystemSounds]::Asterisk.Play()";
        case SoundType::Beep:
            return "[System.Media.SystemSounds]::Beep.Play()";
        default:
            return "";
    }

#else
    return "";
#endif
}
