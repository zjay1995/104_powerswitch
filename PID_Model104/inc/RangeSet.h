#pragma once
#include <array>
#include "U8g2lib.h"
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>
#include <EEPROM.h>

class Range
{
    std::array<int, 12> m_rangeArray{{10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100}};
    int m_selectedRange = 0;

    unsigned long m_startMillis = 0;

    ConfigurationManager* m_configurationManager;
    //U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI* m_u8g2;
    U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* m_u8g2;
    SSD1306* m_display;


public:

    Range()
    {
        m_selectedRange = 0;
    }

    ~Range()=default;

    void init(ConfigurationManager* configurationManager, U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI* u8g2)
    {
        m_configurationManager = configurationManager;
        m_u8g2 = u8g2;
    }

    void selectRangeByValueNoEEPROMSave(int range)
    {
        for(int i=0; i < m_rangeArray.size(); i++)
            if(m_rangeArray[i] == range)
            {
                m_selectedRange = i;
                break;
            }

        return;
    }

    void selectRangeByIndex(int index)
    {
        if(index >= 0 && index < m_rangeArray.size())
        {
            m_selectedRange = index;
            EEPROM.writeInt(72, index);
            EEPROM.commit();
            Serial.print("Range saved ");
            Serial.println(index);
        }

        return;
    }

    void selectRangeByValue(int range)
    {
        for(int i=0; i < m_rangeArray.size(); i++)
            if(m_rangeArray[i] == range)
            {
                m_selectedRange = i;
                m_configurationManager->saveRangeToEEPROM(i);
                break;
            }

        return;
    }

    void selectNextRange()
    {
        m_selectedRange = (m_selectedRange + 1) % m_rangeArray.size();
    }

    void selectPreviousRange()
    {
        if(m_selectedRange == 0)
            m_selectedRange = m_rangeArray.size() - 1;
        else
            m_selectedRange = m_selectedRange - 1;
    }

    void resetIdleCounter()
    {
        m_startMillis = millis();
    }

    int getSelectedRange() {
        int range = EEPROM.read(72);
        return m_rangeArray[range];
    }

    void onParamChange(String param, String value){
        Serial.println("onRangeParamChange: " + param + "=" + value);

        if(param.equals(c_RANGE_PARAM_NAME))
        {
            m_selectedRange = value.toInt();
            Serial.println("Setting Range: " + value);
        }
    };

};
