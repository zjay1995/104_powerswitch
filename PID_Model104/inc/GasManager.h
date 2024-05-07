#pragma once

#include <vector>

#include <Arduino.h>

#include "ConfigurationManager.h"
#include "Globals.h"
#include <EEPROM.h>
#include "CalvalueSet.h"
#include "AlarmSet.h"


class Gas
{
    String m_name;
    double m_intercept;
    double m_slope;
    double m_thermalConductivity;
public:

    Gas(String name, double thermalConductivity = 1.0) : m_name(name),
                                                         m_thermalConductivity(thermalConductivity)
    {

    }

    String getName() { return m_name; }

    double getThermalConductivity() const 					{ return m_thermalConductivity; }
    void setThermalConductivity(double thermalConductivity) { m_thermalConductivity = thermalConductivity; }

};



class GasManager : public ParamChangeListener
{
    std::vector<Gas> m_gases;

    int m_selectedGas =EEPROM.readInt(4);

    double m_intercept;
    double m_slope;
    double m_intercept2;
    double m_slope2;
    double m_intercept3;
    double m_slope3;
    double m_intercept4;
    double m_slope4;
    double m_zero;
    double m_calgas;


    ConfigurationManager* m_configurationManager;

public:
    GasManager(double intercept, double slope,
               double zero, double calgas, double intercept2,double slope2,
               double intercept3,double slope3,double intercept4,double slope4) : 
                                                                                      m_intercept(intercept),
                                                                                      m_slope(slope),
                                                                                      m_intercept2(intercept2),
                                                                                      m_slope2(slope2),
                                                                                      m_intercept3(intercept3),
                                                                                      m_slope3(slope3),
                                                                                      m_intercept4(intercept4),
                                                                                      m_slope4(slope4),
                                                                                      m_zero(zero),
                                                                                      m_calgas(calgas)
    {}

    ~GasManager() {}

    double getIntercept() const { return m_intercept; }
    double getSlope() const { return m_slope; }
    double getIntercept2() const { return m_intercept2; }
    double getSlope2() const { return m_slope2; }
    double getIntercept3() const { return m_intercept3; }
    double getSlope3() const { return m_slope3; }
    double getIntercept4() const { return m_intercept4; }
    double getSlope4() const { return m_slope4; }
    double getZero() const {return m_zero; }
    double getCalgas() const {return m_calgas; }

    void setIntercept(double i) { m_intercept = i; }
    void setSlope(double p) {m_slope = p; }
    void setIntercept2(double ii) { m_intercept2 = ii; }
    void setSlope2(double pp) {m_slope2 = pp; }
    void setIntercept3(double iii) { m_intercept3 = iii; }
    void setSlope3(double ppp) {m_slope3 = ppp; }
    void setIntercept4(double iiii) { m_intercept4 = iiii; }
    void setSlope4(double pppp) {m_slope4 = pppp; }
    void setZero(double z) {m_zero = z; }
    void setCalgas(double c) {m_calgas = c;}


    double calculateSLM(double voltage) {
        double val = 0;
        if(m_selectedGas==0){
            val = ((voltage - m_intercept ) / m_slope) * 1000 ; //votlage in volts
        }else if(m_selectedGas==1){
            val = ((voltage - m_intercept2 ) / m_slope2) * 1000 ; //votlage in volts
        }else if(m_selectedGas==2){
            val = ((voltage - m_intercept3 ) / m_slope3) * 1000 ; //votlage in volts
        }else if(m_selectedGas==3){
            val = ((voltage - m_intercept4 ) / m_slope4) * 1000 ; //votlage in volts
        }
        return val < 0 ? 0 : val;
    }



    void setConfigurationManager(ConfigurationManager* c) { m_configurationManager = c; }

    void addGas(Gas g) { m_gases.push_back(g); }

    void printinfo(){
        Serial.println("S Button Pushed!!!!!!  ");
    }

    void selectGasByIndex(int index)
    {
        Serial.println("selectGasByIndex");
        if(index >= 0 && index < m_gases.size())
        {
            m_selectedGas = index;
            //m_configurationManager->saveGasSelectedToEEPROM(index);
            Serial.println("gas index saved"+index);
            EEPROM.writeInt(4, index);
            EEPROM.commit();
        }
    }

