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

#pragma once

#ifndef TIMECHECKER_H
#define TIMECHECKER_H

#include <chrono>
#include <string_view>

class TimeChecker {
public:
    static constexpr bool ENFORCE_TIMESTAMP = true;
    static constexpr const char *COMPILATION_DATE = __DATE__;
    static constexpr int ALLOWED_MONTH_DIFFERENCE = 1;

    static std::chrono::duration<long long, std::ratio<2629746>>
        getDaysFromDateString(std::string_view dateString);
    static void enforceTimeStamp();
    static bool isExecutionPermitted();

    TimeChecker() = delete;
private:
};

#endif // TIMECHECKER_H