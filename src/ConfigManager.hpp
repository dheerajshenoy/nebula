#pragma once

class ConfigManager {

public:
    static ConfigManager& instance();
    void loadConfig(const std::string &path) noexcept;

    template <typename T>
    T get(const std::string &key, const T& defaultValue = T()) const noexcept;

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManger& operator=(const ConfigManager &) = delete;
    sol::state lua;
    std::unordered_map<std::string, sol::object> settings;

};
