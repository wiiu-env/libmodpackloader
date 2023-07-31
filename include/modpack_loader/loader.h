#pragma once

#include <stdint.h>
#include <sys/iosupport.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ModpackLoaderStatus {
    MODPACK_LOADER_RESULT_SUCCESS               = 0,
    MODPACK_LOADER_RESULT_MODULE_NOT_FOUND      = -0x1,
    MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT = -0x2,
    MODPACK_LOADER_RESULT_UNSUPPORTED_VERSION   = -0x3,
    MODPACK_LOADER_RESULT_INVALID_ARGUMENT      = -0x10,
    MODPACK_LOADER_RESULT_NO_MEMORY             = -0x11,
    MODPACK_LOADER_RESULT_LIB_UNINITIALIZED     = -0x20,
    MODPACK_LOADER_RESULT_UNSUPPORTED_COMMAND   = -0x21,
    MODPACK_LOADER_RESULT_UNKNOWN_ERROR         = -0x1000,
} ModpackLoaderStatus;

typedef uint32_t ModpackLoaderVersion;

#define MODPACK_LOADER_MODULE_VERSION_ERROR 0xFFFFFFFF

const char *ModpackLoader_GetStatusStr(ModpackLoaderStatus status);

/**
 * This function has to be called before any other function of this lib (except ModpackLoader_GetVersion) can be used.
 *
 * @return  MODPACK_LOADER_RESULT_SUCCESS:                 The library has been initialized successfully. Other functions can now be used.
 *          MODPACK_LOADER_RESULT_MODULE_NOT_FOUND:        The module could not be found. Make sure the module is loaded.
 *          MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT:   The module is missing an expected export.
 *          MODPACK_LOADER_RESULT_UNSUPPORTED_VERSION:     The version of the loaded module is not compatible with this version of the lib.
 */
ModpackLoaderStatus ModpackLoader_InitLibrary();

ModpackLoaderStatus ModpackLoader_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded ModpackLoaderModulee.<br>
 * <br>
 * @param outVersion pointer to the variable where the version will be stored.
 *
 * @return MODPACK_LOADER_RESULT_SUCCESS:                  The API version has been store in the version ptr.<br>
 *         MODPACK_LOADER_RESULT_MODULE_NOT_FOUND:         The module could not be found. Make sure the module is loaded.<br>
 *         MODPACK_LOADER_RESULT_MODULE_MISSING_EXPORT:    The module is missing an expected export.<br>
 *         MODPACK_LOADER_RESULT_INVALID_ARGUMENT:         Invalid version pointer.<br>
 *         MODPACK_LOADER_RESULT_UNKNOWN_ERROR:            Retrieving the module version failed.
 */
ModpackLoaderStatus ModpackLoader_GetVersion(ModpackLoaderVersion *outVersion);

#ifdef __cplusplus
} // extern "C"
#endif