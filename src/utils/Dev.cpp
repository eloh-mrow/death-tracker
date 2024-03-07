#include "Dev.hpp"

// TODO: set this to false before builing
const bool Dev::ENABLED = false;

/* global flags
================ */
const bool Dev::MINIFY_SAVE_FILE = Dev::ENABLED
    ? false
    : true;