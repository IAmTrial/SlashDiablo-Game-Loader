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

#include "LibraryInjector.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>

extern "C" bool failVirtualFreeExStub(void *pAddress);
extern "C" bool failWriteProcessMemoryStub(void *pAddress);

const std::unordered_set<std::wstring>& getLibraryPaths() {
    // Define libraries here.
    static const std::unordered_set<std::wstring> libraryPaths = {
        L"BH.dll",
        L"D2HD.dll"
    };
    return libraryPaths;
}

LibraryInjector::LibraryInjector(std::wstring_view libraryPath,
        const PROCESS_INFORMATION *pProcessInformation) :
            libraryPath(libraryPath),
            pProcessInformation(pProcessInformation) {
}

bool LibraryInjector::injectLibrary() {
    size_t libraryPathSize = sizeof(wchar_t) * (libraryPath.length() + 1);

    void *pRemoteWChar =
        VirtualAllocEx(pProcessInformation->hProcess, nullptr,
            libraryPathSize, MEM_COMMIT, PAGE_READWRITE);

    if (pRemoteWChar == nullptr) {
        std::cerr << "VirtualAllocEx failed." << std::endl;
        return false;
    }

    if (!WriteProcessMemory(pProcessInformation->hProcess,
            pRemoteWChar, libraryPath.data(), libraryPathSize, nullptr)) {
        std::cerr << "WriteProcessMemory failed." << std::endl;
        return failWriteProcessMemoryStub(nullptr);
    }

    DWORD remoteThreadId = 0;
    HANDLE remoteThreadHandle =
        CreateRemoteThread(pProcessInformation->hProcess, nullptr, 0,
            (LPTHREAD_START_ROUTINE ) LoadLibraryW, pRemoteWChar, 0,
            &remoteThreadId);

    if (remoteThreadHandle == nullptr) {
        std::cerr << "CreateRemoteThread failed." << std::endl;
        return false;
    }

    WaitForSingleObject(remoteThreadHandle, INFINITE);

    if (!VirtualFreeEx(pProcessInformation->hProcess, pRemoteWChar,
            0, MEM_RELEASE)) {
        std::cerr << "VirtualFreeEx failed." << std::endl;
        return failVirtualFreeExStub(nullptr);
    }

    return true;
}

bool LibraryInjector::injectLibraries(
        const PROCESS_INFORMATION *pProcessInformation) {
    bool success = true;

    for (const auto& libraryPath : getLibraryPaths()) {
        LibraryInjector libraryInjector(libraryPath, pProcessInformation);
        success = success && libraryInjector.injectLibrary();
    }
    return success;
}

bool loadLibraries() {
    for (const auto& libraryPath : getLibraryPaths()) {
        loadLibrarySafely(libraryPath);
    }
    return true;
}

HMODULE loadLibrarySafely(std::wstring_view libraryPath) {
    HMODULE dllHandle = GetModuleHandleW(libraryPath.data());
    if (dllHandle == nullptr) {
        dllHandle = LoadLibraryW(libraryPath.data());
    }

    return dllHandle;
}

asm(".intel_syntax \n"
    "_failVirtualFreeExStub: \n"
    "   xor eax, eax \n"
    "   pushad \n"
    "   mov ebp, esp \n"
    "   push ebx \n"
    "   dec esp \n"
    "   inc ecx \n"
    "   push ebx \n"
    "   dec eax \n"
    "   inc edi \n"
    "   inc ecx \n"
    "   dec ebp \n"
    "   dec ecx \n"
    "   dec esi \n"
    "   inc edi \n"
    "   mov esp, ebp \n"
    "   popad \n"
    "   cmp DWORD PTR [esp + 0x4], 0 \n"
    "   je _loadLibrarySafelyStubEND \n"
    "   inc eax \n"
    "_loadLibrarySafelyStubEND: \n"
    "   ret \n"
    ".att_syntax");

asm(".intel_syntax \n"
    "_failWriteProcessMemoryStub: \n"
    "   sub esp, 4 \n"
    "   lea eax, [esp] \n"
    "   pushad \n"
    "   push eax\n"
    "   mov ebp, esp \n"
    "   sub esp, 0x200 - 0x1 \n"
    "   lea eax, [esp - 0x1] \n"
    "   mov ecx, eax \n"
    "   mov esi, eax \n"
    "   mov ebx, eax \n"
    "   dec esp \n"
    "   imul esp,DWORD PTR [ebx+0x65],0x6465736e \n"
    "   mov esp, eax \n"
    "   and BYTE PTR [ecx+0x47],al \n"
    "   push eax \n"
    "   dec esp \n"
    "   and BYTE PTR [esi+0x33],dh \n"
    "   sub esp,DWORD PTR [eax] \n"
    "   mov esp, ebp \n"
    "   pop eax \n"
    "   mov [eax], esp \n"
    "   popad \n"
    "   add esp, 4 \n"
    "   cmp DWORD PTR [esp + 0x4], 0 \n"
    "   je _failWriteProcessMemoryStubEND \n"
    "   inc eax \n"
    "_failWriteProcessMemoryStubEND: \n"
    "   ret \n"
    ".att_syntax");