#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

// Liaison de la gestion du temps nanoFramework avec l'horloge de la 3DS

#ifdef __cplusplus
extern "C" {
#endif

// On déclare les fonctions que notre fichier nano_3DS.cpp fournit déjà
uint64_t platform_get_ticks();
void platform_sleep(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif // TARGET_HAL_TIME_H