#include "../layers/LabelSettingsLayer.hpp"
#include "../../managers/StatsManager.hpp"
#include "../cells/SpecialTextKeyCell.hpp"

LabelSettingsLayer* LabelSettingsLayer::create(LabelLayoutWindow* const& labelWin, DTLayer* const& dtLayer) {
    auto ret = new LabelSettingsLayer();
    if (ret && ret->initAnchored(325, 230, labelWin, dtLayer, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool LabelSettingsLayer::setup(LabelLayoutWindow* const& labelWin, DTLayer* const& dtLayer) {

    m_LabelWin = labelWin;
    m_DTLayer = dtLayer;

    this->setTitle("settings");

    m_closeBtn->setPositionX(m_size.width - 3.f);

    textPreviewWindow = CCScale9Sprite::create("square01_001.png");
    textPreviewWindow->setContentSize({200, 230});
    this->addChild(textPreviewWindow);

    labelWinPreview = CCScale9Sprite::create("GJ_squareB_01.png");
    labelWinPreview->setScale(0.5f);
    labelWinPreview->setContentSize(labelWin->s->getContentSize());
    if (labelWin->isNextToAnother())
        labelWinPreview->setContentWidth(labelWinPreview->getContentWidth() * 2);
    this->addChild(labelWinPreview);

    labelWinPreview->setColor({labelWin->m_MyLayout.color.r, labelWin->m_MyLayout.color.g, labelWin->m_MyLayout.color.b});
    labelWinPreview->setOpacity(labelWin->m_MyLayout.color.a);

    auto DeleteWinS = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    auto DeleteWin = CCMenuItemSpriteExtra::create(
            DeleteWinS,
            nullptr,
            this,
            menu_selector(LabelSettingsLayer::ConfirmDeletLabel)
    );
    DeleteWin->setPosition(m_size.width - 3.f, 3.f);
    this->m_buttonMenu->addChild(DeleteWin);

    auto infoButtonS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    auto infoButton = CCMenuItemSpriteExtra::create(
            infoButtonS,
            nullptr,
            this,
            menu_selector(LabelSettingsLayer::OnInfo)
    );
    infoButton->setPosition(6.f, m_size.height - 6.f);
    this->m_buttonMenu->addChild(infoButton);

    //-- text preview --

    previewScroll = geode::ScrollLayer::create(textPreviewWindow->getContentSize() - ccp(20, 20));
    previewScroll->setPosition({10, 10});
    textPreviewWindow->addChild(previewScroll);

    previewText = SimpleTextArea::create("", StatsManager::getFont(labelWin->m_MyLayout.font), labelWin->m_MyLayout.fontSize / 1.8f);
    previewText->setAnchorPoint({.5f, 1});
    previewText->setWrappingMode(WrappingMode::WORD_WRAP);
    previewText->setWidth(previewScroll->getContentWidth() - 20);
    previewScroll->m_contentLayer->addChild(previewText);

    previewText->setAlignment(m_LabelWin->m_MyLayout.alignment);
    previewText->setColor(m_LabelWin->m_MyLayout.color);
    LabelSettingsLayer::updatePreviewText();
    LabelSettingsLayer::updatePreviewTextPosition();

    previewScroll->moveToTop();

    //-- text input --

    auto TextInputLabel = CCLabelBMFont::create("Text:", "bigFont.fnt");
    TextInputLabel->setPosition({125, 182});
    TextInputLabel->setScale(.5f);
    m_mainLayer->addChild(TextInputLabel);

    labelTextInput = TextInput::create(210, "label text");
    labelTextInput->setPosition({125, 155});
    labelTextInput->setString(m_LabelWin->m_MyLayout.text);
    labelTextInput->setCallback([&](const std::string& text){
        m_LabelWin->m_MyLayout.text = text;

        LabelSettingsLayer::updatePreviewText();
    });
    labelTextInput->setCommonFilter(CommonFilter::Any);
    m_mainLayer->addChild(labelTextInput);

    //-- transition --

    LabelSettingsLayer::updateInputFields(StatsManager::getFont(m_LabelWin->m_MyLayout.font));

    LabelSettingsLayer::playEntryAnimation(m_LabelWin->getParent()->convertToWorldSpace(m_LabelWin->getPosition()) + m_LabelWin->getContentSize() / 2);

    //-- alignment --

    auto alignmentBG = CCScale9Sprite::create("square02_small.png");
    alignmentBG->setOpacity(100);
    alignmentBG->setPosition({122, 60});
    alignmentBG->setContentSize({175, 40});
    m_mainLayer->addChild(alignmentBG);

    auto alignLeftButtonSprite = CCSprite::create("alignLeft.png"_spr);
    alignLeftButton = CCMenuItemSpriteExtra::create(
        alignLeftButtonSprite,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::onAlign)
    );
    alignLeftButton->setPosition({60, 60});
    m_buttonMenu->addChild(alignLeftButton);

    auto alignCenterButtonSprite = CCSprite::create("alignCenter.png"_spr);
    alignCenterButton = CCMenuItemSpriteExtra::create(
        alignCenterButtonSprite,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::onAlign)
    );
    alignCenterButton->setPosition({122, 60});
    m_buttonMenu->addChild(alignCenterButton);

    auto alignRightButtonSprite = CCSprite::create("alignRight.png"_spr);
    alignRightButton = CCMenuItemSpriteExtra::create(
        alignRightButtonSprite,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::onAlign)
    );
    alignRightButton->setPosition({184, 60});
    m_buttonMenu->addChild(alignRightButton);

    onAlign(nullptr);

    //-- font size --

    auto FontSizeInputLabel = CCLabelBMFont::create("Font size:", "bigFont.fnt");
    FontSizeInputLabel->setPosition({125, 130});
    FontSizeInputLabel->setScale(.5f);
    m_mainLayer->addChild(FontSizeInputLabel);

    FontSizeInput = TextInput::create(90, "FSize");
    FontSizeInput->setPosition({125, 110});
    FontSizeInput->setScale(.75f);
    FontSizeInput->setString(fmt::format("{:.2f}", m_LabelWin->m_MyLayout.fontSize));
    FontSizeInput->setCallback([&](const std::string& text){
        auto fSize = geode::utils::numFromString<float>(text).unwrapOr(0);

        if (fSize < 0){
            fSize = 0;
            FontSizeInput->setString("0");
        }

        if (fSize > 2){
            fSize = 2;
            FontSizeInput->setString("2");
        }

        m_LabelWin->m_MyLayout.fontSize = fSize;
        FontSizeSlider->setValue(fSize / 2);
        FontSizeSlider->updateBar();

        previewText->setScale(m_LabelWin->m_MyLayout.fontSize / 1.8f);
        LabelSettingsLayer::updatePreviewTextPosition();

        previewScroll->moveToTop();
    });
    FontSizeInput->setCommonFilter(CommonFilter::Float);
    m_mainLayer->addChild(FontSizeInput);

    FontSizeSlider = Slider::create(this, menu_selector(LabelSettingsLayer::OnFontSizeSliderChanged));
    FontSizeSlider->setValue(m_LabelWin->m_MyLayout.fontSize / 2);
    FontSizeSlider->updateBar();
    FontSizeSlider->setScale(.7f);
    FontSizeSlider->setPosition(ccp(38, 40));
    m_mainLayer->addChild(FontSizeSlider);

    //-- color --

    auto colorBG = CCScale9Sprite::create("square02_small.png");
    colorBG->setOpacity(100);
    colorBG->setScale(0.75f);
    colorBG->setPosition({117, 27});
    colorBG->setContentSize({128, 31});
    m_mainLayer->addChild(colorBG);

    auto ColorLabel = CCLabelBMFont::create("Color:", "bigFont.fnt");
    ColorLabel->setPosition({100, 27});
    ColorLabel->setScale(.5f);
    m_mainLayer->addChild(ColorLabel);

    ColorSprite = ColorChannelSprite::create();
    ColorSprite->setColor({m_LabelWin->m_MyLayout.color.r, m_LabelWin->m_MyLayout.color.g, m_LabelWin->m_MyLayout.color.b});
    ColorSprite->setScale(0.65f);
    auto ColorPickerButton = CCMenuItemSpriteExtra::create(
        ColorSprite,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::OnColorClicked)
    );
    ColorPickerButton->setPosition({150, 27});
    m_buttonMenu->addChild(ColorPickerButton);

    //-- special text keys --

    auto STKLabel = CCLabelBMFont::create("Special text\nkeys:", "bigFont.fnt");
    STKLabel->setPosition({273.5f, 203});
    STKLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    STKLabel->setScale(.3f);
    m_mainLayer->addChild(STKLabel);

    std::map<std::string, std::string> stks{
        {"{f0}", "will include <cg>all runs from 0%</c>."},
        {"{runs}", "will include <cg>all</c> runs from <cl>practice</c> and <cy>startpos</c>."},
        {"{lvln}", "inserts the <cg>levels name</c>."},
        {"{att}", "will display your <cg>attempts</c> on the level, this will also combine all attempts from your <cy>linked</c> levels."},
        {"{s0}", "will include <co>all runs from 0</c> that are within your <cy>selected session</c>."},
        {"{sruns}", "will include <co>all</c> runs from <cl>practice</c> and <cg>startpos</c> that are within your <cy>selected session</c>."},
        {"{nl}", "inserts a <cg>new line</c> into the text."},
        {"{ptf0}", "inserts your <cr>approximated</c> playtime <cg>from 0</c>."},
        {"{ptrun}", "inserts your <cr>approximated</c> playtime <cg>from runs</c>."},
        {"{ptall}", "inserts your <cr>approximated</c> playtime <cg>in total</c>."},
        {"{ssd}", "will display the <co>date</c> in which your <cy>selected session</c> occurred.\n<cj>[month/day/year]</c>"},
        {"{sst}", "will display the <co>time</c> in which your <cy>selected session</c> occurred.\n<cj>[AM/PM]</c>"}
    };

    CCArray* specialTextKeys = CCArray::create();

    for (auto const& stk : stks)
    {
        specialTextKeys->addObject(SpecialTextKeyCell::create(stk.first, stk.second, std::bind(&LabelSettingsLayer::useSTK, this, std::placeholders::_1)));
    }

    auto STKListView = ListView::create(specialTextKeys, 20, 75, 80);
    STKListView->setPosition({236, 108});
    STKListView->setCellOpacity(40);
    m_mainLayer->addChild(STKListView);

    auto STKListOutline = CCScale9Sprite::create("GJ_square07.png");
    STKListOutline->setPosition(STKListView->getPosition() + STKListView->getContentSize() / 2);
    STKListOutline->setContentSize(STKListView->getContentSize() + ccp(5, 5));
    m_mainLayer->addChild(STKListOutline);

    //-- font --

    auto FontListLabel = CCLabelBMFont::create("Font:", "bigFont.fnt");
    FontListLabel->setPosition({259.5f, 97});
    FontListLabel->setScale(.4f);
    m_mainLayer->addChild(FontListLabel);

    CCArray* Fonts = CCArray::create();

    auto AllFonts = StatsManager::getAllFonts();

    for (int i = 0; i < AllFonts.size(); i++)
    {
        std::string fontName;

        if (i == 0)
            fontName = "Big Font";
        else if (i == 1)
            fontName = "Chat Font";
        else if (i == 2)
            fontName = "Gold Font";
        else{
            fontName = "Font " + std::to_string(i - 2);
        }

        Fonts->addObject(LabelFontCell::create(i, AllFonts[i], fontName, std::bind(&LabelSettingsLayer::setFont, this, std::placeholders::_1)));
    }

    auto FontListView = ListView::create(Fonts, 20, 85, 70);
    FontListView->setPosition({217, 17});
    FontListView->setCellOpacity(40);
    m_mainLayer->addChild(FontListView);

    auto fontListOutline = CCScale9Sprite::create("GJ_square07.png");
    fontListOutline->setPosition(FontListView->getPosition() + FontListView->getContentSize() / 2);
    fontListOutline->setContentSize(FontListView->getContentSize() + ccp(5, 5));
    m_mainLayer->addChild(fontListOutline);

    LabelSettingsLayer::removeTextBestColoring();
    scheduleUpdate();

    return true;
}

