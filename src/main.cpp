#include "includes.hpp"

#include "DynamicIconChange.hpp"

#include <Geode/loader/SettingV3.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

static DynamicIconChange* dic = DynamicIconChange::get();

void initDic() {
	DynamicIconChange::initInstance(dic);
}

$execute {
	initDic();

	listenForSettingChanges<bool>(
		"disable-mod", 
		[](bool value) {
			dic->setGlobalModStatus(!value);
		}
	);

	listenForSettingChanges<bool>(
		"icons-mod", 
		[](bool value) {
			dic->setUnlockIcons(value);
		}
	);
}

class $modify(MyPlayLayer, PlayLayer) {

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!dic->getModStatus()) {
			dic->getIconManager()->saveIconKit();
			dic->initMod();
			dic->setInLevel(true);
			dic->enableMod(this);
		}

		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void onQuit() {
		dic->getIconManager()->loadIconKit();
		dic->getIconManager()->clearPlayerStatus();
		dic->setInLevel(false);
		dic->disableMod();

        PlayLayer::onQuit();
    }

};

class $modify(MyPlayerObject, PlayerObject) {

	void togglePlayerScale(bool p0, bool p1) {
		dic->setMiniMode(this, p0);
		PlayerObject::togglePlayerScale(p0, p1);
	}

	void toggleFlyMode(bool p0, bool p1) {
		dic->changeMode(this, 1, p0, p1);
		PlayerObject::toggleFlyMode(p0, p1);
	}

	void toggleRollMode(bool p0, bool p1) {
		dic->changeMode(this, 2, p0, p1);
		PlayerObject::toggleRollMode(p0, p1);
	}

	void toggleBirdMode(bool p0, bool p1) {
		dic->changeMode(this, 3, p0, p1);
		PlayerObject::toggleBirdMode(p0, p1);
	}

	void toggleDartMode(bool p0, bool p1) {
		dic->changeMode(this, 4, p0, p1);
		PlayerObject::toggleDartMode(p0, p1);
	}

	void toggleRobotMode(bool p0, bool p1) {
		dic->changeMode(this, 5, p0, p1);
		PlayerObject::toggleRobotMode(p0, p1);
	}

	void toggleSpiderMode(bool p0, bool p1) {
		dic->changeMode(this, 6, p0, p1);
		PlayerObject::toggleSpiderMode(p0, p1);
	}

	void toggleSwingMode(bool p0, bool p1) {
		dic->changeMode(this, 7, p0, p1);
		PlayerObject::toggleSwingMode(p0, p1);
	}
};

class $modify(MyPauseLayer, PauseLayer) {

	void customSetup() {  // гайс, я в плюсах зелёный, как перенести MyPauseLayer в GUIManager?
		PauseLayer::customSetup();
		
		auto enableModToggler = CCMenuItemToggler::createWithStandardSprites(
			this, menu_selector(MyPauseLayer::onFlippedToggler), 1.f
		);
		enableModToggler->setTag(69);

		if (dic->getWrongIconRange())
			enableModToggler->setEnabled(false);
		else
			enableModToggler->toggle(dic->getGlobalModStatus());
		
		auto dicText = CCLabelBMFont::create(
			"DIC", "bigFont.fnt"
		);
		dicText->setPosition(18.f, 27.f);
		dicText->setScale(0.25f);
		enableModToggler->addChild(dicText);

		this->getChildByID("right-button-menu")->addChild(enableModToggler);
		this->getChildByID("right-button-menu")->updateLayout();
	}

public:
	void onFlippedToggler(CCObject* sender) {
		GUIManager::onFlippedToggler(sender);
	}
};

class $modify(MyLevelEditorLayer, LevelEditorLayer) {

	bool init(GJGameLevel* p0, bool p1) {
		if (dic->getModStatus()) {
			dic->getIconManager()->loadIconKit();
			dic->getIconManager()->clearPlayerStatus();
			dic->setInLevel(false);
			dic->disableMod();
		}

		return LevelEditorLayer::init(p0, p1);
	}

};

class $modify(MyAppDelegate, AppDelegate) {

	void trySaveGame(bool p0) {
		/*
			loads original icons if game is 
			switched off (when mod ends incorrectly).
		*/

		if (dic->getModStatus()) 
			dic->getIconManager()->loadIconKit();

		AppDelegate::trySaveGame(p0);
	}
};
