#include <Geode/Geode.hpp>
using namespace geode::prelude;

$execute {
    auto _ = file::createDirectory(Mod::get()->getSaveDir() / "levels");
}