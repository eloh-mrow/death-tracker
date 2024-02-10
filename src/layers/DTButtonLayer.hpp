#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DTButtonLayer : public CCLayer {
protected:
	bool init();

public:
	static DTButtonLayer* create();
	void onOpenDTPopup(CCObject* sender);
};