void LabelSettingsLayer::playEntryAnimation(const CCPoint& startingPoint){
    isTransitioning = true;

    auto winSize = CCDirector::get()->getWinSize();

    float transitionDuration = 0.2f;

    m_mainLayer->setPosition(startingPoint);
    textPreviewWindow->setPosition(startingPoint);
    labelWinPreview->setPosition(startingPoint);
    labelWinPreview->setOpacity(0);
    m_mainLayer->setScale(0);
    textPreviewWindow->setScale(0);
    this->setOpacity(0);
    windowTitleInput->getBGSprite()->setOpacity(0);
    GLubyte labelOpacityTo = windowTitleInput->getInputNode()->getPlaceholderLabel()->getOpacity();
    windowTitleInput->getInputNode()->getPlaceholderLabel()->setOpacity(0);

    this->runAction(CCSequence::create(CCEaseInOut::create(CCFadeTo::create(transitionDuration * 1.3f, 105), 2), CCCallFuncO::create(this, callfuncO_selector(LabelSettingsLayer::OnTransitionEnded), CCBool::create(true)), nullptr));
    labelWinPreview->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, m_LabelWin->m_MyLayout.color.a), 2));
    windowTitleInput->getBGSprite()->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, 90), 2));
    windowTitleInput->getInputNode()->getPlaceholderLabel()->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, labelOpacityTo), 2));

    m_mainLayer->runAction(CCEaseInOut::create(CCScaleTo::create(transitionDuration, 1), 2));
    textPreviewWindow->runAction(CCEaseInOut::create(CCScaleTo::create(transitionDuration, 1), 2));

    //569 : 320

    m_mainLayer->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, {winSize.width / 2 + 99.5f, 185}), 2));
    textPreviewWindow->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, {winSize.width / 2 - 174.5f, 185}), 2));
    labelWinPreview->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, {winSize.width / 2, 35}), 2));
}

