#ifndef TARGET_OS_H
#define TARGET_OS_H

// Configuration de l'OS cible pour la Nintendo 3DS (Horizon OS)

#define STR_TARGET_OS  "Horizon OS (3DS)"

// 1. Correction des erreurs #error : nanoFramework a besoin de savoir si on utilise un bootloader
#define TARGET_HAS_NANOBOOTER        0

// 2. On indique au framework qu'on n'utilise pas d'OS embarqué standard
#define CMSIS_V2                     0

// 3. On désactive TOUS les modules de stockage et de fichiers d'origine
// pour éviter qu'il cherche "Target_Windows_Storage.h"
#define HAS_FILESYSTEM               0
#define HAS_NETWORKING               0
#define NF_FEATURE_HAS_STORAGE       0
#define HAL_USE_SDC                  0
#define HAL_USE_WINDOWS_STORAGE      0

#endif // TARGET_OS_H