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

#include "TimeChecker.h"

#include <windows.h>
#include <chrono>
#include <iostream>
#include <regex>
#include <unordered_map>

std::chrono::duration<long long, std::ratio<2629746>>
        TimeChecker::getDaysFromDateString(std::string_view dateString) {
    static const std::unordered_map<std::string, int> monthValues = {
        { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 }, { "May", 4 },
        { "Jun", 5 }, { "Jul", 6 }, { "Aug", 7 }, { "Sep", 8 }, { "Oct", 9 },
        { "Nov", 10 }, { "Dec", 11 }
    };

    const std::regex COMPILE_DATE_REGEX("(\\w+) (\\d+) (\\d+)");
    std::cmatch matches;

    if (!std::regex_match(dateString.data(), matches, COMPILE_DATE_REGEX)) {
        return std::chrono::duration<long long, std::ratio<2629746>>(0);
    }

    // Calculate number of months from epoch time (Jan 1, 1970).
    long long month = monthValues.at(matches[1]);
    long long year = std::stoll(matches[3].str()) - 1970;
    std::chrono::duration<long long, std::ratio<2629746>>
        totalDaysDuration(month + (year * 12));

    return totalDaysDuration;
}

void TimeChecker::enforceTimeStamp() {
    if constexpr (!ENFORCE_TIMESTAMP) {
        return;
    }

    std::cout << "Timestamp is enforced, meaning that this program " <<
        "will cease to function " << ALLOWED_MONTH_DIFFERENCE <<
        " month(s) after " << COMPILATION_DATE << "." << std::endl;
    std::cout << "This means that you have received a version of this " <<
        "program not meant for public release." << std::endl;
    std::cout << std::endl;

    if (!isExecutionPermitted()) {
        MessageBoxW(nullptr, L"Date of execution exceeds timestamp limit.",
            L"Execution Date Exceeded", MB_OK | MB_ICONERROR);
        std::exit(0);
    }
}

bool TimeChecker::isExecutionPermitted() {
    auto compileMonthDuration =
        getDaysFromDateString(COMPILATION_DATE);

    // If the compilation date could not be parsed, don't allow execution.
    if (compileMonthDuration ==
            std::chrono::duration<long long, std::ratio<2629746>>(0)) {
        return false;
    }

    // Calculate the difference in months between the two.
    auto todayTimePoint = std::chrono::system_clock::now();
    auto todayDuration = todayTimePoint.time_since_epoch();
    auto todayMonthDuration =
        std::chrono::duration_cast<std::chrono::duration<long long, std::ratio<2629746>>>(todayDuration);

    auto monthDifference = todayMonthDuration - compileMonthDuration;

    return (monthDifference.count() >= 0) &&
        (monthDifference.count() <= ALLOWED_MONTH_DIFFERENCE);
}