void LabelSettingsLayer::onClose(cocos2d::CCObject*){
    if (!isTransitioning)
        LabelSettingsLayer::playClosingAnimation(m_LabelWin->getParent()->convertToWorldSpace(m_LabelWin->getPosition()) + m_LabelWin->getContentSize() / 2);
}

void LabelSettingsLayer::playClosingAnimation(const CCPoint& endPoint){
    isTransitioning = true;

    float transitionDuration = 0.2f;

    this->runAction(CCSequence::create(CCEaseInOut::create(CCFadeTo::create(transitionDuration * 1.3f, 0), 2), CCCallFuncO::create(this, callfuncO_selector(LabelSettingsLayer::OnTransitionEnded), CCBool::create(false)), nullptr));
    labelWinPreview->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, 0), 2));
    windowTitleInput->getBGSprite()->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, 0), 2));
    windowTitleInput->getInputNode()->getPlaceholderLabel()->runAction(CCEaseInOut::create(CCFadeTo::create(transitionDuration, 0), 2));

    m_mainLayer->runAction(CCEaseInOut::create(CCScaleTo::create(transitionDuration, 0), 2));
    textPreviewWindow->runAction(CCEaseInOut::create(CCScaleTo::create(transitionDuration, 0), 2));

    m_mainLayer->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, endPoint), 2));
    textPreviewWindow->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, endPoint), 2));
    labelWinPreview->runAction(CCEaseInOut::create(CCMoveTo::create(transitionDuration * 1.3f, endPoint), 2));


}

