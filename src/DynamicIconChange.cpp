#include "DynamicIconChange.hpp"

IconLimits* SettingsManager::getIconLimits() {
    IconLimits* il = new IconLimits();

    for (int i = 0; i <= 7; i++) {
		// init min-max range icons
		int indexMin = i; int indexMax = i;

		if (indexMin != 0) indexMin = indexMin * 2;
		indexMax = indexMin + 1;

		il->iconMin[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMin]);
		il->iconMax[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMax]);
	}

    return il;
}

void GUIManager::createAlertLabel(PlayLayer* pl) {
    auto alertLabel = CCLabelBMFont::create(
		"DIC mod was disabled due to incorrectly exposed icon range!",
		"bigFont.fnt"
	);
	alertLabel->setScale(0.4f);
	alertLabel->runAction(CCFadeOut::create(6.f));

	CCPoint winSize = CCDirector::get()->getWinSize();
	alertLabel->setPosition(
		winSize.x / 2.f, 100.f
	);

	pl->addChild(alertLabel);
}

void GUIManager::onFlippedToggler(CCObject* sender) {
    auto togger = static_cast<CCMenuItemToggler*>(sender);
    if (togger->getTag() != 69) return;

    togger->toggle(togger->isToggled());

    GameManager* gmPtr = GameManager::get();
    DynamicIconChange* dic = DynamicIconChange::get();
    bool globalStatusMod = dic->getGlobalModStatus();
    log::debug("global status {}; togger status {}", globalStatusMod, togger->isToggled());

    if (!togger->isToggled()) {
        SettingsManager::setGlobalStatusMod(true);

        if (dic->getIconLimits() == nullptr) dic->initMod();

        dic->enableMod(nullptr);
    } else {
        std::pair<PlayerObject*, PlayerObject*> POs;
        std::pair<int, int>                     activeModes;

        for (int i = 0; i <= 1; i++) {
            PlayerObject* player = i == 0 ? gmPtr->m_playLayer->m_player1 : 
                                            gmPtr->m_playLayer->m_player2;
            int activeMode = 0;

            if      (player->m_isShip)   activeMode = 1;
            else if (player->m_isBall)   activeMode = 2;
            else if (player->m_isBird)   activeMode = 3;
            else if (player->m_isDart)   activeMode = 4;
            else if (player->m_isRobot)  activeMode = 5;
            else if (player->m_isSpider) activeMode = 6;
            else if (player->m_isSwing)  activeMode = 7;
            
            if (i == 0) {
                POs.first = player; activeModes.first = activeMode;
            } else {
                POs.second = player; activeModes.second = activeMode;
            }
        }

        dic->disableModInGame(POs, activeModes);

        SettingsManager::setGlobalStatusMod(false);
    }
}

DynamicIconChange* DynamicIconChange::get() {
    static DynamicIconChange* instance = nullptr;

    if (instance == nullptr) {
        instance = new DynamicIconChange;
        return instance;
    } else {
        return instance;
    }
}

void DynamicIconChange::initInstance(DynamicIconChange* instance) {
    if (DynamicIconChange::initClass) return;

    instance->modStatus = false;
    instance->wrongIconRange = false;
    instance->globalModStatus = SettingsManager::getGlobalStatusMod();
    instance->unlockIcons = SettingsManager::getUnlockIcons();
    instance->iconList = nullptr;
    instance->im = new IconManager;
    instance->il = nullptr;
    instance->iconList = nullptr;
    instance->inLevel = false;

    DynamicIconChange::initClass = true;
}

void DynamicIconChange::initMod() {
    if (il != nullptr) delete il;  // solving a memory leak

    il = SettingsManager::getIconLimits();

    if (!this->validateLimits()) {
        this->wrongIconRange = true;
        return;
    }

    if (this->wrongIconRange) this->wrongIconRange = false;  // disable wIR if validation was successful

    if (this->unlockIcons) 
        if (!this->generateIconList()) {
            this->wrongIconRange = true;
            return;
        }

    log::debug("INIT MOD");
}

void DynamicIconChange::enableMod(PlayLayer* pl) {
    if (!this->globalModStatus) return;

    if (this->wrongIconRange) {
        SettingsManager::setGlobalStatusMod(false);

        if (pl != nullptr)
            GUIManager::createAlertLabel(pl);
            
        return;
    }

    modStatus = true;

    log::debug("ENABLE MOD");
}

