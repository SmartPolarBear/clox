#pragma once
#include <string>

namespace clox::driver
{

[[nodiscard]] int run_file(std::string name);

[[nodiscard]] int run_command();

}