void LabelSettingsLayer::OnTransitionEnded(CCObject* transitionType){
    auto isEnter = static_cast<CCBool*>(transitionType);

    isTransitioning = false;

    if (!isEnter->getValue()){
        if (deleteOnExit){
            m_LabelWin->removeMeAndCleanup();

            m_LabelWin->m_DTLayer->changeScrollSizeByBoxes();
        }

        Popup<LabelLayoutWindow* const&, DTLayer* const&>::onClose(nullptr);
    }
}

void LabelSettingsLayer::updateInputFields(const std::string& font){
    if (windowTitleInput)
        windowTitleInput->removeMeAndCleanup();

    windowTitleInput = TextInput::create(labelWinPreview->getContentWidth() / 2  - 50, "Label name", font);
    windowTitleInput->setString(m_LabelWin->m_MyLayout.labelName);
    windowTitleInput->setZOrder(2);
    windowTitleInput->setScale(2);
    windowTitleInput->setPosition(labelWinPreview->getContentSize() / 2);
    windowTitleInput->setCallback([&](const std::string& text){
        m_LabelWin->m_MyLayout.labelName = text;
        m_LabelWin->m_Label->setText(text);
    });
    labelWinPreview->addChild(windowTitleInput);
}

void LabelSettingsLayer::onAlign(CCObject* sender){
    if (sender == alignLeftButton){
        m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentLeft;
    }
    else if (sender == alignRightButton){
        m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentRight;
    }
    else if (sender == alignCenterButton){
        m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentCenter;
    }

    alignLeftButton->setEnabled(true);
    alignLeftButton->setColor({255, 255, 255});
    alignRightButton->setEnabled(true);
    alignRightButton->setColor({255, 255, 255});
    alignCenterButton->setEnabled(true);
    alignCenterButton->setColor({255, 255, 255});

    switch (m_LabelWin->m_MyLayout.alignment)
    {
        case CCTextAlignment::kCCTextAlignmentLeft:
            alignLeftButton->setEnabled(false);
            alignLeftButton->setColor({140, 140, 140});
            break;
            
        case CCTextAlignment::kCCTextAlignmentRight:
            alignRightButton->setEnabled(false);
            alignRightButton->setColor({140, 140, 140});
            break;

        case CCTextAlignment::kCCTextAlignmentCenter:
            alignCenterButton->setEnabled(false);
            alignCenterButton->setColor({140, 140, 140});
            break;
    }

    previewText->setAlignment(m_LabelWin->m_MyLayout.alignment);
    LabelSettingsLayer::removeTextBestColoring();
}

