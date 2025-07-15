/*
 * ble_td5322a.c
 *
 *  Created on: Jul 2, 2025
 *      Author: hugoc
 */

#include "ble_td5322a.h"

#define huart_BLE huart1
extern UART_HandleTypeDef huart_BLE;

// Fonction d'envoi générique
void BLE_SendCommand(const char* cmd) {
    HAL_UART_Transmit(&huart_BLE, (uint8_t*)cmd, strlen(cmd), HAL_MAX_DELAY);
}

void BLE_SendTrame(const uint8_t* cmd) {
    HAL_UART_Transmit(&huart_BLE, cmd, strlen((const char*)cmd), HAL_MAX_DELAY);
}

// Test AT
void BLE_AT(void) {
    BLE_SendCommand("AT\r\n");
}

// Redémarrage du module
void BLE_Reset(void) {
    BLE_SendCommand("AT+RESET\r\n");
}

// Restauration usine
void BLE_RestoreDefaults(void) {
    BLE_SendCommand("AT+RESTORE\r\n");
}

// Lire version du firmware
void BLE_ReadVersion(void) {
    BLE_SendCommand("AT+VER\r\n");
}

// Lire nom BLE
void BLE_ReadName(void) {
    BLE_SendCommand("AT+BLENAME\r\n");
}

// Définir nom BLE
void BLE_SetName(const char* name) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+BLENAME=%s\r\n", name);
    BLE_SendCommand(cmd);
}

// Lire l’adresse MAC BLE
void BLE_ReadMAC(void) {
    BLE_SendCommand("AT+BLEMAC\r\n");
}

// Lire le rôle actuel
void BLE_ReadRole(void) {
    BLE_SendCommand("AT+ROLE\r\n");
}

// Définir rôle (0 = slave, 1 = master)
void BLE_SetRole(uint8_t role) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+ROLE=%d\r\n", (char)role);
    BLE_SendCommand(cmd);
}

// Définir mot de passe
void BLE_SetPIN(const char* pin6digits) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+PIN=%s\r\n", pin6digits);
    BLE_SendCommand(cmd);
}

// Définir le niveau de sécurité
void BLE_SetSecurity(uint8_t level) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+SECURITY=%d\r\n", (char)level);
    BLE_SendCommand(cmd);
}

// Lire niveau de sécurité
void BLE_ReadSecurity(void) {
    BLE_SendCommand("AT+SECURITY\r\n");
}

// Lire PIN actuel
void BLE_ReadPIN(void) {
    BLE_SendCommand("AT+PIN\r\n");
}

// Passer en mode AT
void BLE_EnterATMode(void) {
    BLE_SendCommand("AT>9\r\n");
}

// Revenir au mode BLE
void BLE_EnterBLEMode(void) {
    BLE_SendCommand("AT>8\r\n");
}

// Activer/désactiver le log
void BLE_SetLog(uint8_t enable) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+LOG=%d\r\n", (char)enable);
    BLE_SendCommand(cmd);
}

// Lire état du log
void BLE_ReadLog(void) {
    BLE_SendCommand("AT+LOG\r\n");
}

// Réglage LED d’état
void BLE_SetLED(uint8_t mode) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+LED=%d\r\n", (char)mode);
    BLE_SendCommand(cmd);
}

// Lire mode LED
void BLE_ReadLED(void) {
    BLE_SendCommand("AT+LED\r\n");
}

// Définir puissance d’émission (0 à 9)
void BLE_SetRFPower(uint8_t level) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+RFPWR=%d\r\n", (char)level);
    BLE_SendCommand(cmd);
}

// Lire UUID
void BLE_ReadUUID(void) {
    BLE_SendCommand("AT+UUID\r\n");
}

// Modifier UUID d’un service ou caractéristique
void BLE_SetUUID(uint8_t index, const char* uuid) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+UUID=%d,%s\r\n", (char)index, (char)uuid);
    BLE_SendCommand(cmd);
    BLE_Reset();  // UUID settings require reboot
}