    void calibrate(double zero){
        m_zero = zero/1000;
        delay(20);
        if(m_selectedGas==0) {
            m_intercept = m_zero;
            Serial.println("calibrating for gas 1");
            EEPROM.put(8+m_selectedGas*16, m_intercept);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_intercept);
        }else if(m_selectedGas ==1){
            m_intercept2 = m_zero;
            Serial.println("calibrating for gas 2");
            EEPROM.put(8+m_selectedGas*16, m_intercept2);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_intercept2);
        }else if(m_selectedGas ==2){
            m_intercept3 = m_zero;
            Serial.println("calibrating for gas 3");
            EEPROM.put(8+m_selectedGas*16, m_intercept3);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_intercept3);
        }else if(m_selectedGas ==3){
            m_intercept4 = m_zero;
            Serial.println("calibrating for gas 4");
            EEPROM.put(8+m_selectedGas*16, m_intercept4);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_intercept4);
        }

        EEPROM.commit();


    }

    void calibrate2(double cal){
        m_calgas = cal / 1000;
        int calvalue=Calvalue().getSelectedCalvalue();
        double calgasv = calvalue/1000.0;
        Serial.println(calvalue);
        Serial.println(calgasv);
        m_slope = (m_calgas - m_zero) / calgasv;
        if(m_selectedGas==0) {
            m_slope = (m_calgas - m_zero) / calgasv;
            Serial.println("calibrating for gas 1");
            Serial.println(m_slope);
            EEPROM.put(16+m_selectedGas*16, m_slope);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_slope);
        }else if(m_selectedGas ==1){
            m_slope2 = (m_calgas - m_zero) / calgasv;
            Serial.println("calibrating for gas 2");
            EEPROM.put(16+m_selectedGas*16, m_slope2);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_slope2);
        }else if(m_selectedGas ==2){
            m_slope3 = (m_calgas - m_zero) / calgasv;
            Serial.println("calibrating for gas 3");
            EEPROM.put(16+m_selectedGas*16, m_slope3);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_slope3);
        }else if(m_selectedGas ==3){
            m_slope4 = (m_calgas - m_zero) / calgasv;
            Serial.println("calibrating for gas 4");
            EEPROM.put(16+m_selectedGas*16, m_slope4);
            EEPROM.commit();
            Serial.print("Factors Saved to EEPROM: "); Serial.println(m_slope4);
        }

    }

    void calibrate3(){

    }

    void selectPreviousGas()
    {
        Serial.println("selectPreviousGas");

        if(m_selectedGas == 0)
            m_selectedGas = m_gases.size() - 1;
        else
            m_selectedGas = m_selectedGas - 1;
    }

    Gas& getSelectedGas() { return m_gases[m_selectedGas]; }


    std::vector<Gas>& getAllGases() { return m_gases; }

    void onParamChange(String param, String value)
    {

        Serial.println("onGasParamChange: " + param + "=" + value);

        if(param.equals(c_INTERCEPT_PARAM_NAME))
        {
            m_intercept = value.toDouble();
            Serial.println("Setting INTERCEPT: " + value);
        }
        else if(param.equals(c_SLOPE_PARAM_NAME))
        {
            m_slope = value.toDouble();
            Serial.println("Setting SLOPE: " + value);
        }
        else if(param.equals(c_INTERCEPT2_PARAM_NAME))
        {
            m_intercept2 = value.toDouble();
            Serial.println("Setting INTERCEPT2: " + value);
        }
        else if(param.equals(c_SLOPE2_PARAM_NAME))
        {
            m_slope2 = value.toDouble();
            Serial.println("Setting SLOPE2: " + value);
        }
        else if(param.equals(c_INTERCEPT3_PARAM_NAME))
        {
            m_intercept3 = value.toDouble();
            Serial.println("Setting INTERCEPT3: " + value);
        }
        else if(param.equals(c_SLOPE3_PARAM_NAME))
        {
            m_slope3 = value.toDouble();
            Serial.println("Setting SLOPE3: " + value);
        }
        else if(param.equals(c_INTERCEPT4_PARAM_NAME))
        {
            m_intercept4 = value.toDouble();
            Serial.println("Setting INTERCEPT4: " + value);
        }
        else if(param.equals(c_SLOPE4_PARAM_NAME))
        {
            m_slope4 = value.toDouble();
            Serial.println("Setting SLOPE4: " + value);
        }
        else if(param.equals(c_GASINDEX_PARAM_NAME))
        {
            m_selectedGas = value.toDouble();
            Serial.println("Setting Gas_Index: " + value);
        }
        else
        {
            for (auto& gas : m_gases)
            {
                if (gas.getName().equals(param))
                {
                    double tc = value.toDouble();
                    gas.setThermalConductivity(tc);
                    Serial.println("Setting TC for gas: " + gas.getName() + " " + String(tc));
                    break;
                }
            }
        }
    }


};

