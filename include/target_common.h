#ifndef TARGET_COMMON_H
#define TARGET_COMMON_H

// Configuration commune pour le portage Nintendo 3DS

#define TARGET_NAME "Nintendo 3DS"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_BUILD 0

// Désactivation des options avancées de nanoFramework qui demandent trop de dépendances
#define NF_FEATURE_DEBUGGER          0
#define NF_FEATURE_INTEROP           0
#define NF_FEATURE_SERIALIZATION     0

#endif // TARGET_COMMON_H