// Lire données du paquet de broadcast
void BLE_ReadAdvData(void) {
    BLE_SendCommand("AT+ADVDATA\r\n");
}

// Définir données broadcast personnalisées
void BLE_SetAdvData(const char* adv_data_hex) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+ADVDATA=%s\r\n", adv_data_hex);
    BLE_SendCommand(cmd);
}

// Lire intervalle broadcast
void BLE_ReadAdvInterval(void) {
    BLE_SendCommand("AT+ADVPARAM\r\n");
}

// Définir intervalle broadcast (10~4000 ms)
void BLE_SetAdvInterval(uint16_t interval_ms) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+ADVPARAM=%d\r\n", (char)interval_ms);
    BLE_SendCommand(cmd);
}

// Activer/désactiver le broadcast
void BLE_SetBroadcast(uint8_t enable) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+ADV=%d\r\n", (char)enable);
    BLE_SendCommand(cmd);
}

// Activer ou désactiver le mode low power
void BLE_SetLowPowerMode(uint8_t enable) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+LPM=%d\r\n", (char)enable);
    BLE_SendCommand(cmd);
}

// Lire état low power
void BLE_ReadLowPowerMode(void) {
    BLE_SendCommand("AT+LPM\r\n");
}

// Rechercher des périphériques BLE autour (master mode)
void BLE_Scan(void) {
    BLE_SendCommand("AT+SCAN=1\r\n");
}

// Connexion à un périphérique BLE (master mode)
void BLE_ConnectTo(const char* mac_addr) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+CONN=%s\r\n", mac_addr);
    BLE_SendCommand(cmd);
}

// Déconnexion (nécessite d’abord AT>9)
void BLE_Disconnect(uint8_t mode) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+DISC=%d\r\n", (char)mode);
    BLE_SendCommand(cmd);
}

// Mise en veille (soft shutdown)
void BLE_Sleep(void) {
    BLE_SendCommand("AT+SLEEP\r\n");
}
// Lire le baudrate
void BLE_ReadBaudrate(void) {
    BLE_SendCommand("AT+BAUD\r\n");
}

// Définir le baudrate (ex: 9600, 115200, etc.)
void BLE_SetBaudrate(uint32_t baudrate) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+BAUD=%lu\r\n", (char)baudrate);
    BLE_SendCommand(cmd);
}

// Lire le mode (0 = BLE / 1 = BLE&SPP)
void BLE_ReadMode(void) {
    BLE_SendCommand("AT+MODE\r\n");
}

// Définir le mode BLE ou BLE+SPP
void BLE_SetMode(uint8_t mode) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+MODE=%d\r\n", (char)mode);
    BLE_SendCommand(cmd);
}

// Lire nom SPP
void BLE_ReadSPPName(void) {
    BLE_SendCommand("AT+SPPNAME\r\n");
}

// Définir nom SPP
void BLE_SetSPPName(const char* name) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+SPPNAME=%s\r\n", name);
    BLE_SendCommand(cmd);
}

// Lire MAC SPP
void BLE_ReadSPPMAC(void) {
    BLE_SendCommand("AT+SPPMAC\r\n");
}

// Définir MAC BLE
void BLE_SetBLEMAC(const char* mac12) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+BLEMAC=%s\r\n", mac12);
    BLE_SendCommand(cmd);
}

// Définir MAC SPP
void BLE_SetSPPMAC(const char* mac12) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+SPPMAC=%s\r\n", mac12);
    BLE_SendCommand(cmd);
}

// Définir UUID cible pour connexion (client mode)
void BLE_SetTargetUUID(const char* uuid) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+TARGETUUID=%s\r\n", uuid);
    BLE_SendCommand(cmd);
}

// Scanner les périphériques BLE
void BLE_ScanStart(void) {
    BLE_SendCommand("AT+SCAN=1\r\n");
}

