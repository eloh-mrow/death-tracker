#include "Save.hpp"


std::vector<LabelLayout> Save::getLayout(){
    return Mod::get()->getSavedValue<std::vector<LabelLayout>>("Layout");
}

void Save::setLayout(std::vector<LabelLayout> layout){
    Mod::get()->setSavedValue("Layout",layout );
}