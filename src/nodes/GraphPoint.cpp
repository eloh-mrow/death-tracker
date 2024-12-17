#include "../nodes/GraphPoint.hpp"

GraphPoint* GraphPoint::create(const std::string& run, const float& passrate, const ccColor3B& color) {
    auto ret = new GraphPoint();
    if (ret && ret->init(run, passrate, color)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool GraphPoint::init(const std::string& run, const float& passrate, const ccColor3B& color){

    m_Run = run;
    m_Passrate = passrate;

    c = CCSprite::createWithSpriteFrameName("d_circle_02_001.png");
    c->setColor(color);
    c->setPosition(c->getContentSize());
    this->addChild(c);

    this->setContentSize(c->getContentSize() * 2);
    this->setAnchorPoint({0.5f, 0.5f});

    this->setEnabled(true);

    scheduleUpdate();

    return true;
}

void GraphPoint::update(float delta){
    if (m_bSelected){
        c->setScale(2);
        if (!m_oneTimeCall){
            m_oneTimeCall = true;
            m_Delegate->OnPointSelected(this);
        }
    }
    else{
        c->setScale(1);
        if (m_oneTimeCall){
            m_oneTimeCall = false;
            m_Delegate->OnPointDeselected(this);
        }
    }
}

void GraphPoint::setDelegate(GraphPointDelegate* Delegate){
    m_Delegate = Delegate;
}