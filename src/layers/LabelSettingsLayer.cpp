#include "../layers/LabelSettingsLayer.hpp"
#include "../managers/StatsManager.hpp"
#include "../hooks/CCControlColourPickerBypass.h"

LabelSettingsLayer* LabelSettingsLayer::create(LabelLayoutWindow* const& labelWin) {
    auto ret = new LabelSettingsLayer();
    if (ret && ret->init(280, 280, labelWin, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool LabelSettingsLayer::setup(LabelLayoutWindow* const& labelWin) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    m_LabelWin = labelWin;

    this->setZOrder(100);

    this->setTitle("Label Settings");

    auto overallInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    overallInfoBS->setScale(0.8f);
    auto overallInfoButton = CCMenuItemSpriteExtra::create(
        overallInfoBS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::onOverallInfo)
    );
    overallInfoButton->setPosition({111, 118});
    this->m_buttonMenu->addChild(overallInfoButton);

    auto specialInfoBS = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    specialInfoBS->setScale(0.55f);
    auto specialInfoButton = CCMenuItemSpriteExtra::create(
        specialInfoBS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::onSpecialInfo)
    );
    specialInfoButton->setPosition({29, 46});
    this->m_buttonMenu->addChild(specialInfoButton);

    alighmentNode = CCNode::create();
    alighmentNode->setPosition(m_buttonMenu->getPosition());
    m_mainLayer->addChild(alighmentNode);

    auto allCont = CCNode::create();
    allCont->setPosition({-286, -157});
    alighmentNode->addChild(allCont);

    /*
    -- settings that need changing -- 

    - label name Y
    - text Y
    - color Y
    - alignment Y
    - font Y
    - font size X

    -- other --

    - text preview X
    
    */

   auto DeleteWinS = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
   auto DeleteWin = CCMenuItemSpriteExtra::create(
        DeleteWinS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::deleteLabel)
   );
   DeleteWin->setPosition(m_size.width / 2 + 3.f, m_size.height / 2 - 3.f);
   this->m_buttonMenu->addChild(DeleteWin);

    //label name

    CCNode* LabelNameCont = CCNode::create();
    LabelNameCont->setID("Label-Name-Container");
    LabelNameCont->setPosition({228, 228});
    allCont->addChild(LabelNameCont);

    auto LabelNameInputLabel = CCLabelBMFont::create("Label Name", "bigFont.fnt");
    LabelNameInputLabel->setPosition({0, 23});
    LabelNameInputLabel->setScale(.55f);
    LabelNameCont->addChild(LabelNameInputLabel);

    m_LabelNameInput = InputNode::create(150, "");
    m_LabelNameInput->setPosition({0, 0});
    m_LabelNameInput->setScale(0.8f);
    m_LabelNameInput->setString(m_LabelWin->m_MyLayout.labelName);
    m_LabelNameInput->getInput()->setDelegate(this);
    m_LabelNameInput->getInput()->setAllowedChars("abcdefghijkmlnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789!@#$%^&*(){}[]\"\"/-_=+'?;:><,.`~|");
    LabelNameCont->addChild(m_LabelNameInput);

    //alighment

    CCNode* AlighmentCont = CCNode::create();
    AlighmentCont->setID("Alighment-Container");
    AlighmentCont->setPosition({351, 231});
    AlighmentCont->setScale(1.3f);
    allCont->addChild(AlighmentCont);

    auto AlighmentMenu = CCMenu::create();
    AlighmentMenu->setPosition({-5,-1});
    AlighmentCont->addChild(AlighmentMenu);

    m_Alighment = CCLabelBMFont::create(AlignmentToSring(m_LabelWin->m_MyLayout.alignment).c_str(), "bigFont.fnt");
    m_Alighment->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_Alighment->setPosition({0, -1});
    m_Alighment->setScale(0.475f);
    m_Alighment->setZOrder(1);
    AlighmentCont->addChild(m_Alighment);

    CCLabelBMFont* aligmentLabel = CCLabelBMFont::create("Text Alignment", "bigFont.fnt");
    aligmentLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    aligmentLabel->setPosition({0, 14});
    aligmentLabel->setScale(0.3f);
    AlighmentCont->addChild(aligmentLabel);

    CCScale9Sprite* aligmentTABG = CCScale9Sprite::create("square02_small.png", {0,0, 40, 40});
    aligmentTABG->setPosition({0, -2});
    aligmentTABG->setScale(0.525f);
    aligmentTABG->setContentSize({114, 27});
    aligmentTABG->setOpacity(100);
    AlighmentCont->addChild(aligmentTABG);

    auto changeAlignmentRightS = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    changeAlignmentRightS->setScaleX(0.35f);
    changeAlignmentRightS->setScaleY(0.2f);
    auto changeAlignmentRight = CCMenuItemSpriteExtra::create(
        changeAlignmentRightS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::ChangeAlignmentRight)
    );
    changeAlignmentRight->setPosition({42, -0.5f});
    AlighmentMenu->addChild(changeAlignmentRight);

    auto changeAlignmentLeftS = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    changeAlignmentLeftS->setScaleX(0.35f);
    changeAlignmentLeftS->setScaleY(0.2f);
    auto changeAlignmentLeft = CCMenuItemSpriteExtra::create(
        changeAlignmentLeftS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::ChangeAlignmentLeft)
    );
    changeAlignmentLeft->setPosition({-32, -0.5f});
    changeAlignmentLeft->setRotation(180);
    AlighmentMenu->addChild(changeAlignmentLeft);

    //color

    CCNode* ColorCont = CCNode::create();
    ColorCont->setID("Color-Container");
    ColorCont->setPosition({229, 134});
    allCont->addChild(ColorCont);

    ccColor3B currentColor = {m_LabelWin->m_MyLayout.color.r, m_LabelWin->m_MyLayout.color.g, m_LabelWin->m_MyLayout.color.b};

    m_ColorPicker = CCControlColourPicker::colourPicker();
    m_ColorPicker->setAnchorPoint({0,0});
    m_ColorPicker->setPosition({-22, -7});
    m_ColorPicker->setScale(0.425f);
    m_ColorPicker->setColorValue(currentColor);
    m_ColorPicker->setID("Color-Picker");
    m_ColorPicker->setDelegate(this);
    ColorCont->addChild(m_ColorPicker);

    m_OpacitySlider = Slider::create(this, menu_selector(LabelSettingsLayer::OnOpacitySliderChanged));
    m_OpacitySlider->setPosition({-106, -153});
    m_OpacitySlider->setScale(0.6f);
    float valFloat = (static_cast<float>(m_LabelWin->m_MyLayout.color.a)) / 255;
    m_OpacitySlider->setValue(valFloat);
    m_OpacitySlider->setID("Opacity-Slider");
    ColorCont->addChild(m_OpacitySlider);

    m_ColorInputR = InputNode::create(95, "Red");
    m_ColorInputR->setPosition({50, 18});
    m_ColorInputR->setScale(0.6f);
    m_ColorInputR->getInput()->setAllowedChars("1234567890");
    m_ColorInputR->getInput()->setMaxLabelLength(3);
    m_ColorInputR->setString(std::to_string(currentColor.r));
    m_ColorInputR->setID("R-Input");
    m_ColorInputR->getInput()->setDelegate(this);
    ColorCont->addChild(m_ColorInputR);

    m_ColorInputG = InputNode::create(95, "Green");
    m_ColorInputG->setPosition({50, -9});
    m_ColorInputG->setScale(0.6f);
    m_ColorInputG->getInput()->setAllowedChars("1234567890");
    m_ColorInputG->getInput()->setMaxLabelLength(3);
    m_ColorInputG->setString(std::to_string(currentColor.g));
    m_ColorInputG->setID("G-Input");
    m_ColorInputG->getInput()->setDelegate(this);
    ColorCont->addChild(m_ColorInputG);

    m_ColorInputB = InputNode::create(95, "Blue");
    m_ColorInputB->setPosition({50, -36});
    m_ColorInputB->setScale(0.6f);
    m_ColorInputB->getInput()->setAllowedChars("1234567890");
    m_ColorInputB->getInput()->setMaxLabelLength(3);
    m_ColorInputB->setString(std::to_string(currentColor.b));
    m_ColorInputB->setID("B-Input");
    m_ColorInputB->getInput()->setDelegate(this);
    ColorCont->addChild(m_ColorInputB);

    m_ColorInputA = InputNode::create(95, "Alpha");
    m_ColorInputA->setPosition({68, -60});
    m_ColorInputA->setScale(0.6f);
    m_ColorInputA->getInput()->setAllowedChars("1234567890");
    m_ColorInputA->getInput()->setMaxLabelLength(3);
    m_ColorInputA->setString(std::to_string(m_LabelWin->m_MyLayout.color.a));
    m_ColorInputA->setID("Alpha-Input");
    m_ColorInputA->getInput()->setDelegate(this);
    ColorCont->addChild(m_ColorInputA);

    m_ColorInputHex = InputNode::create(150, "Hex");
    m_ColorInputHex->setPosition({-11, -60});
    m_ColorInputHex->setScale(0.6f);
    m_ColorInputHex->getInput()->setAllowedChars("1234567890AaBbCcDdEeFf");
    m_ColorInputHex->getInput()->setMaxLabelLength(6);
    m_ColorInputHex->setString(cc3bToHexString(currentColor));
    m_ColorInputHex->setID("Hex-Input");
    m_ColorInputHex->getInput()->setDelegate(this);
    ColorCont->addChild(m_ColorInputHex);

    //text

    CCNode* TextCont = CCNode::create();
    TextCont->setID("Text-Container");
    TextCont->setPosition({284, 182});
    allCont->addChild(TextCont);

    auto TextInputLabel = CCLabelBMFont::create("Text", "bigFont.fnt");
    TextInputLabel->setPosition({0, 23});
    TextInputLabel->setScale(.55f);
    TextCont->addChild(TextInputLabel);

    m_TextInput = InputNode::create(300, "");
    m_TextInput->setPosition({0, 0});
    m_TextInput->setScale(0.8f);
    m_TextInput->setString(m_LabelWin->m_MyLayout.text);
    m_TextInput->getInput()->setDelegate(this);
    m_TextInput->getInput()->setAllowedChars("abcdefghijkmlnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789!@#$%^&*(){}[]\"\"/-_=+'?;:><,.`~|");
    TextCont->addChild(m_TextInput);

    //font

    m_FontCont = CCNode::create();
    m_FontCont->setID("Font-Container");
    m_FontCont->setPosition({318, 88});
    allCont->addChild(m_FontCont);

    m_FontOuterCont = CCNode::create();
    m_FontOuterCont->setID("Font-Outer-Container");
    m_FontOuterCont->setPosition({38, 46});
    m_FontCont->addChild(m_FontOuterCont);

    auto FontOuterContMenu = CCMenu::create();
    FontOuterContMenu->setPosition({0,0});
    m_FontOuterCont->addChild(FontOuterContMenu);

    m_FontTextDisplay = SimpleTextArea::create(StatsManager::getFontName(m_LabelWin->m_MyLayout.font), StatsManager::getFont(m_LabelWin->m_MyLayout.font));
    m_FontTextDisplay->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
    m_FontTextDisplay->setPosition({0, -2});
    m_FontTextDisplay->setScale(0.35f);
    m_FontTextDisplay->setZOrder(1);
    m_FontOuterCont->addChild(m_FontTextDisplay);

    CCScale9Sprite* FontTextDisplayBG = CCScale9Sprite::create("square02_small.png", {0,0, 40, 40});
    FontTextDisplayBG->setPosition({0, -2});
    FontTextDisplayBG->setScale(0.525f);
    FontTextDisplayBG->setContentSize({145, 30});
    FontTextDisplayBG->setOpacity(100);
    m_FontOuterCont->addChild(FontTextDisplayBG);

    auto FontLabel = CCLabelBMFont::create("Font", "bigFont.fnt");
    FontLabel->setPosition({0, 15});
    FontLabel->setScale(.475f);
    m_FontOuterCont->addChild(FontLabel);

    auto SelectFontBS = ButtonSprite::create("Change");
    SelectFontBS->setScale(0.5f);
    auto SelectFontB = CCMenuItemSpriteExtra::create(
        SelectFontBS,
        nullptr,
        this,
        menu_selector(LabelSettingsLayer::ChangeFont)
    );
    SelectFontB->setPosition({0, -21});
    FontOuterContMenu->addChild(SelectFontB);

    //font size

    CCNode* FontSizeCont = CCNode::create();
    FontSizeCont->setID("Font-Size-Container");
    FontSizeCont->setPosition({229, 134});
    allCont->addChild(FontSizeCont);

    auto FontSLabel = CCLabelBMFont::create("Font Size", "bigFont.fnt");
    FontSLabel->setPosition({139, -59});
    FontSLabel->setScale(.45f);
    FontSizeCont->addChild(FontSLabel);

    m_FontSizeInput = InputNode::create(95, "Alpha");
    m_FontSizeInput->setPosition({138, -78});
    m_FontSizeInput->setScale(0.6f);
    m_FontSizeInput->getInput()->setAllowedChars("1234567890.");
    m_FontSizeInput->getInput()->setMaxLabelLength(5);
    m_FontSizeInput->setString(fmt::format("{:.3f}", m_LabelWin->m_MyLayout.fontSize));
    m_FontSizeInput->setID("Font-Size-Input");
    m_FontSizeInput->getInput()->setDelegate(this);
    FontSizeCont->addChild(m_FontSizeInput);

    m_FontSizeSlider = Slider::create(this, menu_selector(LabelSettingsLayer::OnFontSizeSliderChanged));
    m_FontSizeSlider->setPosition({-33, -190});
    m_FontSizeSlider->setScale(0.4f);
    float valFloatFontSize = (static_cast<float>(m_LabelWin->m_MyLayout.fontSize)) / 2;
    m_FontSizeSlider->setValue(valFloatFontSize);
    m_FontSizeSlider->setID("Font-Size-Slider");
    FontSizeCont->addChild(m_FontSizeSlider);

    return true;
}

