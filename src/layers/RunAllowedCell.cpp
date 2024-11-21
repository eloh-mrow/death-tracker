#include "../layers/RunAllowedCell.hpp"

RunAllowedCell* RunAllowedCell::create(const int& Percent, const float& scale, const std::function<void(const int&, cocos2d::CCNode*)>& callback) {
    auto ret = new RunAllowedCell();
    if (ret && ret->init(Percent, scale, callback)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool RunAllowedCell::init(const int& Percent, const float& scale, const std::function<void(const int&, cocos2d::CCNode*)>& callback){

    buttonSprite = ButtonSprite::create((std::to_string(Percent) + "%").c_str(), "goldFont.fnt", "GJ_button_04.png", 1);
    buttonSprite->setScale(scale);
    if (!CCMenuItemSpriteExtra::init(buttonSprite, nullptr, this, menu_selector(RunAllowedCell::DeleteMe))) return false;

    m_Percent = Percent;
    m_Callback = callback;

    return true;
}

void RunAllowedCell::DeleteMe(CCObject*){
    if (m_Callback != NULL)
        m_Callback(m_Percent, this);
}
