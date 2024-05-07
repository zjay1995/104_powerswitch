#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>
#include <DS3231M.h> // Include the DS3231M RTC library

class Hour
{
    std::array<int,24> m_hourArray{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
    int m_selectedHour = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;

    DS3231M_Class m_ds3231mRtc;


public:

    Hour()
    {
        m_selectedHour = 0;
    }

    ~Hour()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectHourByIndex(int index)
    {
        if(index >= 0 && index < m_hourArray.size())
        {
            m_selectedHour = index;
            EEPROM.writeInt(220, index);
            EEPROM.commit();
            Serial.print("Hour saved");
            Serial.println(index);
            DateTime now = m_ds3231mRtc.now();
            m_ds3231mRtc.adjust(DateTime(now.year(), now.month(), now.day(), index, now.minute(), now.second()));


        }

        return;
    }



    void selectNextHour()
    {
        m_selectedHour = (m_selectedHour + 1) % m_hourArray.size();
    }

    void selectPreviousHour()
    {
        if(m_selectedHour == 0)
            m_selectedHour = m_hourArray.size() - 1;
        else
            m_selectedHour = m_selectedHour - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedHour() {
        int hour = EEPROM.read(220);
        return m_hourArray[hour];
    }

};
