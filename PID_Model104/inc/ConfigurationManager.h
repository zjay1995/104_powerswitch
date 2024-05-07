#pragma once
#include <Arduino.h>

class SleepTimer;
class GasManager;
class ParamChangeListener;


class ParamChangeListener
{
public:

	virtual void onParamChange(String param, String value) = 0;

};


class ConfigurationManager : public ParamChangeListener
{

    const uint16_t EEPROM_SIZE = sizeof(int) * (1 + 1 + 3) + // Timer, Selected Gas Index, Range, Alarm, Calibration
                                 sizeof(double) * 8 + // Gas calibration parameters (intercept + slope for 4 gases)
                                 64 + // Device ID
                                 32 * 2; // Wi-Fi SSID and Password
	
    const uint8_t EEPROM_TIMER_OFFSET = 0;
    const uint8_t EEPROM_GAS_SELECT_OFFSET = 4;

    const uint8_t EEPROM_GAS_INTERCEPT_OFFSET = 8;
    const uint8_t EEPROM_GAS_SLOPE_OFFSET = 16;

    const uint8_t EEPROM_GAS_INTERCEPT2_OFFSET = 24;
    const uint8_t EEPROM_GAS_SLOPE2_OFFSET = 32;

    const uint8_t EEPROM_GAS_INTERCEPT3_OFFSET = 40;
    const uint8_t EEPROM_GAS_SLOPE3_OFFSET = 48;

    const uint8_t EEPROM_GAS_INTERCEPT4_OFFSET = 56;
    const uint8_t EEPROM_GAS_SLOPE4_OFFSET = 64;


    //const uint8_t EEPROM_GAS_AIR_TC_OFFSET = 28;
    //const uint8_t EEPROM_GAS_O2_TC_OFFSET = 36;
    //const uint8_t EEPROM_GAS_N2_TC_OFFSET = 44;
    //const uint8_t EEPROM_GAS_He_TC_OFFSET = 52;
    //const uint8_t EEPROM_GAS_H2_TC_OFFSET = 60;
    //const uint8_t EEPROM_GAS_ArCH4_TC_OFFSET = 68;
    const uint8_t EEPROM_DEVICE_ID_OFFSET = 84;			//64 chars
    const uint8_t EEPROM_WIFI_SSID_OFFSET = 148;		//32 chars
    const uint8_t EEPROM_WIFI_PASSWORD_OFFSET = 180;	//32 chars
    const uint8_t EEPROM_RANGE_OFFSET = 72;	//uint16_t
    const uint8_t EEPROM_Calvalue_OFFSET = 80;	//uint16_t
    const uint8_t EEPROM_ALARM_OFFSET = 76;	//uint16_t


    const uint8_t EEPROM_GAS_AIR_TC_OFFSET = 999;
    const uint8_t EEPROM_GAS_O2_TC_OFFSET = 999;
    const uint8_t EEPROM_GAS_N2_TC_OFFSET = 999;
    const uint8_t EEPROM_GAS_He_TC_OFFSET = 999;
    const uint8_t EEPROM_GAS_H2_TC_OFFSET = 999;
    const uint8_t EEPROM_GAS_ArCH4_TC_OFFSET = 999;



    bool m_loadAllInProgress = false;

	std::vector<ParamChangeListener*> m_paramChangeListeners;

	String m_wifiSsid;
	String m_wifiPassword;
	String m_deviceId;
	String m_wifiRtLogFreq;
	String m_flashLogFreq;
	String m_mqttServerUrl;

public:

	ConfigurationManager();
	
	void init();
	
	void loadFromEEPROM();

	void saveTimerIntervalToEEPROM(int interval, bool doCommit = true);

    void saveRangeToEEPROM(int range, bool doCommit = true);
    void saveAlarmToEEPROM(int alarm, bool doCommit = true);

    void saveCalvalueToEEPROM(int calvalue, bool doCommit = true);

    void saveGasSelectedToEEPROM(int gasIndex, bool doCommit = true);

	void saveInterceptToEEPROM(double intercept, bool doCommit = true);
    void saveSlopeToEEPROM(double slope, bool doCommit = true);
    void saveGasThermalConductivity(String gasName, double tc, bool doCommit = true);
	void saveWifiSSID(String ssid, bool doCommit = true);
	void saveWifiPassword(String password, bool doCommit = true);

	void saveDeviceId(String id, bool doCommit = true);

	// Getters
	String getWifiSsid() const { return m_wifiSsid; }
	String getWifiPassword() const { return m_wifiPassword; }
	String getDeviceId() const { return m_deviceId; }



	void setupEEPROM();
	
	void clearEEPROM();

	void onParamChange(String param, String value);


	void addParamChangeListener(ParamChangeListener* l);
	void notifyParamChanged(String param, String value);

};
