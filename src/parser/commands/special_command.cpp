// Copyright (C) 2022 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <ostream>

#include "../../../standardese/parser/commands/special_command.hpp"

namespace standardese::parser::commands {

std::ostream& operator<<(std::ostream& os, special_command command) {
  switch (command) { 
    case special_command::end:
      return os << "end";
    case special_command::exclude:
      return os << "exclude";
    case special_command::unique_name:
      return os << "unique_name";
    case special_command::output_name:
      return os << "output_name";
    case special_command::synopsis:
      return os << "synopsis";
    case special_command::group:
      return os << "group";
    case special_command::module:
      return os << "module";
    case special_command::output_section:
      return os << "output_section";
    case special_command::entity:
      return os << "entity";
    case special_command::file:
      return os << "file";
    default:
      throw std::logic_error("unrecognized special command");
  }
}

}
