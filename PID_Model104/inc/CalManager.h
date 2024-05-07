#pragma once

#include <vector>

#include <Arduino.h>

#include "ConfigurationManager.h"
#include "Globals.h"

class Cal
{
    String m_name;
    double m_slope;
    double m_intercept;

public:

    Gas(String name) : m_name(name){

    }

    String getName() { return m_name; }

};

class CalManager : public ParamChangeListener
{
    std::vector<Cal> m_cal;

    int m_selectcalibration = 0;

    double m_slope;
    double m_intercept;

    ConfigurationManager* m_configurationManager;


public:
    GasManager(double slope, double intercept) : m_slope(slope),
    m_intercept(intercept),
    {}

    ~GasManager() {}

    double getSlope() const { return m_slope; }
    double getIntercept() const { return m_intercept; }
    void setSlope(double s) { m_slope = s; }
    void setIntercept(double i) { m_intercept = i; }
