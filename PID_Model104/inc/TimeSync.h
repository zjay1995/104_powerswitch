#pragma once

#include "ConfigurationManager.h"
#include "Globals.h"
#include <WiFi.h>
#include "DS3231M.h"

void NTPTimeSync_Task(void* param);

class TimeSync : public ParamChangeListener
{

	String m_wifiSsid;
	String m_wifiPassword;

	DS3231M_Class m_ds3231mRtc;

	bool m_isNTPSyncRunning = false;

	TaskHandle_t m_task;

public:

	TimeSync();
	~TimeSync() = default;


	void initTimeFromRTC();

	void stopNTPSync();

	void startNTPSync();

	bool isNTCSyncRunning() const;

	void NTPSyncTask_run();

	void onParamChange(String param, String value);
};

