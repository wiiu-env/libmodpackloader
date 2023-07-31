#include "logger.h"
#include "modpack_loader/loader.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <sys/iosupport.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static ModpackLoaderStatus (*sMPLGetversion)(ModpackLoaderVersion *) = nullptr;

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