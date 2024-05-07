#pragma once

class Menu;
class SSD1306Wire;
class U8G2_SSD1327_MIDAS_128X128_F_4W_SW_SPI;
class Adafruit_ADS1115;
class GasManager;
class GasMenuItem;
class RunMenuItem;
class SleepTimer;
class DataLogger;
class DataSource;
class TimeSync;
class Range;
class Alarm;
class Calvalue;
class Hour;
class Minute;


class MenuRenderer
{
public:

	virtual void render(Menu* menu) = 0;
};

class SSD1306MenuRenderer : public MenuRenderer
{
protected:	
	SSD1306Wire* 			m_display;

public:

	SSD1306MenuRenderer(SSD1306Wire* display) : m_display(display)
	{}

	void render(Menu* menu);
	
};

class SSD1306GasMenuRenderer : public SSD1306MenuRenderer
{


public:

	SSD1306GasMenuRenderer(SSD1306Wire* display);
	
	void render(Menu* menu);
	
};

class SSD1306RunMenuRenderer : public SSD1306MenuRenderer
{
	DataSource* 		m_dataSource;
	GasManager*			m_gasManager;
    Range*              m_range;
    Alarm*              m_alarm;
    Calvalue*           m_calvalue;
	
public:

	SSD1306RunMenuRenderer(SSD1306Wire* display, DataSource* dataSource, GasManager* gasManager,Alarm* alarm, Range* range, Calvalue* calvalue);

	void render(Menu* menu);
};

class SSD1306SleepTimerMenuRenderer : public SSD1306MenuRenderer
{
	SleepTimer* m_sleepTimer;
	
public:	
	
	SSD1306SleepTimerMenuRenderer(SSD1306Wire* display, SleepTimer* sleepTimer);
	
	void render(Menu* menu);
};

class SSD1306RangeMenuRenderer : public SSD1306MenuRenderer
{
    Range* m_range;

public:

    SSD1306RangeMenuRenderer(SSD1306Wire* display, Range* range);

    void render(Menu* menu);
};

class SSD1306AlarmMenuRenderer : public SSD1306MenuRenderer
{
    Alarm* m_alarm;

public:

    SSD1306AlarmMenuRenderer(SSD1306Wire* display, Alarm* alarm);

    void render(Menu* menu);
};


class SSD1306HourMenuRenderer : public SSD1306MenuRenderer
{
    Hour* m_hour;

public:

    SSD1306HourMenuRenderer(SSD1306Wire* display, Hour* hour);

    void render(Menu* menu);
};

class SSD1306MinuteMenuRenderer : public SSD1306MenuRenderer
{
    Minute* m_minute;

public:

    SSD1306MinuteMenuRenderer(SSD1306Wire* display, Minute* minute);

    void render(Menu* menu);
};

class SSD1306CalvalueMenuRenderer : public SSD1306MenuRenderer
{
    Calvalue* m_calvalue;

public:

    SSD1306CalvalueMenuRenderer(SSD1306Wire* display, Calvalue* calvalue);

    void render(Menu* menu);
};


class SSD1306FlashLoggerMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;
	
public:	
	
	SSD1306FlashLoggerMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);
	
	void render(Menu* menu);
};

class SSD1306WiFiDumpMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SSD1306WiFiDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SSD1306WiFiRealTimeDumpMenuRenderer : public SSD1306MenuRenderer
{
	DataLogger* m_dataLogger;

public:

	SSD1306WiFiRealTimeDumpMenuRenderer(SSD1306Wire* display, DataLogger* dataLogger);

	void render(Menu* menu);
};

class SSD1306NTPSyncMenuRenderer : public SSD1306MenuRenderer
{
	TimeSync* m_timeSync;

public:

	SSD1306NTPSyncMenuRenderer(SSD1306Wire* display, TimeSync* timeSync);

	void render(Menu* menu);
};

class SSD1306ShowTimeMenuRenderer : public SSD1306MenuRenderer
{

public:

	SSD1306ShowTimeMenuRenderer(SSD1306Wire* display);

	void render(Menu* menu);
};



class SSD1306CalStartMenuRenderer : public SSD1306MenuRenderer
{

public:

    SSD1306CalStartMenuRenderer(SSD1306Wire* display);

    void render(Menu* menu);
};



class SSD1306ZEROMenuRenderer : public SSD1306MenuRenderer
{
    DataSource* 		m_dataSource;
    GasManager*				m_gasManager;
public:

    SSD1306ZEROMenuRenderer(SSD1306Wire* display, DataSource* dataSource, GasManager* gasManager);
    void render(Menu* menu);
};


class SSD1306CalGasMenuRenderer : public SSD1306MenuRenderer
{
    DataSource* 		m_dataSource;
    Calvalue*              m_calvalue;
    GasManager*				m_gasManager;
public:

    SSD1306CalGasMenuRenderer(SSD1306Wire* display,  DataSource* dataSource,Calvalue* calvalue, GasManager* gasManager);
    void render(Menu* menu);
};



class SSD1306CalResMenuRenderer : public SSD1306MenuRenderer
{
    GasManager*				m_gasManager;

public:

    SSD1306CalResMenuRenderer(SSD1306Wire* display, GasManager* gasManager);
    void render(Menu* menu);
};