std::string LabelSettingsLayer::AlignmentToSring(CCTextAlignment alignment){
    switch (alignment)
    {
        case CCTextAlignment::kCCTextAlignmentLeft:
            return "Left";
        case CCTextAlignment::kCCTextAlignmentCenter:
            return "Center";
        case CCTextAlignment::kCCTextAlignmentRight:
            return "Right";
        default:
            return "Center";
    }
}

void LabelSettingsLayer::ChangeAlignmentLeft(CCObject* object){
    switch (m_LabelWin->m_MyLayout.alignment)
    {
        case CCTextAlignment::kCCTextAlignmentLeft:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentRight;
            break;
        case CCTextAlignment::kCCTextAlignmentCenter:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentLeft;
            break;
        case CCTextAlignment::kCCTextAlignmentRight:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentCenter;
            break;
        
        default:
            break;
    }
    m_Alighment->setString(AlignmentToSring(m_LabelWin->m_MyLayout.alignment).c_str());
}

void LabelSettingsLayer::ChangeAlignmentRight(CCObject* object){
    switch (m_LabelWin->m_MyLayout.alignment)
    {
        case CCTextAlignment::kCCTextAlignmentLeft:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentCenter;
            break;
        case CCTextAlignment::kCCTextAlignmentCenter:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentRight;
            break;
        case CCTextAlignment::kCCTextAlignmentRight:
            m_LabelWin->m_MyLayout.alignment = CCTextAlignment::kCCTextAlignmentLeft;
            break;
        
        default:
            break;
    }
    m_Alighment->setString(AlignmentToSring(m_LabelWin->m_MyLayout.alignment).c_str());
}

