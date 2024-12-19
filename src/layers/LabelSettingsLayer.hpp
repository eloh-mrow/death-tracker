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

        void update(float delta) override;

        void updateColor(cocos2d::ccColor4B const& color) override;

        void FLAlert_Clicked(FLAlertLayer* p0, bool p1) override;
    public:
        static LabelSettingsLayer* create(LabelLayoutWindow* const& labelWin, DTLayer* const& dtLayer);

    private:
        //opens the confirm delet label popup
        void ConfirmDeletLabel(CCObject*);
        FLAlertLayer* deleteConfPopup;

        //delets the label
        void deleteLabel();
        bool deleteOnExit;

        //update the contents of the preview text
        void updatePreviewText();
        //update the position of the preview text 
        void updatePreviewTextPosition();
        //formats the new best coloring in the preview text
        void removeTextBestColoring();

        SimpleTextArea* previewText;
        geode::ScrollLayer* previewScroll;

        //play the windows open animation
        void playEntryAnimation(const CCPoint& startingPoint);
        //play the windows close animation and close it
        void playClosingAnimation(const CCPoint& endPoint);
        bool isTransitioning;

        //handle everything after a transition ends
        void OnTransitionEnded(CCObject* transitionType);

        //refresh some input fields based on the label settings
        void updateInputFields(const std::string& font);

        //sets the labels alignment based on the alignment button clicked
        void onAlign(CCObject* sender);

        CCMenuItemSpriteExtra* alignLeftButton;
        CCMenuItemSpriteExtra* alignRightButton;
        CCMenuItemSpriteExtra* alignCenterButton;

        //process font size slider changing and save the values
        void OnFontSizeSliderChanged(CCObject*);
        TextInput* FontSizeInput;
        Slider* FontSizeSlider;

        //open the color picker layer for editing the labels color
        void OnColorClicked(CCObject*);
        ColorChannelSprite* ColorSprite;

        //sets the labels font based on a fontID
        void setFont(const int& fontID);

        //add a special text key onto the labels text
        void useSTK(const std::string& stk);
        TextInput* labelTextInput;

        //info abput the label settings and its settings
        void OnInfo(CCObject*);

        DTLayer* m_DTLayer;
        LabelLayoutWindow* m_LabelWin;

        CCScale9Sprite* textPreviewWindow;
        CCScale9Sprite* labelWinPreview;
        TextInput* windowTitleInput;
};