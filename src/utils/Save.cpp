#include "Save.hpp"


std::vector<LabelLayout> Save::getLayout(){
    return Mod::get()->getSavedValue<std::vector<LabelLayout>>("Layout");
}

void Save::setLayout(std::vector<LabelLayout> layout){
    Mod::get()->setSavedValue("Layout",layout );
}

ccColor3B Save::getNewBestColor(){
    return Mod::get()->getSavedValue<ccColor3B>("NewBestColor");
}

void Save::setNewBestColor(ccColor3B color){
    Mod::get()->setSavedValue("NewBestColor", color);
}

ccColor3B Save::getSessionBestColor(){
    return Mod::get()->getSavedValue<ccColor3B>("SessionBestColor");
}

void Save::setSessionBestColor(ccColor3B color){
    Mod::get()->setSavedValue("SessionBestColor", color);
}