/**
 * SlashGaming Game Loader
 * Copyright (C) 2018  Mir Drualga
 *
 * This file is part of SlashGaming Game Loader.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Additional permissions under GNU Affero General Public License version 3
 *  section 7
 *
 *  If you modify this Program, or any covered work, by linking or combining
 *  it with any program (or a modified version of that program and its
 *  libraries), containing parts covered by the terms of an incompatible
 *  license, the licensors of this Program grant you additional permission
 *  to convey the resulting work.
 */

#include <windows.h>
#include <array>
#include <iostream>
#include <string_view>
#include <unordered_set>

#include "config_reader.h"
#include "game_loader.h"
#include "library_injector.h"
#include "license.h"
#include "time_checker.h"

namespace sgexe {

extern "C" int
main(
    int argc,
    const char* argv[]
) {
  PrintLicenseNotice();
  std::cout << "----------" << std::endl;

  // ShowWindow(GetConsoleWindow(), SW_HIDE);
  timechecker::EnforceTimeStamp();

  // Create a new process, waiting for its full initialization before the
  // startGame function can return.
  PROCESS_INFORMATION process_info;
  StartGame(&process_info);

  // Read the list of DLLs to inject from the config.
  std::vector<std::filesystem::path> library_paths = GetLibraryPaths();

  // Inject libraries, after reading all files.
  if (InjectLibraries(library_paths, process_info)) {
    std::cout << "All libraries have been successfully injected." << std::endl;
  } else {
    std::cout << "Some or all libraries failed to inject." << std::endl;
  }

  Sleep(500);

  return 0;
}

} // namespace sgexe