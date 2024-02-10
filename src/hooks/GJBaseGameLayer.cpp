#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../utils/SaveManager.hpp"

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
	TodoReturn setStartPosObject(StartPosObject* startpos) {
		GJBaseGameLayer::setStartPosObject(startpos);
		SaveManager::setUsingStartpos(startpos != nullptr);
	}
};