void DynamicIconChange::disableMod() {
    if (!this->globalModStatus) return;

    modStatus = false;

    log::debug("DISABLE MOD");
}

void DynamicIconChange::disableModInGame(
    std::pair<PlayerObject*, PlayerObject*>& po, 
    std::pair<int, int>& activeMode
) {
    log::debug("FIRST STATE - DISABLE MOD IN GAME");
    log::debug("! {} !", this->globalModStatus);
    if (!this->globalModStatus) return;
    log::debug("TWO STATE - DISABLE MOD IN GAME");

    modStatus = false;

    for (int i = 0; i <= 1; i++) {
        auto _po          = i == 0 ? po.first         : po.second;
        auto _activeMode  = i == 0 ? activeMode.first : activeMode.second;

        this->im->loadAndUpdateIconKit(_po, _activeMode);
    }

    log::debug("FINAL STATE - DISABLE MOD IN GAME");
}

bool DynamicIconChange::validateLimits() {
    for (int i = 0; i <= 7; i++) {
		if (il->iconMin[i] > il->iconMax[i]) {
            log::debug("VALIDATE FAIL");
            return false;
        }
	}

    log::debug("VALIDATE DONE");
	return true;
}

int DynamicIconChange::generateRandIcon(int gamemodeId) {
    if (this->unlockIcons)  // index on array
        return (*(*iconList)[gamemodeId])[(this->gen() % (*iconList)[gamemodeId]->size())];  // I love pointers UwU
    else                    // icon id
        return (this->gen() % (il->iconMax[gamemodeId] - il->iconMin[gamemodeId] + 1)) + il->iconMin[gamemodeId];
}

void DynamicIconChange::changeMode(
    PlayerObject* po, int gamemodeId, bool p0, bool p1
) {
    if (!this->modStatus) return;

    if ( this->im->getPlayerStatus(true)->pObjOwner == nullptr ) this->im->initPlayerStatus();

    if (!p0 && !p1) {
        if (po->m_isShip || po->m_isBall || po->m_isBird || po->m_isDart || 
		    po->m_isRobot || po->m_isSpider || po->m_isSwing) return;  // if gamemode is not a cube
        
        gamemodeId = 0;
    }

    if ((po->m_isShip || po->m_isBird) && gamemodeId != 0)  // change cube icon in ship/ufo
        this->changeMode(po, 0, p0, p1);

    int iconId = this->generateRandIcon(gamemodeId);

    if (po == nullptr) im->setIcon(nullptr, gamemodeId, iconId, false);
    else               im->setAndUpdateIcon(po, gamemodeId, iconId, false);
}

void DynamicIconChange::setMiniMode(PlayerObject* po, bool status) {
    if ( this->im->getPlayerStatus(true)->pObjOwner == nullptr && this->inLevel ) this->im->initPlayerStatus();
    
    bool isFirstPlayer = po == this->im->getPlayerObject(true);
    PlayerStatus* ps = this->im->getPlayerStatus(isFirstPlayer);
    ps->isMiniMode = status;

    log::debug("is first player - {}, mini mode - {}", isFirstPlayer, this->im->getPlayerStatus(isFirstPlayer)->isMiniMode);

    if ( !this->modStatus ) return;

    if (!status)    // update cube and ball after resize
        if (po->m_isBall) 
            this->im->updateIcon(po, 2, ps->iconKit[2], false);

        else if (!po->m_isShip && !po->m_isBall && !po->m_isBird && !po->m_isDart &&  // robtop is gay!
		        !po->m_isRobot && !po->m_isSpider && !po->m_isSwing)                  // if is a cube
            this->im->updateIcon(po, 0, ps->iconKit[0], false);
}

bool DynamicIconChange::generateIconList() {
    if (this->iconList == nullptr) this->iconList = new std::vector<std::vector<int>*>;
    else                           this->iconList->clear();

    for (int i = 0; i <= 7; i++) {
        auto iv = this->im->getUnlockIcons(i);

        iv->erase(  // range icons
            std::remove_if(
                iv->begin(), iv->end(),
                [&](int const& j) { 
                    return j > this->il->iconMax[i] || j < this->il->iconMin[i]; 
                }
            ), 
            iv->end()
        );

        if (iv->size() == 0) return false;

        this->iconList->push_back(iv);
    }

    return true;
}
