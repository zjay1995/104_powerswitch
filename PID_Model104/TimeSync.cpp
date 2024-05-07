#include "inc/TimeSync.h"

#include "inc/ConfigurationManager.h"
#include "inc/Globals.h"
#include <WiFi.h>
#include "DS3231M.h"


TimeSync::TimeSync()
{
    // DS3231M RTC
    if (!m_ds3231mRtc.begin())
    {
        Serial.print("Failed to init DS3231M RTC!!");
    }
}

void TimeSync::initTimeFromRTC()
{
    uint32_t now = m_ds3231mRtc.now().unixtime();

    struct timeval tv_now = { 0 };
    tv_now.tv_sec = now;
    settimeofday(&tv_now, NULL);

    //setenv("TZ", "EST5EDT", 1);

    struct tm timeinfo;

}

void TimeSync::stopNTPSync()
{
    m_isNTPSyncRunning = false;
}

void TimeSync::startNTPSync()
{
    if (m_isNTPSyncRunning)
        Serial.println("TimeSync :: startNTPSync() -- Task is already running!!");

    if (!xTaskCreatePinnedToCore(
            NTPTimeSync_Task,     // Function that should be called
            "NTPTimeSync_Task",   // Name of the task (for debugging)
            15000,					// Stack size (bytes)
            this,					// Parameter to pass
            1,						// Task priority
            &m_task,				// Task handle
            0						// Core you want to run the task on (0 or 1)
    )
            )
    {
        Serial.println("Failed to create TimeSync_Task!!!!");
        return;
    }

    m_isNTPSyncRunning = true;
}

bool TimeSync::isNTCSyncRunning() const {
    return m_isNTPSyncRunning;
}

void TimeSync::NTPSyncTask_run()
{

    Serial.println("Syncing with creds: " + m_wifiSsid + " " + m_wifiPassword);

    WiFi.begin(m_wifiSsid.c_str(), m_wifiPassword.c_str());

    int connectTries = 25;
    while (WiFi.status() != WL_CONNECTED || connectTries-- > 0) {

        if (!m_isNTPSyncRunning)
        {
            WiFi.mode(WIFI_OFF);
            return;
        }

        Serial.print(".");
        vTaskDelay(200);
    }

    Serial.println(WiFi.dnsIP());
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), IPAddress(8, 8, 8, 8));

    struct timeval tv_now = { 0 };
    struct timezone tz = { 0 };
    settimeofday(&tv_now, &tz);

    configTime(0, 0, "pool.ntp.org");
    setenv("TZ", "EST5EDT", 1);
    tzset();

    while (tv_now.tv_sec < 10)
    {
        if (!m_isNTPSyncRunning)
            return;

        gettimeofday(&tv_now, NULL);
        vTaskDelay(50);
    }

    bool timeSyncSuccess = false;
    if (tv_now.tv_sec < 50)
    {
        Serial.println("FAILED TO SYNC TIME!");
    }
    else
        timeSyncSuccess = true;

    WiFi.mode(WIFI_OFF);
    while (WiFi.getMode() != WIFI_OFF)
    {
        if (!m_isNTPSyncRunning)
            return;

        vTaskDelay(10);
    }


    if (!timeSyncSuccess)
    {
        m_isNTPSyncRunning = false;
        return;
    }

    Serial.println("TIME IS SYNCED!!!!");

    gettimeofday(&tv_now, NULL);

    m_ds3231mRtc.adjust(DateTime(tv_now.tv_sec));

    struct tm timeinfo;
    getLocalTime(&timeinfo, 10);

    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    m_isNTPSyncRunning = false;
}


void TimeSync::onParamChange(String param, String value)
{
    m_wifiSsid = "22CDPro";
    m_wifiPassword = "00525508";
}


void NTPTimeSync_Task(void* param)
{
    TimeSync* timeSync = (TimeSync*)param;

    timeSync->NTPSyncTask_run();

    vTaskDelete(0);
}