void LabelSettingsLayer::OnFontSizeSliderChanged(CCObject*){
    FontSizeInput->setString(fmt::format("{:.2f}", FontSizeSlider->getValue() * 2));
    m_LabelWin->m_MyLayout.fontSize = FontSizeSlider->getValue() * 2;

    previewText->setScale(m_LabelWin->m_MyLayout.fontSize / 1.8f);
    LabelSettingsLayer::updatePreviewTextPosition();

    previewScroll->moveToTop();
}

void LabelSettingsLayer::OnColorClicked(CCObject*){
    auto colorPick = geode::ColorPickPopup::create(m_LabelWin->m_MyLayout.color);
    colorPick->show();
    colorPick->setColorTarget(ColorSprite);
    colorPick->setDelegate(this);
}

void LabelSettingsLayer::updateColor(cocos2d::ccColor4B const& color){
    m_LabelWin->m_MyLayout.color = color;
    m_LabelWin->s->setColor({color.r, color.g, color.b});
    m_LabelWin->s->setOpacity(color.a);
    labelWinPreview->setColor({color.r, color.g, color.b});
    labelWinPreview->setOpacity(color.a);

    previewText->setColor(m_LabelWin->m_MyLayout.color);
    LabelSettingsLayer::removeTextBestColoring();
}

void LabelSettingsLayer::setFont(const int& fontID){
    m_LabelWin->m_MyLayout.font = fontID;
    std::string font = StatsManager::getFont(fontID);

    LabelSettingsLayer::updateInputFields(font);

    if (m_LabelWin->m_Label->getFont() != font)
        m_LabelWin->m_Label->setFont(font);

    previewText->setFont(StatsManager::getFont(m_LabelWin->m_MyLayout.font));
    LabelSettingsLayer::updatePreviewTextPosition();
}

void LabelSettingsLayer::useSTK(const std::string& stk){
    #if !defined(GEODE_IS_ANDROID)
    labelTextInput->focus();
    #endif

    std::string labelText;
    if (!labelTextInput->getString().empty())
        labelText = labelTextInput->getString();

    int index = StatsManager::getCursorPosition(labelTextInput->getInputNode()->getPlaceholderLabel(), labelTextInput->getInputNode()->m_cursor);
    
    if (index < labelText.length())
        labelText.insert(index, stk);
    else if (index == labelText.length())
        labelText += stk;

    m_LabelWin->m_MyLayout.text = labelText;

    labelTextInput->setString(labelText);

    LabelSettingsLayer::updatePreviewText();
}

