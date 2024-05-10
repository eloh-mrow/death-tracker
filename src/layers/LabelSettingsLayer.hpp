#pragma once

#include <Geode/Geode.hpp>
#include "../layers/LabelLayoutWindow.hpp"
#include <Geode/ui/TextArea.hpp>
#include "../layers/LabelFontCell.hpp"

using namespace geode::prelude;

class LabelSettingsLayer : public Popup<LabelLayoutWindow* const&>, public TextInputDelegate, public ColorPickerDelegate {
    protected:
        bool setup(LabelLayoutWindow* const& labelWin) override;
    public:
        static LabelSettingsLayer* create(LabelLayoutWindow* const& labelWin);

    LabelLayoutWindow* m_LabelWin;
    void deleteLabel(CCObject*);

    CCNode* alighmentNode;

    //label name
    InputNode* m_LabelNameInput;

    //alighment
    std::string AlignmentToSring(CCTextAlignment alignment);
    void ChangeAlignmentLeft(CCObject* object);
    void ChangeAlignmentRight(CCObject* object);
    CCLabelBMFont* m_Alighment;

    //color
    CCControlColourPicker* m_ColorPicker;
    void colorValueChanged(ccColor3B color) override;
    Slider* m_OpacitySlider;
    void OnOpacitySliderChanged(CCObject* object);
    InputNode* m_ColorInputR;
    InputNode* m_ColorInputG;
    InputNode* m_ColorInputB;
    InputNode* m_ColorInputA;
    InputNode* m_ColorInputHex;
    void textChanged(CCTextInputNode* input) override;
    int m_BlockSelfCall;

    //text
    InputNode* m_TextInput;

    //font
    SimpleTextArea* m_FontTextDisplay;
    GJListLayer* m_FontList = nullptr;
    CCNode* m_FontCont;
    CCNode* m_FontOuterCont;
    void ChangeFont(CCObject*);
    void FontSelected(int FontID);

    //font size
    InputNode* m_FontSizeInput;
    Slider* m_FontSizeSlider;
    void OnFontSizeSliderChanged(CCObject* object);
};