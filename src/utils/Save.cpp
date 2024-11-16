#include "Save.hpp"


std::vector<LabelLayout> Save::getLayout(){
    return Mod::get()->getSavedValue<std::vector<LabelLayout>>("Layout");
}

void Save::setLayout(const std::vector<LabelLayout>& layout){
    Mod::get()->setSavedValue("Layout",layout );
}

ccColor3B Save::getNewBestColor(){
    return Mod::get()->getSavedValue<ccColor3B>("NewBestColor");
}

void Save::setNewBestColor(const ccColor3B& color){
    Mod::get()->setSavedValue("NewBestColor", color);
}

ccColor3B Save::getSessionBestColor(){
    return Mod::get()->getSavedValue<ccColor3B>("SessionBestColor");
}

void Save::setSessionBestColor(const ccColor3B& color){
    Mod::get()->setSavedValue("SessionBestColor", color);
}

bool Save::getExportWOutLabels(){
    return Mod::get()->getSavedValue<bool>("ExportWLabels");
}

void Save::setExportWOutLabels(const bool& b){
    Mod::get()->setSavedValue("ExportWLabels", b);
}

std::string Save::getLastOpenedVersion(){
    return Mod::get()->getSavedValue<std::string>("LastOpenedVersion");
}

void Save::setLastOpenedVersion(const std::string& verion){
    Mod::get()->setSavedValue("LastOpenedVersion", verion);
}

