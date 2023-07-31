#include "logger.h"
#include "modpack_loader/loader.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <sys/iosupport.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static ModpackLoaderStatus (*sMPLGetversion)(ModpackLoaderVersion *)                       = nullptr;
static ModpackLoaderStatus (*sMPLParseModpackByPath)(const char *, MPLModpackHandle *)     = nullptr;
static ModpackLoaderStatus (*sMPLParseModpackFromCurrentlyRunningWUHB)(MPLModpackHandle *) = nullptr;
static ModpackLoaderStatus (*sMPLCheckIfLaunchable)(MPLModpackHandle, bool *)              = nullptr;
static ModpackLoaderStatus (*sMPLLaunchModpack)(MPLModpackHandle, bool *)                  = nullptr;

static ModpackLoaderVersion sModpackLoaderVersion = MODPACK_LOADER_MODULE_VERSION_ERROR;

const char *ModpackLoader_GetStatusStr(ModpackLoaderStatus status) {
    switch (status) {
        case MODPACK_LOADER_RESULT_SUCCESS:
            return "MODPACK_LOADER_RESULT_SUCCESS";
        case MODPACK_LOADER_RESULT_MODULE_NOT_FOUND:
            return "MODPACK_LOADER_RESULT_MODULE_NOT_FOUND";
        case MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT:
            return "MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT";
        case MODPACK_LOADER_RESULT_UNSUPPORTED_VERSION:
            return "MODPACK_LOADER_RESULT_UNSUPPORTED_VERSION";
        case MODPACK_LOADER_RESULT_INVALID_ARGUMENT:
            return "MODPACK_LOADER_RESULT_INVALID_ARGUMENT";
        case MODPACK_LOADER_RESULT_NO_MEMORY:
            return "MODPACK_LOADER_RESULT_NO_MEMORY";
        case MODPACK_LOADER_RESULT_LIB_UNINITIALIZED:
            return "MODPACK_LOADER_RESULT_LIB_UNINITIALIZED";
        case MODPACK_LOADER_RESULT_UNKNOWN_ERROR:
            return "MODPACK_LOADER_RESULT_UNKNOWN_ERROR";
        case MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND:
            return "MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND";
    }
    return "MODPACK_LOADER_RESULT_UNKNOWN_ERROR";
}

ModpackLoaderStatus ModpackLoader_InitLibrary() {
    if (OSDynLoad_Acquire("homebrew_modpack_loader", &sModuleHandle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("OSDynLoad_Acquire failed.");
        return MODPACK_LOADER_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLGetVersion", (void **) &sMPLGetversion) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport MPLGetVersion failed.");
        return MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }
    auto res = ModpackLoader_GetVersion(&sModpackLoaderVersion);
    if (res != MODPACK_LOADER_RESULT_SUCCESS) {
        return MODPACK_LOADER_RESULT_UNSUPPORTED_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLParseModpackFromCurrentlyRunningWUHB", (void **) &sMPLParseModpackFromCurrentlyRunningWUHB) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport MPLParseModpackFromCurrentlyRunningWUHB failed.");
        sMPLParseModpackFromCurrentlyRunningWUHB = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLParseModpackByPath", (void **) &sMPLParseModpackByPath) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport MPLParseModpackByPath failed.");
        sMPLParseModpackByPath = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLCheckIfLaunchable", (void **) &sMPLCheckIfLaunchable) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport MPLCheckIfLaunchable failed.");
        sMPLCheckIfLaunchable = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLLaunchModpack", (void **) &sMPLLaunchModpack) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport MPLLaunchModpack failed.");
        sMPLCheckIfLaunchable = nullptr;
    }

    return MODPACK_LOADER_RESULT_SUCCESS;
}

ModpackLoaderStatus ModpackLoader_DeInitLibrary() {
    return MODPACK_LOADER_RESULT_SUCCESS;
}

ModpackLoaderStatus ModpackLoader_GetVersion(ModpackLoaderVersion *outVariable) {
    if (sMPLGetversion == nullptr) {
        if (OSDynLoad_Acquire("homebrew_modpack_loader", &sModuleHandle) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("OSDynLoad_Acquire failed.");
            return MODPACK_LOADER_RESULT_MODULE_NOT_FOUND;
        }

        if (OSDynLoad_FindExport(sModuleHandle, OS_DYNLOAD_EXPORT_FUNC, "MPLGetVersion", (void **) &sMPLGetversion) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("FindExport MPLGetVersion failed.");
            return MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT;
        }
    }

    return reinterpret_cast<decltype(&ModpackLoader_GetVersion)>(sMPLGetversion)(outVariable);
}

ModpackLoaderStatus ModpackLoader_ParseModpackFromCurrentlyRunningWUHB(MPLModpackHandle *outHandle) {
    if (sModpackLoaderVersion == MODPACK_LOADER_MODULE_VERSION_ERROR) {
        return MODPACK_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sMPLParseModpackFromCurrentlyRunningWUHB == nullptr || sModpackLoaderVersion < 1) {
        return MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&ModpackLoader_ParseModpackFromCurrentlyRunningWUHB)>(sMPLParseModpackFromCurrentlyRunningWUHB)(outHandle);
}

ModpackLoaderStatus ModpackLoader_ParseModpackByPath(const char *path, MPLModpackHandle *outHandle) {
    if (sModpackLoaderVersion == MODPACK_LOADER_MODULE_VERSION_ERROR) {
        return MODPACK_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sMPLParseModpackByPath == nullptr || sModpackLoaderVersion < 1) {
        return MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&ModpackLoader_ParseModpackByPath)>(sMPLParseModpackByPath)(path, outHandle);
}

ModpackLoaderStatus ModpackLoader_CheckIfLaunchable(MPLModpackHandle handle, bool *readyToLaunchOut) {
    if (sModpackLoaderVersion == MODPACK_LOADER_MODULE_VERSION_ERROR) {
        return MODPACK_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sMPLCheckIfLaunchable == nullptr || sModpackLoaderVersion < 1) {
        return MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&ModpackLoader_CheckIfLaunchable)>(sMPLCheckIfLaunchable)(handle, readyToLaunchOut);
}

ModpackLoaderStatus ModpackLoader_LaunchModpack(MPLModpackHandle handle, bool *launchedOut) {
    if (sModpackLoaderVersion == MODPACK_LOADER_MODULE_VERSION_ERROR) {
        return MODPACK_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sMPLLaunchModpack == nullptr || sModpackLoaderVersion < 1) {
        return MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&ModpackLoader_LaunchModpack)>(sMPLLaunchModpack)(handle, launchedOut);
}
