#include <Geode/Geode.hpp>
using namespace geode::prelude;

$execute {
    file::createDirectory(Mod::get()->getSaveDir() / "levels");
}