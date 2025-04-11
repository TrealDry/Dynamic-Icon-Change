#pragma once

#include <random>
#include <string>
#include <utility>

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

    static bool getGlobalStatusMod()            { return !Mod::get()->getSettingValue<bool>("disable-mod"); }
    static void setGlobalStatusMod(bool status) { Mod::get()->setSettingValue("disable-mod", !status); }

    static bool getUnlockIcons()                { return Mod::get()->getSettingValue<bool>("unlock-icons"); }

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
    bool unlockIcons;

    bool wrongIconRange;

    bool inLevel;

    IconManager* im;
    IconLimits* il;

    std::vector<std::vector<int>*>* iconList;

    std::mt19937 gen;
    std::random_device rd;

public:
    DynamicIconChange()  { gen.seed(rd()); };
    ~DynamicIconChange() { delete il; delete im; delete iconList; DynamicIconChange::initClass = false; };

    static DynamicIconChange* get();
    static void initInstance(DynamicIconChange* instance);

    void initMod();

    void enableMod(PlayLayer* pl);
    void disableMod();
    void disableModInGame(
        std::pair<PlayerObject*, PlayerObject*>& po, 
        std::pair<int, int>& activeMode
    );

    bool validateLimits();

    int generateRandIcon(int gamemodeId);
    void changeMode(
        PlayerObject* po, int gamemodeId, bool p0, bool p1  // player0 and player1
    );
    void setMiniMode(PlayerObject* po, bool status);

    bool generateIconList();

    bool getModStatus()           { return modStatus; }
    bool getGlobalModStatus()     { return globalModStatus; }
    bool getUnlockIcons()         { return unlockIcons; }
    bool getWrongIconRange()      { return wrongIconRange; }
    bool getInLevel()             { return inLevel; }

    IconManager* getIconManager() { return im; }
    IconLimits* getIconLimits()   { return il; }

    void setGlobalModStatus(bool status) { globalModStatus = status; }
    void setUnlockIcons(bool status)     { unlockIcons = status; }
    void setInLevel(bool status)         { inLevel = status; }

};