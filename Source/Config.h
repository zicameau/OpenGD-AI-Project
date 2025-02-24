#pragma once
#include <cstdlib>
#include <string>

namespace Config {
    inline const char* getResourcesPath() {
        const char* envPath = std::getenv("OPENGD_RESOURCES");
        return envPath ? envPath : "/path/to/geometrydash";  // fallback path if env var not set
    }

    // Other configuration variables can go here
} 