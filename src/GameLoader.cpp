/**
 * SlashDiablo Game Loader
 * Copyright (C) 2018 Mir Drualga
 *
 *  This file is part of SlashDiablo Game Loader.
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
 */

#include "GameLoader.h"

#include <windows.h>

bool startGame(PROCESS_INFORMATION *processInformation) {
    STARTUPINFO startupInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);

    // Create the desired process.
    if (!CreateProcess("Game.exe", GetCommandLine(), nullptr, nullptr, true,
            0, nullptr, nullptr, &startupInfo,
            processInformation)) {
        MessageBoxW(nullptr, L"Game.exe could not be found.",
            L"Game executable not found.", MB_OK | MB_ICONERROR);
        std::exit(0);
    }

    // Wait until the process is started.
    return WaitForInputIdle(processInformation->hProcess, INFINITE) == 0;
}
