#include <Geode/Geode.hpp>
using namespace geode::prelude;

$execute{
    Result<> res = file::createDirectory(Mod::get()->getSaveDir() / "Death Saves");
}