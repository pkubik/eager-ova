#pragma once

#include <iostream>

typedef decltype(std::cerr) LoggerStream;
static LoggerStream& logW = std::cerr;
