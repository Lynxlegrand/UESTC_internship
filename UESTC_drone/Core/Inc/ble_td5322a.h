/*
 * ble_td5322a.h
 *
 *  Created on: Jul 2, 2025
 *      Author: hugoc
 */

#ifndef INC_BLE_TD5322A_H_
#define INC_BLE_TD5322A_H_

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"

void BLE_SendCommand(const char* cmd);

// Commandes de base
void BLE_AT(void);
void BLE_Reset(void);
void BLE_RestoreDefaults(void);
void BLE_Sleep(void);

// Nom et identifiants
void BLE_ReadVersion(void);
void BLE_ReadName(void);
void BLE_SetName(const char* name);
void BLE_ReadSPPName(void);
void BLE_SetSPPName(const char* name);

// Adresse MAC
void BLE_ReadMAC(void);
void BLE_ReadSPPMAC(void);
void BLE_SetBLEMAC(const char* mac12);
void BLE_SetSPPMAC(const char* mac12);

// Baudrate
void BLE_ReadBaudrate(void);
void BLE_SetBaudrate(uint32_t baudrate);

// Mode BLE / BLE+SPP
void BLE_ReadMode(void);
void BLE_SetMode(uint8_t mode);

// Sécurité
void BLE_SetPIN(const char* pin6digits);
void BLE_ReadPIN(void);
void BLE_SetSecurity(uint8_t level);
void BLE_ReadSecurity(void);

// Rôle
void BLE_ReadRole(void);
void BLE_SetRole(uint8_t role);

// UUIDs
void BLE_ReadUUID(void);
void BLE_SetUUID(uint8_t index, const char* uuid);
void BLE_SetTargetUUID(const char* uuid);

// Broadcasting (advertising)
void BLE_ReadAdvData(void);
void BLE_SetAdvData(const char* adv_data_hex);
void BLE_ReadAdvInterval(void);
void BLE_SetAdvInterval(uint16_t interval_ms);
void BLE_SetBroadcast(uint8_t enable);

// Indicateur LED
void BLE_SetLED(uint8_t mode);
void BLE_ReadLED(void);

// Puissance RF
void BLE_SetRFPower(uint8_t level);

// Low power
void BLE_SetLowPowerMode(uint8_t enable);
void BLE_ReadLowPowerMode(void);

// Logs
void BLE_SetLog(uint8_t enable);
void BLE_ReadLog(void);

// Canaux / modes
void BLE_EnterATMode(void);
void BLE_EnterBLEMode(void);

// Connexion / scan
void BLE_ConnectTo(const char* mac_addr);
void BLE_Disconnect(uint8_t mode);
void BLE_ScanStart(void);
void BLE_SetScanLimits(uint8_t count, uint8_t timeout_sec);
void BLE_SetScanParams(uint8_t active, uint8_t interval, uint8_t window);
void BLE_ClearBindings(void);
void BLE_SetAutoConnect(const char* mac);

// Structures
typedef struct {
	// Envoyer command
	void (*SendCommand)(const char* cmd);
	// Envoyer trame
	void (*SendTrame)(const uint8_t* cmd);
    // Commandes de base
    void (*AT)(void);
    void (*Reset)(void);
    void (*RestoreDefaults)(void);
    void (*Sleep)(void);

    // Nom & identifiants
    void (*ReadVersion)(void);
    void (*ReadName)(void);
    void (*SetName)(const char* name);
    void (*ReadSPPName)(void);
    void (*SetSPPName)(const char* name);

    // Adresse MAC
    void (*ReadMAC)(void);
    void (*ReadSPPMAC)(void);
    void (*SetBLEMAC)(const char* mac12);
    void (*SetSPPMAC)(const char* mac12);

    // Baudrate
    void (*ReadBaudrate)(void);
    void (*SetBaudrate)(uint32_t baudrate);

    // Mode BLE / BLE+SPP
    void (*ReadMode)(void);
    void (*SetMode)(uint8_t mode);

    // Sécurité
    void (*SetPIN)(const char* pin6digits);
    void (*ReadPIN)(void);
    void (*SetSecurity)(uint8_t level);
    void (*ReadSecurity)(void);

    // Rôle
    void (*ReadRole)(void);
    void (*SetRole)(uint8_t role);

    // UUIDs
    void (*ReadUUID)(void);
    void (*SetUUID)(uint8_t index, const char* uuid);
    void (*SetTargetUUID)(const char* uuid);

    // Broadcasting (advertising)
    void (*ReadAdvData)(void);
    void (*SetAdvData)(const char* adv_data_hex);
    void (*ReadAdvInterval)(void);
    void (*SetAdvInterval)(uint16_t interval_ms);
    void (*SetBroadcast)(uint8_t enable);

    // Indicateur LED
    void (*SetLED)(uint8_t mode);
    void (*ReadLED)(void);

    // RF Power
    void (*SetRFPower)(uint8_t level);

    // Low power
    void (*SetLowPowerMode)(uint8_t enable);
    void (*ReadLowPowerMode)(void);

    // Logs
    void (*SetLog)(uint8_t enable);
    void (*ReadLog)(void);

    // Canaux
    void (*EnterATMode)(void);
    void (*EnterBLEMode)(void);

    // Scan / Connexion
    void (*ScanStart)(void);
    void (*SetScanLimits)(uint8_t count, uint8_t timeout_sec);
    void (*SetScanParams)(uint8_t active, uint8_t interval, uint8_t window);
    void (*ClearBindings)(void);
    void (*SetAutoConnect)(const char* mac);
    void (*ConnectTo)(const char* mac);
    void (*Disconnect)(uint8_t mode);
} BLE_Interface_t;

// ble_td5322a.h
extern BLE_Interface_t BLE;


#endif /* INC_BLE_TD5322A_H_ */
