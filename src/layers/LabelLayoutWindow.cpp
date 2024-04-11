#include "../layers/LabelLayoutWindow.hpp"
#include "../managers/StatsManager.hpp"
#include "../layers/LabelSettingsLayer.hpp"

LabelLayoutWindow* LabelLayoutWindow::create(LabelLayout MyLayout, DTLayer* DTLayer) {
    auto ret = new LabelLayoutWindow();
    if (ret && ret->init(MyLayout, DTLayer)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool LabelLayoutWindow::init(LabelLayout MyLayout, DTLayer* DTLayer){

    s = CCScale9Sprite::create("GJ_squareB_01.png");
    s->setScale(0.5f);
    this->addChild(s);

    m_MyLayout = MyLayout;

    m_DTLayer = DTLayer;

    s->setColor({m_MyLayout.color.r, m_MyLayout.color.g, m_MyLayout.color.b});
    s->setOpacity(m_MyLayout.color.a);

    m_Label = SimpleTextArea::create(m_MyLayout.labelName.c_str(), StatsManager::getFont(MyLayout.font));
    m_Label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_Label->setZOrder(2);
    this->addChild(m_Label);

    schedule(schedule_selector(LabelLayoutWindow::myUpdate));

    return true;
}

CCPoint LabelLayoutWindow::mousePosToNode(CCNode* node){
    if (!m_DTLayer->ClickPos) return {0, 0};
    CCPoint mousePos = m_DTLayer->ClickPos->getLocation();

    bool HasreachedEndOfParentCheck = false;

    CCNode* LastParent = node->getParent();

    CCNode* Scene = nullptr;

    while(!HasreachedEndOfParentCheck){

        if (LastParent != nullptr){
            mousePos = ccp(mousePos.x - LastParent->getPositionX(), mousePos.y - LastParent->getPositionY());

            Scene = LastParent;
            LastParent = LastParent->getParent();
        }
        else {
            HasreachedEndOfParentCheck = true;
        }
    }

    return mousePos;
}

void LabelLayoutWindow::myUpdate(float delta){
    if (m_FollowMouse){
        this->setPosition(mousePosToNode(this) - s->getScaledContentSize() / 2);
    }

    if ((m_MyLayout.color.r + m_MyLayout.color.g + m_MyLayout.color.b) / 3 > 200)
        m_Label->setColor({ 136, 136, 136, 255});
    else
        m_Label->setColor({255, 255, 255, 255});

    if (m_DoubleClickTimer > 0)
        m_DoubleClickTimer -= delta;

    if (this->getParent()->isVisible())
        if (isMouseTouching(this) && !m_Lock){
            if (!m_DTLayer->m_IsMovingAWindow){
                if (m_DoubleClickTimer <= 0){
                    m_DoubleClickTimer = 0.3f;
                }
                else{
                    auto Settings = LabelSettingsLayer::create(this);
                    m_DTLayer->addChild(Settings);
                }

                m_DTLayer->m_IsMovingAWindow = true;
                m_FollowMouse = true;
                m_DTLayer->m_ScrollLayer->setTouchEnabled(false);
                m_MyLayoutSave = m_MyLayout;
                this->setZOrder(2);

                int lineToUpdate = m_MyLayout.line;

                m_MyLayout.line = -1;
                m_MyLayout.position = -1;

                updateLine(lineToUpdate);
            }
        }
        else{
            m_Lock = m_DTLayer->m_IsClicking;
            if (m_FollowMouse){
                m_DTLayer->m_IsMovingAWindow = false;
                m_FollowMouse = false;
                auto positioning = getLineByPos(this->getPosition() + s->getScaledContentSize() / 2);
                this->setZOrder(0);
                if (positioning.first != -1){
                    m_MyLayout.line = positioning.first;
                    m_MyLayout.position = positioning.second;
                    
                    setPositionBasedOnLayout(m_MyLayout);
                }
                else{
                    m_MyLayout.line = m_MyLayoutSave.line;
                    m_MyLayout.position = m_MyLayoutSave.position;
                    setPositionBasedOnLayout(m_MyLayout);
                }

                m_DTLayer->changeScrollSizeByBoxes();
                
                m_DTLayer->m_ScrollLayer->setTouchEnabled(true);
            }
        }
}

void LabelLayoutWindow::setPositionBasedOnLayout(LabelLayout layout, int d){
    s->setPosition({0, 0});
    this->setPosition({0, 0});
    s->setContentSize({323 / s->getScale() / 1.05f, s->getContentSize().height});

    this->setPositionY(-(s->getContentSize().height / 2 * s->getScale()) - 5 - (s->getContentSize().height * s->getScale()) * layout.line);

    for (int i = 0; i < m_DTLayer->m_LayoutLines.size(); i++)
    {
        auto IWindow = static_cast<LabelLayoutWindow*>(m_DTLayer->m_LayoutLines[i]);
        if (IWindow != this)
            if (IWindow->m_MyLayout.line == layout.line){
                if (layout.position == IWindow->m_MyLayout.position)
                {
                    if (layout.position == 1){
                        IWindow->m_MyLayout.position = 0;
                    }
                    if (layout.position == 0){
                        IWindow->m_MyLayout.position = 1;
                    }
                    
                }
                if (d == 0){
                    d++;
                    IWindow->setPositionBasedOnLayout(IWindow->m_MyLayout, d);
                }
                
                    
                float moveBy = s->getContentSize().width * s->getScale() / 4;
                s->setContentSize({s->getContentSize().width / 2, s->getContentSize().height});
                if (layout.position == 0){
                    this->setPositionX(-moveBy);
                }
                else{
                    this->setPositionX(moveBy);
                }
            }
    }

    s->setPosition(s->getPosition() + s->getScaledContentSize() / 2);
    m_Label->setPosition(s->getPosition());
    this->setPosition({this->getPositionX() - s->getScaledContentSize().width / 2, this->getPositionY() - s->getScaledContentSize().height / 2});
    this->setContentSize(s->getScaledContentSize());
}

bool LabelLayoutWindow::isMouseTouching(CCNode* node){
    bool toReturn = false;
    CCRect* rect = new CCRect(node->getPositionX(), node->getPositionY(), node->getScaledContentSize().width, node->getScaledContentSize().height);
    toReturn = rect->containsPoint(mousePosToNode(node)) && m_DTLayer->m_IsClicking;
    delete rect;

    return toReturn;
}

std::pair<int, int> LabelLayoutWindow::getLineByPos(CCPoint pos){
    int linesAmount = (m_DTLayer->m_ScrollLayer->m_contentLayer->getScaledContentSize().height / this->getScaledContentSize().height) + 1;

    int lineChosen = 0;

    pos.y = abs(pos.y);

    for (int i = 0; i < linesAmount; i++)
    {
        if (pos.y >= this->getScaledContentSize().height * i && pos.y < this->getScaledContentSize().height * (i + 1)){
            lineChosen = i;
        }
        if (i == linesAmount - 1)
            if (pos.y >= this->getScaledContentSize().height * i){
                lineChosen = i;
            }
        if (i == 0)
            if (pos.y < this->getScaledContentSize().height * i){
                lineChosen = i;
            }
    }

    int positionChosen = 0;

    if (pos.x >= 0)
        positionChosen = 1;

    int amountInExistingLine = 0;

    for (int i = 0; i < m_DTLayer->m_LayoutLines.size(); i++)
    {
        auto IWindow = static_cast<LabelLayoutWindow*>(m_DTLayer->m_LayoutLines[i]);
        if (IWindow->m_MyLayout.line == lineChosen){
            amountInExistingLine++;
            if (amountInExistingLine == 2){
                return std::pair<int, int>{-1, -1};
            }
        }
    }
    
    return std::pair<int, int>{lineChosen, positionChosen};
}

void LabelLayoutWindow::updateLine(int line){
    for (int i = 0; i < m_DTLayer->m_LayoutLines.size(); i++)
    {
        auto winToCheck = static_cast<LabelLayoutWindow*>(m_DTLayer->m_LayoutLines[i]);
        if (winToCheck->m_MyLayout.line == line){
            winToCheck->setPositionBasedOnLayout(winToCheck->m_MyLayout);
        }
    }
}