 #include "inc/ConfigurationManager.h"
#include <EEPROM.h>

#include "inc/Globals.h"
#include "inc/SleepTimer.h"
#include "inc/GasManager.h"

ConfigurationManager::ConfigurationManager()
{

}

void ConfigurationManager::init()
{
	setupEEPROM();
}

void ConfigurationManager::loadFromEEPROM()
{
	m_loadAllInProgress = true;
	
	int timerInterval = EEPROM.readInt(EEPROM_TIMER_OFFSET);
	Serial.println("config: timer interval: " + String(timerInterval));
	
	//m_sleepTimer->selectIntervalByValue( timerInterval );	

    int range = EEPROM.read(72);
    Serial.println("config: range: " + String(range));
    notifyParamChanged(c_RANGE_PARAM_NAME, String(range));

    int alarm = EEPROM.read(76);
    Serial.println("config: alarm: " + String(alarm));
    notifyParamChanged(c_ALARM_PARAM_NAME, String(alarm));

    int gasIndex = EEPROM.readInt(4);
    Serial.println("config: gas index: " + String(gasIndex));
    notifyParamChanged(c_GASINDEX_PARAM_NAME, String(gasIndex));


	// INTERCEPT
	double intercept = -1;
	intercept = EEPROM.readDouble(EEPROM_GAS_INTERCEPT_OFFSET);		
	if(intercept != -1)
		notifyParamChanged(c_INTERCEPT_PARAM_NAME, String(intercept));
	Serial.println("config: intercept: " + String(intercept));

    double intercept2 = -1;
    intercept2 = EEPROM.readDouble(EEPROM_GAS_INTERCEPT2_OFFSET);
    if(intercept2 != -1)
        notifyParamChanged(c_INTERCEPT2_PARAM_NAME, String(intercept2));
    Serial.println("config: intercept2: " + String(intercept2));

    double intercept3 = -1;
    intercept3 = EEPROM.readDouble(EEPROM_GAS_INTERCEPT3_OFFSET);
    if(intercept3 != -1)
        notifyParamChanged(c_INTERCEPT3_PARAM_NAME, String(intercept3));
    Serial.println("config: intercept3: " + String(intercept3));

    double intercept4 = -1;
    intercept4 = EEPROM.readDouble(EEPROM_GAS_INTERCEPT4_OFFSET);
    if(intercept4 != -1)
        notifyParamChanged(c_INTERCEPT4_PARAM_NAME, String(intercept4));
    Serial.println("config: intercept4: " + String(intercept4));

	//Slope
    double slope = -1;
    slope = EEPROM.readDouble(EEPROM_GAS_SLOPE_OFFSET);
    if(slope != -1)
        notifyParamChanged(c_SLOPE_PARAM_NAME, String(slope));
    Serial.println("config: slope: " + String(slope));


    //slope2
    double slope2 = -1;
    slope2 = EEPROM.readDouble(EEPROM_GAS_SLOPE2_OFFSET);
    if(slope2 != -1)
        notifyParamChanged(c_SLOPE2_PARAM_NAME, String(slope2));
    Serial.println("config: slope2: " + String(slope2));

    double slope3 = -1;
    slope3 = EEPROM.readDouble(EEPROM_GAS_SLOPE3_OFFSET);
    if(slope3 != -1)
        notifyParamChanged(c_SLOPE3_PARAM_NAME, String(slope3));
    Serial.println("config: slope3: " + String(slope3));

    double slope4 = -1;
    slope4 = EEPROM.readDouble(EEPROM_GAS_SLOPE4_OFFSET);
    if(slope4 != -1)
        notifyParamChanged(c_SLOPE4_PARAM_NAME, String(slope4));
    Serial.println("config: slope4: " + String(slope4));

	/*
		Gas TCs
	*/

	double AirTC = -1;
	AirTC =	EEPROM.readDouble(EEPROM_GAS_AIR_TC_OFFSET);
	if(AirTC != -1)
		notifyParamChanged(c_GAS_AIR_PARAM_NAME, String(AirTC));
	Serial.println("config: Air TC: " + String(AirTC));
	
	double O2TC = -1;
	O2TC = EEPROM.readDouble(EEPROM_GAS_O2_TC_OFFSET);
	if(O2TC != -1)
		notifyParamChanged(c_GAS_O2_PARAM_NAME, String(O2TC));
	Serial.println("config: O2 TC: " + String(O2TC));		
	
	double N2TC = -1;
	N2TC = EEPROM.readDouble(EEPROM_GAS_N2_TC_OFFSET);	
	if(N2TC != -1)
		notifyParamChanged(c_GAS_N2_PARAM_NAME, String(N2TC));
	Serial.println("config: N2 TC: " + String(N2TC));
	
	double HeTC = -1;
	HeTC = EEPROM.readDouble(EEPROM_GAS_He_TC_OFFSET);
	if(HeTC != -1)
		notifyParamChanged(c_GAS_He_PARAM_NAME, String(HeTC));
	Serial.println("config: He TC: " + String(HeTC));
	
	double H2TC = -1;
	H2TC = EEPROM.readDouble(EEPROM_GAS_H2_TC_OFFSET);		
	if(H2TC != -1)
		notifyParamChanged(c_GAS_H2_PARAM_NAME, String(H2TC));
	Serial.println("config: H2 TC: " + String(H2TC));
	
	double ArCH4TC = -1;
	ArCH4TC = EEPROM.readDouble(EEPROM_GAS_ArCH4_TC_OFFSET);		
	if(ArCH4TC != -1)
		notifyParamChanged(c_GAS_ARCH4_PARAM_NAME, String(ArCH4TC));
	Serial.println("config: ArCH4 TC: " + String(ArCH4TC));
	
	///////////////

//	// WiFi SSID
//	char wifiSsid[32] = { 0 };
//	size_t readSsid = EEPROM.readString(EEPROM_WIFI_SSID_OFFSET, wifiSsid, 31);
//	if (readSsid > 0)
//		notifyParamChanged(c_WIFI_SSID_PARAM_NAME, String(wifiSsid));
//	Serial.println("config: WiFi SSID: " + String(wifiSsid));
//	m_wifiSsid = wifiSsid;
//
//	// WiFi PASSWORD
//	char wifiPsw[32] = { 0 };
//	size_t readPsw = EEPROM.readString(EEPROM_WIFI_PASSWORD_OFFSET, wifiPsw, 31);
//	if (readPsw > 0)
//		notifyParamChanged(c_WIFI_PASSWORD_PARAM_NAME, String(wifiPsw));
//	Serial.println("config: WiFi PSW: " + String(wifiPsw));
//	m_wifiPassword = wifiPsw;



	/////////////////

	m_loadAllInProgress = false;

	return;		
}

