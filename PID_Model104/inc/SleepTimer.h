#pragma once
#include <array>
#define SSD1306_DISPLAYOFF          0xAE
#include "ConfigurationManager.h"
#include <SSD1306.h>
#include <Wire.h>

class SleepTimer
{
	//int m_intervalArray[] = {1,2,5,-1};
	std::array <int,5> m_intervalArray{{5,60,120,300,-1}};
	int m_selectedIndex = 0;
	
	unsigned long m_startMillis = 0;
	
	ConfigurationManager* m_configurationManager;
    //U8G2_SSD1327_MIDAS_128X128_F_4W_HW_SPI* m_u8g2;
    SSD1306* m_display;


public:

	SleepTimer()
	{
		m_selectedIndex = 4;
	}
	
	~SleepTimer()=default;
	
	void init(ConfigurationManager* configurationManager)
	{
		m_configurationManager = configurationManager;
	}

    void selectIntervalByValueNoEEPROMSave(int interval)
    {
        for(int i=0; i < m_intervalArray.size(); i++)
            if(m_intervalArray[i] == interval)
            {
                m_selectedIndex = i;
                break;
            }

        return;
    }

	void selectIntervalByIndex(int index)
	{	
		if(index >= 0 && index < m_intervalArray.size())
		{
			m_selectedIndex = index;
			m_configurationManager->saveTimerIntervalToEEPROM(m_intervalArray[m_selectedIndex]);
		}
		
		return;
	}

	void selectIntervalByValue(int interval)
	{
		for(int i=0; i < m_intervalArray.size(); i++)
			if(m_intervalArray[i] == interval)
			{
				m_selectedIndex = i;
				m_configurationManager->saveTimerIntervalToEEPROM(5);
				break;
			}
		
		return;
	}
	
	void selectNextInterval()
	{
		m_selectedIndex = (m_selectedIndex + 1) % m_intervalArray.size();
	}
	
	void selectPreviousInterval()
	{
		if(m_selectedIndex == 0)
			m_selectedIndex = m_intervalArray.size() - 1;
		else
			m_selectedIndex = m_selectedIndex - 1;		
	}
	
	void resetIdleCounter()
	{
		m_startMillis = millis();	
	}
	
	int getSelectedInterval() { m_intervalArray[m_selectedIndex]; } 
	
	void handleTick()
	{

		unsigned long passedMillis = millis() - m_startMillis;
		

//		if(passedMillis / 1000 >= 3000)
//		{
//
//            //add auto-off here
//            //digitalWrite(19, LOW);
//            //go in to deep sleep
//            gpio_hold_en(GPIO_NUM_5);
//            gpio_deep_sleep_hold_en();
//
//
//  for (int i = 0; i <= 3; i++) {
//    digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
//    delay(200);                       // wait for a second
//    digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
//    delay(200);                       // wait for a second
//  }
//
//delay(1000);
//
//            Serial.println("SleepTimer deep_sleep!");
//            Serial.flush();
//
//			//m_u8g2->sleepOff();
//            Wire.beginTransmission(0x3C);
//            Wire.write(0x80);
//            Wire.write(SSD1306_DISPLAYOFF);
//            Wire.endTransmission(); //           m_display -> display();
//
//            delay(2000);
//            esp_deep_sleep_start();
//		}			
	}
};