void LabelSettingsLayer::OnOpacitySliderChanged(CCObject* object){
    m_LabelWin->s->setOpacity(m_OpacitySlider->getThumb()->getValue() * 255);
    m_ColorInputA->setString(std::to_string(static_cast<int>(m_OpacitySlider->getThumb()->getValue() * 255)));
    m_LabelWin->m_MyLayout.color.a = static_cast<GLubyte>(m_OpacitySlider->getThumb()->getValue() * 255);
}

void LabelSettingsLayer::textChanged(CCTextInputNode* input){
    if (m_BlockSelfCall > 0){
        m_BlockSelfCall--;
        return;
    }
    int value;
    ccColor3B currentColorPickerColor = static_cast<CCControlColourPickerBypass*>(m_ColorPicker)->getPickedColor();

    //alpha changed
    if (input == m_ColorInputA->getInput()){
        if (input->getString() == "")
            value = 0;
        else
            value = std::stoi(input->getString());
            
        if (value > 255){
            value = 255;
            input->setString("255");
        }

        float valFloat = (static_cast<float>(value)) / 255;
        m_OpacitySlider->setValue(valFloat);

        m_LabelWin->s->setOpacity(value);
            
        m_LabelWin->m_MyLayout.color.a = static_cast<GLubyte>(value);
    }

    //red changed
    if (input == m_ColorInputR->getInput()){
        if (input->getString() == "")
            value = 0;
        else
            value = std::stoi(input->getString());
            
        if (value > 255){
            value = 255;
            input->setString("255");
        }

        ccColor3B colorCreated = {static_cast<GLubyte>(value), currentColorPickerColor.g, currentColorPickerColor.b};

        m_BlockSelfCall += 2;
        m_ColorInputHex->setString(cc3bToHexString(colorCreated));
        m_BlockSelfCall += 1;
        m_ColorPicker->setColorValue(colorCreated);
                
        m_LabelWin->m_MyLayout.color.r = static_cast<GLubyte>(value);
    }

    //green changed
    if (input == m_ColorInputG->getInput()){
        if (input->getString() == "")
            value = 0;
        else
            value = std::stoi(input->getString());
            
        if (value > 255){
            value = 255;
            input->setString("255");
        }

        ccColor3B colorCreated = {currentColorPickerColor.r, static_cast<GLubyte>(value), currentColorPickerColor.b};

        m_BlockSelfCall += 2;
        m_ColorInputHex->setString(cc3bToHexString(colorCreated));
        m_BlockSelfCall += 1;
        m_ColorPicker->setColorValue(colorCreated);
                
        m_LabelWin->m_MyLayout.color.g = static_cast<GLubyte>(value);
    }

    //blue changed
    if (input == m_ColorInputB->getInput()){
        if (input->getString() == "")
            value = 0;
        else
            value = std::stoi(input->getString());
            
        if (value > 255){
            value = 255;
            input->setString("255");
        }

        ccColor3B colorCreated = {currentColorPickerColor.r, currentColorPickerColor.g, static_cast<GLubyte>(value)};

        m_BlockSelfCall += 2;
        m_ColorInputHex->setString(cc3bToHexString(colorCreated));
        m_BlockSelfCall += 1;
        m_ColorPicker->setColorValue(colorCreated);
                
        m_LabelWin->m_MyLayout.color.b = static_cast<GLubyte>(value);
    }

    //HEX changed
    if (input == m_ColorInputHex->getInput()){
        std::string hexValue;

        if (input->getString() == "")
            hexValue = "000000";
        else
            hexValue = input->getString();

        while (hexValue.length() < 6){
            hexValue.push_back('0');
        }
        try{
            ccColor3B hexColor = cc3bFromHexString(hexValue).value();

            m_BlockSelfCall += 1;
            m_ColorPicker->setColorValue(hexColor);

            m_BlockSelfCall += 2;
            m_ColorInputR->setString(std::to_string(hexColor.r));
            m_BlockSelfCall += 2;
            m_ColorInputG->setString(std::to_string(hexColor.g));
            m_BlockSelfCall += 2;
            m_ColorInputB->setString(std::to_string(hexColor.b));
                    
            m_LabelWin->m_MyLayout.color = {hexColor.r, hexColor.g, hexColor.b, m_LabelWin->m_MyLayout.color.a};
        }
        catch (...) {}
    }

    if (input == m_LabelNameInput->getInput()){
        if (input->getString() == ""){
            m_LabelWin->m_MyLayout.labelName = "";
            m_LabelWin->m_Label->setText("");
        }
        else{
            m_LabelWin->m_MyLayout.labelName = input->getString();
            m_LabelWin->m_Label->setText(input->getString().c_str());
        }
    }

    if (input == m_TextInput->getInput()){
        if (input->getString() == ""){
            m_LabelWin->m_MyLayout.text = "";
        }
        else{
            m_LabelWin->m_MyLayout.text = input->getString();
        }
    
    }

    //Font Size changed
    if (input == m_FontSizeInput->getInput()){
        float fvalue = 0;
        if (input->getString() == "")
            fvalue = 0;
        else{
            if (input->getString()[0] == '.'){
                fvalue = 0;
            }
            else{
                fvalue = std::stof(input->getString());
            }
        }
            
        if (fvalue > 2){
            fvalue = 2;
            input->setString("2");
        }

        float valFloat = fvalue / 2;
        m_FontSizeSlider->setValue(valFloat);

        m_LabelWin->m_MyLayout.fontSize = fvalue;
    }
}

