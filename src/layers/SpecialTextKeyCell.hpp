#pragma once

#include <Geode/Geode.hpp>

using namespace cocos2d;
using namespace geode;

class SpecialTextKeyCell : public CCNode {
    protected:
        bool init(const std::string& textKey, const std::string& description, const std::function<void(const std::string&)>& onAdd);
    public:
        static SpecialTextKeyCell* create(const std::string& textKey, const std::string& description, const std::function<void(const std::string&)>& onAdd = NULL);

    private:
        //runs the given callback to this SpecialTextKeyCell
        void Add(CCObject*);
        //info about this special text key 
        void onInfo(CCObject*);

        std::string m_TextKey;
        std::string m_Description;
        std::function<void(const std::string&)> m_Callback;
};