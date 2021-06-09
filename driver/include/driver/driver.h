#pragma once
#include <string>

namespace clox::driver
{

[[nodiscard]] int run_file(const std::string& name);

[[nodiscard]] int run_command();

}