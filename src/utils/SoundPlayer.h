#pragma once

#include <string>

/**
 * Cross-platform sound player utility
 * Supports playing system sounds on Linux, macOS, and Windows
 */
class SoundPlayer {
public:
    enum class SoundType {
        Success,    // Success/completion sound
        Error,      // Error/failure sound
        Notify,     // Notification/alert sound
        Beep        // Simple beep
    };

    /**
     * Play a system sound based on the platform
     * @param type The type of sound to play
     * @return true if sound was played successfully, false otherwise
     */
    static bool playSound(SoundType type);

    /**
     * Play a simple beep sound
     * @return true if beep was played successfully, false otherwise
     */
    static bool beep();

    /**
     * Get the current operating system name
     * @return "Linux", "macOS", "Windows", or "Unknown"
     */
    static std::string getOSName();

private:
    // Execute a system command to play sound
    static bool executeCommand(const std::string& command);

    // Get sound file path for the given type
    static std::string getSoundPath(SoundType type);
};
