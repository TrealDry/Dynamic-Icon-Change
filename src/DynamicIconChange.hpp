#pragma once

#include <random>
#include <string>

#include "includes.hpp"
#include "IconManager.hpp"

struct IconLimits {

    int* iconMin;
    int* iconMax;

    IconLimits() : iconMin(new int[8]), iconMax(new int[8]) {};
    IconLimits(int* iMin, int* iMax) : iconMin(iMin), iconMax(iMax) {};

    ~IconLimits() { delete[] iconMin; delete[] iconMax; };

};

class SettingsManager {

public:
    static inline std::string_view nameRangeSettings[16] = {
        "min-range-cube", "max-range-cube", "min-range-ship", "max-range-ship", 
        "min-range-ball", "max-range-ball", "min-range-ufo", "max-range-ufo", 
        "min-range-wave", "max-range-wave", "min-range-robot", "max-range-robot",
        "min-range-spider", "max-range-spider", "min-range-swing", "max-range-swing",
    };

public:
    SettingsManager() = delete;

    static bool getGlobalStatusMod()            { return Mod::get()->getSettingValue<bool>("disable-mod"); }
    static void setGlobalStatusMod(bool status) { Mod::get()->setSettingValue("disable-mod", status); }

    static IconLimits* getIconLimits();

};

class GUIManager {

public:
    GUIManager() = delete;

    static void createAlertLabel(PlayLayer* pl);

    static void onFlippedToggler(CCObject* sender);

};

class DynamicIconChange {

    inline static bool initClass = false;

    bool modStatus;
    bool globalModStatus;

    IconManager* im;
    IconLimits* il;

    std::mt19937 gen;
    std::random_device rd;

public:
    DynamicIconChange() : gen(rd()) {};
    ~DynamicIconChange() { delete il; delete im; DynamicIconChange::initClass = false; };

    static DynamicIconChange* get();
    static void initInstance(DynamicIconChange* instance);

    void initMod();

    void enableMod();
    void disableMod();
    void disableModInGame(PlayerObject* po, int activeMode);

    bool validateLimits();

    inline int generateRandIcon(int gamemodeId);
    void changeMode(
        PlayerObject* po, int gamemodeId, bool p0, bool p1  // player0 and player1
    );

    bool& getModStatus()          { return modStatus; }
    bool& getGlobalModStatus()    { return globalModStatus; }
    IconManager* getIconManager() { return im; }
    IconLimits* getIconLimits()   { return il; }

};