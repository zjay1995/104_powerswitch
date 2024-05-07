#pragma once

#include "GasManager.h"
#include "MenuRenderer.h"
#include "SleepTimer.h"
#include "RangeSet.h"
#include "AlarmSet.h"
#include "HourSet.h"
#include "MinuteSet.h"

#include "CalvalueSet.h"
#include "DataLogger.h"
#include "TimeSync.h"
#include "DataSource.h"


class Menu
{
protected:

    String m_menuName;
    String m_parentMenuName;

    MenuRenderer* m_menuRenderer;

public:

    Menu(String name, String parentName, MenuRenderer* renderer) : m_menuName(name),
                                                                   m_parentMenuName(parentName),
                                                                   m_menuRenderer(renderer)
    {

    }

    virtual ~Menu() = default;


    String getName() { return m_menuName; }
    String getParentName() { return m_parentMenuName; }

    virtual void action() = 0;

    virtual void render()
    {
        m_menuRenderer->render(this);
    }

};

class GasMenuItem : public Menu
{
    GasManager* m_gasManager;
    int m_gasIndex;

public:

    GasMenuItem(String name, String parentName, int gasIndex, GasManager* gasManager, MenuRenderer* renderer) : Menu(name, parentName, renderer),
                                                                                                                m_gasManager(gasManager),
                                                                                                                m_gasIndex(gasIndex)
    {

    }

    void action()
    {
        m_gasManager->selectGasByIndex(m_gasIndex);
    }

};

class RunMenuItem : public Menu
{
    GasManager* m_gasManager;
    int m_menuIndex;

public:

    RunMenuItem(String name, String parentName,int menuIndex, GasManager* gasManager, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_gasManager(gasManager),
              m_menuIndex(menuIndex)
    {

    }

    void action()
    {
        m_gasManager->printinfo();
    }

};

class SleepTimerMenuItem : public Menu
{
    SleepTimer* m_sleepTimer;

    int m_intervalIndex;

public:

    SleepTimerMenuItem(String name, String parentName, int intervalIndex, SleepTimer* sleepTimer, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_sleepTimer(sleepTimer),
              m_intervalIndex(intervalIndex)
    {

    }

    void action()
    {
        m_sleepTimer->selectIntervalByIndex(m_intervalIndex);
    }
};



class RangeMenuItem : public Menu
{
    Range* m_range;

    int m_rangeIndex;

public:

    RangeMenuItem(String name, String parentName, int rangeIndex, Range* range, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_range(range),
              m_rangeIndex(rangeIndex)
    {

    }

    void action()
    {
        m_range->selectRangeByIndex(m_rangeIndex);
    }

    void render()
    {
        m_menuRenderer->render(this);
    }

};

class AlarmMenuItem : public Menu
{
    Alarm* m_alarm;

    int m_alarmIndex;

public:

    AlarmMenuItem(String name, String parentName, int alarmIndex, Alarm* alarm, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_alarm(alarm),
              m_alarmIndex(alarmIndex)
    {

    }

    void action()
    {
        m_alarm->selectAlarmByIndex(m_alarmIndex);
    }
};


class HourMenuItem : public Menu
{
    Hour* m_hour;

    int m_hourIndex;

public:

    HourMenuItem(String name, String parentName, int hourIndex, Hour* hour, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_hour(hour),
              m_hourIndex(hourIndex)
    {

    }

    void action()
    {
        m_hour->selectHourByIndex(m_hourIndex);
    }
};


class MinuteMenuItem : public Menu
{
    Minute* m_minute;

    int m_minuteIndex;

public:

    MinuteMenuItem(String name, String parentName, int minuteIndex, Minute* minute, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_minute(minute),
              m_minuteIndex(minuteIndex)
    {

    }

    void action()
    {
        m_minute->selectMinuteByIndex(m_minuteIndex);
    }
};


class CalvalueMenuItem : public Menu
{
    Calvalue* m_calvalue  ;

    int m_calvalueIndex;

public:

    CalvalueMenuItem(String name, String parentName, int calvalueIndex, Calvalue* calvalue, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_calvalue(calvalue),
              m_calvalueIndex(calvalueIndex)
    {

    }

    void action()
    {
        m_calvalue->selectCalvalueByIndex(m_calvalueIndex);
    }

};



class DataLoggerFlashStoreMenuItem : public Menu
{
    DataLogger* m_dataLogger;

public:

    DataLoggerFlashStoreMenuItem(String name, String parentName, DataLogger* dataLogger, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_dataLogger(dataLogger)
    {

    }

    void action()
    {
        if( m_dataLogger->isFlashStoreSessionRunning() )
            m_dataLogger->stopFlashStoreSession();
        else
            m_dataLogger->startFlashStoreSession();
    }

};

class WiFiDumpMenuItem : public Menu
{
    DataLogger* m_dataLogger;

public:

    WiFiDumpMenuItem(String name, String parentName, DataLogger* dataLogger, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_dataLogger(dataLogger)
    {

    }

    void action()
    {
        if (m_dataLogger->isWiFiDumpRunning())
            m_dataLogger->stopWiFiDumpSession();
        else
            m_dataLogger->startWiFiDumpSession();
    }

};

class WiFiRealTimeDumpMenuItem : public Menu
{
    DataLogger* m_dataLogger;

public:

    WiFiRealTimeDumpMenuItem(String name, String parentName, DataLogger* dataLogger, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_dataLogger(dataLogger)
    {

    }

    void action()
    {
        if (m_dataLogger->isWiFiRealTimeDumpRunning())
            m_dataLogger->stopWiFiRealTimeDumpSession();
        else
            m_dataLogger->startWiFiRealTimeDumpSession();
    }

};

class NTPSyncMenuItem : public Menu
{

    TimeSync* m_timeSync;

public:

    NTPSyncMenuItem(String name, String parentName, TimeSync* timeSync, MenuRenderer* renderer)
            : Menu(name, parentName, renderer)
    {
        m_timeSync = timeSync;
    }

    void action()
    {
        if(!m_timeSync->isNTCSyncRunning())
            m_timeSync->startNTPSync();
        else
        {
            m_timeSync->stopNTPSync();
        }

    }

};




class InfoMenuItem : public Menu
{

    //TimeSync* m_timeSync;

public:

    InfoMenuItem(String name, String parentName, MenuRenderer* renderer)
            : Menu(name, parentName, renderer)
    {
        //m_timeSync = timeSync;
    }

    void action()
    {


    }

};





class CalMenuItemZero : public Menu
{

    DataSource* m_dataSource;
    GasManager* m_gasManager;

public:

    CalMenuItemZero(String name, DataSource* dataSource, GasManager* gasManager, String parentName, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_gasManager(gasManager),
              m_dataSource(dataSource)
    {

    }

    void action()
    {
        m_gasManager->calibrate(m_dataSource->getRawMiliVolts());
    }

};

class CalMenuItemCalGas : public Menu
{

    DataSource* m_dataSource;
    GasManager* m_gasManager;

public:

    CalMenuItemCalGas(String name, DataSource* dataSource, GasManager* gasManager, String parentName, MenuRenderer* renderer)
            : Menu(name, parentName, renderer),
              m_gasManager(gasManager),
              m_dataSource(dataSource)
    {

    }

    void action()
    {
        m_gasManager->calibrate2(m_dataSource->getRawMiliVolts());
    }

};







class ShowTimeMenuItem : public Menu
{

public:

    ShowTimeMenuItem(String name, String parentName, MenuRenderer* renderer)
            : Menu(name, parentName, renderer)
    {

    }

    void action()
    {

    }

};

class CompositeMenu : public Menu
{
    std::vector<Menu*> m_menus;
    int m_currentIndex;

public:

    CompositeMenu(String name, String parentName, std::vector<Menu*> menus) : Menu(name, parentName, nullptr),
                                                                              m_menus(menus),
                                                                              m_currentIndex(0)
    {

    }

    Menu* getCurrentMenu()
    {
        return m_menus[ m_currentIndex ];
    }

    void moveToNext()
    {
        m_currentIndex = (m_currentIndex + 1) % m_menus.size();
        if(m_currentIndex == 6 & m_menuName == "Main Menu"){
            m_currentIndex = 0;
        }
        Serial.println(m_parentMenuName);
        Serial.println("moveToNext" + String(m_currentIndex) + " " + String(m_menus.size()) );
        Serial.println("moveToNext: " + m_menuName );
        Serial.flush();
    }

    void setCurrentMenu(int menuIndex) {
        m_currentIndex = menuIndex;
        Serial.println("moveToNext" + String(m_currentIndex) + " ");


        // Optional: Handle the case where the menuName is not found
        Serial.println("Menu not found: " + menuIndex);
    }

    void action() override
    {
        m_menus[m_currentIndex]->action();
        Serial.println(m_currentIndex);
        Serial.println(m_menuName);
        m_currentIndex = (m_currentIndex + 1) % m_menus.size();
        if(m_menuName == "LIBRARY") {
            m_currentIndex = (m_currentIndex - 1) % m_menus.size();
        }else if(m_menuName == "Alarm"){
            m_currentIndex = (m_currentIndex - 1) % m_menus.size();
        }else if(m_menuName == "Range") {
            m_currentIndex = (m_currentIndex - 1) % m_menus.size();
        }else if(m_menuName == "Set Hour") {
            m_currentIndex = 0;
        }else if(m_menuName == "Set Minute") {
            m_currentIndex = 0;
        }else if(m_menuName == "Calvalue") {
            m_currentIndex = (m_currentIndex - 1) % m_menus.size();
        }

        if(m_currentIndex == 6 & m_menuName == "Main Menu"){
            m_currentIndex = 0;
        }
        Serial.println("moveToNext" + String(m_currentIndex) + " " + String(m_menus.size()) );
        Serial.println("moveToNext: " + m_menuName );
        Serial.flush();
    }

    void render() override
    {
        m_menus[ m_currentIndex ]->render();
    }

    void print()
    {
        Serial.println("Menu name: " + m_menuName + " Parent name: " + m_parentMenuName );
        for(Menu* m : m_menus)
            Serial.println("Menu name: " + m->getName() + " Parent name: " + m->getParentName() );
        Serial.flush();
    }

};