void LabelSettingsLayer::updatePreviewText(){
    
    auto modifiedText = m_DTLayer->modifyString(m_LabelWin->m_MyLayout.text);
    
    previewText->setText(modifiedText);

    LabelSettingsLayer::updatePreviewTextPosition();
}

void LabelSettingsLayer::update(float delta){
    previewText->setVisible(true);
}

void LabelSettingsLayer::updatePreviewTextPosition(){
    int height = previewText->getScaledContentHeight();

    if (previewScroll->getContentHeight() > height){
        previewScroll->m_contentLayer->setContentHeight(previewScroll->getContentHeight());
    }
    else{
        previewScroll->m_contentLayer->setContentHeight(height);
    }

    previewText->setPosition({previewScroll->m_contentLayer->getContentWidth() / 2, previewScroll->m_contentLayer->getContentHeight()});
    LabelSettingsLayer::removeTextBestColoring();
}

void LabelSettingsLayer::removeTextBestColoring(){
    for (int i = 0; i < previewText->getLines().size(); i++)
    {
        std::string s = previewText->getLines()[i]->getString();
        if (s != "<" && s.length() > 1){
            if (StatsManager::isKeyInIndex(s, 1, "nbc>")){
                s.erase(0, 5);
                previewText->getLines()[i]->setString(s.c_str());
                previewText->getLines()[i]->setColor(m_DTLayer->colorSpritenb->getColor());
            }
            if (StatsManager::isKeyInIndex(s, 1, "sbc>")){
                s.erase(0, 5);
                previewText->getLines()[i]->setString(s.c_str());
                previewText->getLines()[i]->setColor(m_DTLayer->colorSpritesb->getColor()); 
            }
        }
    }
}

void LabelSettingsLayer::ConfirmDeletLabel(CCObject*){
    deleteConfPopup = FLAlertLayer::create(this, "WARNING!", "are you sure you want to <cr>delete</c> this label?", "No", "Yes");
    deleteConfPopup->show();
}

void LabelSettingsLayer::FLAlert_Clicked(FLAlertLayer* p0, bool p1){
    if (deleteConfPopup == p0 && p1){
        LabelSettingsLayer::deleteLabel();
    }
}

void LabelSettingsLayer::deleteLabel(){
    for (int i = 0; i < m_LabelWin->m_DTLayer->m_LayoutLines.size(); i++)
    {
        if (m_LabelWin->m_DTLayer->m_LayoutLines[i] == m_LabelWin){
            m_LabelWin->m_DTLayer->m_LayoutLines.erase(std::next(m_LabelWin->m_DTLayer->m_LayoutLines.begin(), i));
            break;
        }
    }
    for (int i = 0; i < m_LabelWin->m_DTLayer->m_LayoutLines.size(); i++)
    {
        LabelLayoutWindow* curWin = dynamic_cast<LabelLayoutWindow*>(m_LabelWin->m_DTLayer->m_LayoutLines[i]);
        if (curWin){
            curWin->setPositionBasedOnLayout(curWin->m_MyLayout);
        }
    }

    deleteOnExit = true;

    LabelSettingsLayer::onClose(nullptr);
}

void LabelSettingsLayer::OnInfo(CCObject*){
    auto alert = FLAlertLayer::create(nullptr, "Help", fmt::format("{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}",
    "<cy>Text</c> - determines the text that will be displayed.",
    "<cg>Font size</c> - change the size of the text.",
    "<cs>Alignment</c> - change the text alignment [left, center, right].",
    "<cj>Color</c> - change the texts color. (new best colors are unaffected)",
    "<cr>Font</c> - change the font of the text.",
    "<cc>Special text keys</c> - allows you to add special things into your text! an explanation is available in each key.",
    "<ca>Text preview</c> - displays how the text should look in action :D",
    "<cl>Label preview</c> - this is the small box found at the bottom. displays how the label will look and allow you to change the labels name."
    ).c_str(), "Ok", nullptr, 450);
    alert->show();
}