// Configurer paramètres de scan (nombre, timeout)
void BLE_SetScanLimits(uint8_t count, uint8_t timeout_sec) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+SCANM=%d,%d\r\n", (char)count, (char)timeout_sec);
    BLE_SendCommand(cmd);
}

// Configurer scan params (mode, interval, window)
void BLE_SetScanParams(uint8_t active, uint8_t interval, uint8_t window) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+SCANPARAM=%d,%d,%d\r\n", (char)active, (char)interval, (char)window);
    BLE_SendCommand(cmd);
}

// Effacer les liaisons enregistrées
void BLE_ClearBindings(void) {
    BLE_SendCommand("AT+CLRBIND\r\n");
}

// Configurer auto-connexion (client mode uniquement)
void BLE_SetAutoConnect(const char* mac) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+SERVER=%s\r\n", mac);
    BLE_SendCommand(cmd);
    BLE_Reset();  // Reboot pour appliquer
}


BLE_Interface_t BLE = {
	// Envoyer trame
	.SendCommand         = BLE_SendCommand,
	.SendTrame           = BLE_SendTrame,
    // Commandes de base
    .AT                  = BLE_AT,
    .Reset               = BLE_Reset,
    .RestoreDefaults     = BLE_RestoreDefaults,
    .Sleep               = BLE_Sleep,

    // Nom & identifiants
    .ReadVersion         = BLE_ReadVersion,
    .ReadName            = BLE_ReadName,
    .SetName             = BLE_SetName,
    .ReadSPPName         = BLE_ReadSPPName,
    .SetSPPName          = BLE_SetSPPName,

    // Adresse MAC
    .ReadMAC             = BLE_ReadMAC,
    .ReadSPPMAC          = BLE_ReadSPPMAC,
    .SetBLEMAC           = BLE_SetBLEMAC,
    .SetSPPMAC           = BLE_SetSPPMAC,

    // Baudrate
    .ReadBaudrate        = BLE_ReadBaudrate,
    .SetBaudrate         = BLE_SetBaudrate,

    // Mode BLE / BLE+SPP
    .ReadMode            = BLE_ReadMode,
    .SetMode             = BLE_SetMode,

    // Sécurité
    .SetPIN              = BLE_SetPIN,
    .ReadPIN             = BLE_ReadPIN,
    .SetSecurity         = BLE_SetSecurity,
    .ReadSecurity        = BLE_ReadSecurity,

    // Rôle
    .ReadRole            = BLE_ReadRole,
    .SetRole             = BLE_SetRole,

    // UUIDs
    .ReadUUID            = BLE_ReadUUID,
    .SetUUID             = BLE_SetUUID,
    .SetTargetUUID       = BLE_SetTargetUUID,

    // Broadcasting
    .ReadAdvData         = BLE_ReadAdvData,
    .SetAdvData          = BLE_SetAdvData,
    .ReadAdvInterval     = BLE_ReadAdvInterval,
    .SetAdvInterval      = BLE_SetAdvInterval,
    .SetBroadcast        = BLE_SetBroadcast,

    // LED
    .SetLED              = BLE_SetLED,
    .ReadLED             = BLE_ReadLED,

    // RF
    .SetRFPower          = BLE_SetRFPower,

    // Low power
    .SetLowPowerMode     = BLE_SetLowPowerMode,
    .ReadLowPowerMode    = BLE_ReadLowPowerMode,

    // Logs
    .SetLog              = BLE_SetLog,
    .ReadLog             = BLE_ReadLog,

    // Canaux
    .EnterATMode         = BLE_EnterATMode,
    .EnterBLEMode        = BLE_EnterBLEMode,

    // Scan / Connexion
    .ScanStart           = BLE_ScanStart,
    .SetScanLimits       = BLE_SetScanLimits,
    .SetScanParams       = BLE_SetScanParams,
    .ClearBindings       = BLE_ClearBindings,
    .SetAutoConnect      = BLE_SetAutoConnect,
    .ConnectTo           = BLE_ConnectTo,
    .Disconnect          = BLE_Disconnect
};