void ConfigurationManager::addParamChangeListener(ParamChangeListener* l)
{
	m_paramChangeListeners.push_back(l);
}

void ConfigurationManager::notifyParamChanged(String param, String value)
{
	for (auto& l : m_paramChangeListeners)
		l->onParamChange(param, value);
}

void ConfigurationManager::onParamChange(String param, String value)
{

	if (param.equals(c_INTERCEPT_PARAM_NAME))
	{
		Serial.println("EEPROM save INTERCEPT: " + value);
		EEPROM.writeDouble(EEPROM_GAS_INTERCEPT_OFFSET, value.toDouble());
	}
    else if (param.equals(c_SLOPE_PARAM_NAME))
    {
        Serial.println("EEPROM save SLOPE: " + value);
        EEPROM.writeDouble(EEPROM_GAS_SLOPE_OFFSET, value.toDouble());
    }


	if (!EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");

	// This is WebServer changing params so notify others too!
	notifyParamChanged(param, value);
}



void ConfigurationManager:: saveInterceptToEEPROM(double intercept, bool doCommit)
{
	if(m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveInterceptToEEPROM: " + String(intercept));
	size_t written = EEPROM.writeDouble(EEPROM_GAS_INTERCEPT_OFFSET, intercept);
	Serial.println("EEPROM written: " + String(written));
	if(doCommit && !EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}

void ConfigurationManager:: saveSlopeToEEPROM(double slope, bool doCommit)
{
    if(m_loadAllInProgress)
        return;
    Serial.println("EEPROM saveSlopeToEEPROM: " + String(slope));
    size_t written = EEPROM.writeDouble(EEPROM_GAS_SLOPE_OFFSET, slope);
    Serial.println("EEPROM written: " + String(written));
    if(doCommit && !EEPROM.commit())
        Serial.println("EEPROM commit ERROR!!!");
}


void ConfigurationManager::saveTimerIntervalToEEPROM(int interval, bool doCommit)
{	
	if(m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveTimerIntervalToEEPROM: " + String(interval));
	size_t written = EEPROM.put(0, interval);
	Serial.println("EEPROM written: " + String(written));
	if(doCommit && !EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}

 void ConfigurationManager::saveRangeToEEPROM(int range, bool doCommit)
 {

     Serial.println("EEPROM saveRangeToEEPROM: " + String(range));
     EEPROM.writeInt(72, range);
     EEPROM.commit();
 }

 void ConfigurationManager::saveAlarmToEEPROM(int alarm, bool doCommit)
 {

     Serial.println("EEPROM saveAlarmToEEPROM: " + String(alarm));
     EEPROM.writeInt(76, alarm);
     EEPROM.commit();
 }


 void ConfigurationManager::saveCalvalueToEEPROM(int calvalue, bool doCommit)
 {

     Serial.println("EEPROM saveCalvalueToEEPROM: " + String(calvalue));
     EEPROM.writeInt(80, calvalue);
     EEPROM.commit();

 }
// void ConfigurationManager::saveGasSelectedToEEPROM(int gasIndex, bool doCommit)
// {
//     if(m_loadAllInProgress)
//         return;
//     Serial.println("EEPROM saveGasSelectedToEEPROM: " + String(gasIndex));
//     size_t written = EEPROM.put(0, gasIndex);
//     Serial.println("EEPROM written: " + String(written));
//     if(doCommit && !EEPROM.commit())
//         Serial.println("EEPROM commit ERROR!!!");
// }


 void ConfigurationManager::saveWifiSSID(String ssid, bool doCommit)
{
	if (m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveWifiSSID: " + ssid);
	size_t written = EEPROM.writeString(EEPROM_WIFI_SSID_OFFSET, ssid.c_str());
	Serial.println("EEPROM written: " + String(written));
	if (doCommit && !EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}

void ConfigurationManager::saveWifiPassword(String password, bool doCommit)
{
	if (m_loadAllInProgress)
		return;
	Serial.println("EEPROM saveWifiPassword: " + password);
	size_t written = EEPROM.writeString(EEPROM_WIFI_PASSWORD_OFFSET, password.c_str());
	Serial.println("EEPROM written: " + String(written));
	if (doCommit && !EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}




void ConfigurationManager::saveDeviceId(String id, bool doCommit)
{
	if (m_loadAllInProgress)
		return;

	Serial.println("EEPROM saveDeviceId: " + id);
	size_t written = EEPROM.writeString(EEPROM_DEVICE_ID_OFFSET, id.c_str());
	Serial.println("EEPROM written: " + String(written));
	if (doCommit && !EEPROM.commit())
		Serial.println("EEPROM commit ERROR!!!");
}

void ConfigurationManager::setupEEPROM()
{
	if (!EEPROM.begin(EEPROM_SIZE))
	{
		Serial.println("EEPROM start failed");
		delay(60000);
		return;
	}

	Serial.println("EEPROM begin!!");
}

void ConfigurationManager::clearEEPROM()
{
	for (size_t i = 0; i < EEPROM_SIZE; ++i)
		EEPROM.writeByte(i, 0xFF);
	EEPROM.commit();
}
