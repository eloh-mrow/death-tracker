#pragma once

#include <Geode/Geode.hpp>
#include "../layers/LabelLayoutWindow.hpp"
#include <Geode/ui/TextArea.hpp>
#include "../layers/LabelFontCell.hpp"
#include "../layers/DTLayer.hpp"

using namespace geode::prelude;

class LabelSettingsLayer : public Popup<LabelLayoutWindow* const&, DTLayer* const&>, public ColorPickPopupDelegate, public FLAlertLayerProtocol {
    protected:
        bool setup(LabelLayoutWindow* const& labelWin, DTLayer* const& dtLayer) override;

        void onClose(cocos2d::CCObject*) override;

        void update(float delta);

        void updateColor(cocos2d::ccColor4B const& color) override;

        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) override;
    public:
        static LabelSettingsLayer* create(LabelLayoutWindow* const& labelWin, DTLayer* const& dtLayer);

        LabelLayoutWindow* m_LabelWin;
        DTLayer* m_DTLayer;

        void ConfirmDeletLabel(CCObject*);
        FLAlertLayer* deleteConfPopup;
        void deleteLabel();
        bool deleteOnExit;

        SimpleTextArea* previewText;
        geode::ScrollLayer* previewScroll;
        void updatePreviewText();
        void updatePreviewTextPosition();
        void removeTextBestColoring();

        CCScale9Sprite* textPreviewWindow;
        CCScale9Sprite* labelWinPreview;
        void playEntryAnimation(const CCPoint& startingPoint);
        void playColsingAnimation(const CCPoint& endPoint);
        bool isTransitioning;

        void OnTransitionEnded(CCObject* transitionType);

        TextInput* windowTitleInput;
        void updateInputFields(const std::string& font);

        CCMenuItemSpriteExtra* alignLeftButton;
        CCMenuItemSpriteExtra* alignRightButton;
        CCMenuItemSpriteExtra* alignCenterButton;
        void onAlign(CCObject* sender);

        TextInput* FontSizeInput;
        Slider* FontSizeSlider;
        void OnFontSizeSliderChanged(CCObject*);

        ColorChannelSprite* ColorSprite;
        void OnColorClicked(CCObject*);

        void setFont(const int& fontID);

        void useSTK(const std::string& stk);
        TextInput* labelTextInput;

        void OnInfo(CCObject*);
};