void LabelSettingsLayer::colorValueChanged(ccColor3B color){
    m_LabelWin->s->setColor(color);

    if (m_BlockSelfCall > 0){
        m_BlockSelfCall--;
        return;
    }
    m_BlockSelfCall += 2;
    m_ColorInputR->setString(std::to_string(color.r));
    m_BlockSelfCall += 2;
    m_ColorInputG->setString(std::to_string(color.g));
    m_BlockSelfCall += 2;
    m_ColorInputB->setString(std::to_string(color.b));
    m_BlockSelfCall += 2;
    m_ColorInputHex->setString(cc3bToHexString(color));

    m_LabelWin->m_MyLayout.color = {color.r, color.g, color.b, m_LabelWin->m_MyLayout.color.a};
}

void LabelSettingsLayer::ChangeFont(CCObject*){

    m_FontOuterCont->setVisible(false);

    if (!m_FontList){
        CCArray* Fonts = CCArray::create();

        auto AllFonts = StatsManager::getAllFont();

        for (int i = 0; i < AllFonts.size(); i++)
        {
            std::string fontName = "";

            if (i == 0)
                fontName = "Big Font";
            else if (i == 1)
                fontName = "Chat Font";
            else if (i == 2)
                fontName = "Gold Font";
            else{
                fontName = "Font " + std::to_string(i - 2);
            }

            Fonts->addObject(LabelFontCell::create(i, AllFonts[i], fontName, this));
        }

        auto FontListView = ListView::create(Fonts, 20, 85, 70);

        m_FontList = GJListLayer::create(FontListView, "Fonts", {0,0,0,75}, 85, 70, 1);
        m_FontCont->addChild(m_FontList);

        CCObject* child;

        CCARRAY_FOREACH(m_FontList->m_listView->m_tableView->m_cellArray, child){
            auto childCell = dynamic_cast<GenericListCell*>(child);
            if (childCell)
                childCell->m_backgroundLayer->setOpacity(30);
        }

        std::vector<CCSprite*> spritesToRemove;
        CCLabelBMFont* title;

        CCARRAY_FOREACH(m_FontList->getChildren(), child){
            auto childSprite = dynamic_cast<CCSprite*>(child);
            auto childLabel = dynamic_cast<CCLabelBMFont*>(child);
            if (childSprite)
                spritesToRemove.push_back(childSprite);
            if (childLabel)
                title = childLabel;
        }

        for (int i = 0; i < spritesToRemove.size(); i++)
        {
            spritesToRemove[i]->removeMeAndCleanup();
        }

        title->setScale(0.4f);
        title->setPosition({42.5f, 76});
    }
    else{
        m_FontList->setVisible(true);
    }
    
}

