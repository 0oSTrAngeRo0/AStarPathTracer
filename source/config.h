#ifndef PATHTRACER_CONFIG_H
#define PATHTRACER_CONFIG_H

#include <vector>

struct AppConfig {
public:
    int window_width;
    int window_height;
    const char *window_title;
    std::vector<const char*> external_extensions;
    bool enable_debug;
public:
    static AppConfig CreateDefault() {
        AppConfig config = AppConfig();
        config.window_height = 1024;
        config.window_width = 1024;
        config.window_title = "PathTracer";
        config.enable_debug = true;
        return config;
    }
};

#endif //PATHTRACER_CONFIG_H