void LabelSettingsLayer::FontSelected(int FontID){
    m_FontList->setVisible(false);
    m_FontOuterCont->setVisible(true);
    m_LabelWin->m_MyLayout.font = FontID;
    std::string selectedFont = StatsManager::getFont(FontID);
    m_FontTextDisplay->setFont(selectedFont);
    m_FontTextDisplay->setText(StatsManager::getFontName(FontID));
    m_LabelWin->m_Label->setFont(selectedFont);
}

void LabelSettingsLayer::OnFontSizeSliderChanged(CCObject* object){
    m_FontSizeInput->setString(fmt::format("{:.3f}", m_FontSizeSlider->getThumb()->getValue() * 2));
    m_LabelWin->m_MyLayout.fontSize = m_FontSizeSlider->getThumb()->getValue() * 2;
}

void LabelSettingsLayer::deleteLabel(CCObject*){
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
    
    m_LabelWin->removeMeAndCleanup();

    m_LabelWin->m_DTLayer->changeScrollSizeByBoxes();

    keyBackClicked();
}

void LabelSettingsLayer::onOverallInfo(CCObject*){
    auto alert = FLAlertLayer::create("Help", "<cg>Label Name</c> - change this labels name\n<cy>Text Alighnment</c> - change the alighnment of this label\n<cj>Text</c> - the text displayed by this label\n<cr>Color picker</c> - change the texts color\n<cb>Font</c> - change the font of the text\n<ca>Font Size</c> - change the texts size", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}

/*
    keys to check for

    {f0} - runs from 0

    {runs} - runs

    {lvln} - levels name
    
    {att} - level attempts (with linked levels attempts)

    {s0} - selected session runs from 0
    
    {sruns} - selected session runs

    {nl} - new line
    */
void LabelSettingsLayer::onSpecialInfo(CCObject*){
    auto alert = FLAlertLayer::create("Special Text Keys:", "{f0} - runs from 0\n{runs} - runs\n{lvln} - the levels name\n{att} - level attempts (with linked levels attempts)\n{s0} - selected session runs from 0\n{sruns} - selected session runs\n{nl} - new line", "Ok");
    alert->setZOrder(150);
    this->addChild(alert);
}
