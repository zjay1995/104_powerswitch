
/*
   Release Log
   Save the selected gas index to the EEPROM memory. 3/4/2023
   Range Selection Added but not implemented in the calculation. 2/28/2023
   Bluetooth Datalogging added and implemented.
*/

/* Parts List:
   Huzzah32
   ADS1115
*/

#include <Adafruit_ADS1015.h>
#include <SSD1306.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <vector>

#include "inc/TimeSync.h"
#include "inc/GasManager.h"
#include "inc/Button.h"
#include "inc/Menu.h"
#include "inc/MenuRenderer.h"
#include "inc/WebServer.h"
#include "inc/SleepTimer.h"
#include "inc/RangeSet.h"
#include "inc/AlarmSet.h"
#include "inc/HourSet.h"
#include "inc/MinuteSet.h"

#include "inc/CalvalueSet.h"
#include "inc/Globals.h"
#include "inc/DataLogger.h"
#include "inc/DataSource.h"
#include "inc/image.h"


using namespace std;

#define USE_SSD1306_DISPLAY

//Adafruit_SHT4x sht4 = Adafruit_SHT4x();

#define MAX_SCCM 5000

#define wifi_ssid "22CDPro"
#define wifi_password "00525508"

GasManager g_gasManager(-2.054456771, 1, 0, 0, 0, 0, 0, 0, 0, 0);

WebServer g_webServer;

CompositeMenu* g_mainMenu = nullptr;

Adafruit_ADS1115 ads1115;

#ifdef USE_SSD1306_DISPLAY
SSD1306 display(0x3c, 23, 22);
#endif


SleepTimer g_sleepTimer;

Range g_range;
Alarm g_alarm;
Hour g_hour;
Minute g_minute;
Calvalue g_calvalue;

DataLogger g_dataLogger;

TimeSync g_timeSync;

const char* ssid     = "ESP32-Access-Point";
const char* password = "Polaroid1";

volatile bool CALIBRATION_MODE = false;

unsigned long buttonPressStartTime = 0;
bool isButtonBeingPressed = false;

void setupWiFi() {
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  g_webServer.begin(80);
}

void IRAM_ATTR dummyTouchISR() {}

void setup() {
  Serial.begin(115200);
  Serial.println("PID M30 v230304");
  // DEEP-SLEEP init
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH); 
  //esp_sleep_enable_ext1_wakeup(0x8004, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, LOW);
  // ADC
  ads1115.begin();
  ads1115.setGain(GAIN_ONE);

  AnalogSourceInput* ads1115AnalogSourceInput = new ADS1115AnalogSourceInput(&ads1115);

  DataSource* dataSource = new DataSource(&g_gasManager, ads1115AnalogSourceInput);

  // Gas Manager
  g_gasManager.setConfigurationManager(&g_configurationManager);

  g_gasManager.addGas(Gas("Benzene", 1.0));
  g_gasManager.addGas(Gas("H2S", 1.0));
  g_gasManager.addGas(Gas("CO", 1.0));
  g_gasManager.addGas(Gas("O2", 5.73));
  g_gasManager.addGas(Gas("H2", 6.84));
  g_gasManager.addGas(Gas("ArCH4", 0.85));
  //
  /// Menus
  //

#ifdef USE_SSD1306_DISPLAY

  // Display
  display.init();
  display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_16);
  display.drawXbm(0,0,128,60,image_arr);
  display.display();
  delay(200);

  MenuRenderer* gasMenuRenderer = new SSD1306GasMenuRenderer(&display);
  MenuRenderer* runMenuRenderer = new SSD1306RunMenuRenderer(&display, dataSource, &g_gasManager, &g_alarm, &g_range, &g_calvalue);
  MenuRenderer* sleepTimerMenuRenderer = new SSD1306SleepTimerMenuRenderer(&display, &g_sleepTimer);
  MenuRenderer* rangeMenuRenderer = new SSD1306RangeMenuRenderer(&display, &g_range);
  MenuRenderer* alarmMenuRenderer = new SSD1306AlarmMenuRenderer(&display, &g_alarm);
  MenuRenderer* hourMenuRenderer = new SSD1306HourMenuRenderer(&display, &g_hour);
  MenuRenderer* minuteMenuRenderer = new SSD1306MinuteMenuRenderer(&display, &g_minute);
  MenuRenderer* calvalueMenuRenderer = new SSD1306CalvalueMenuRenderer(&display, &g_calvalue);

  MenuRenderer* flashLoggerMenuRenderer = new SSD1306FlashLoggerMenuRenderer(&display, &g_dataLogger);
  MenuRenderer* wifiDumpMenuRenderer = new SSD1306WiFiDumpMenuRenderer(&display, &g_dataLogger);
  MenuRenderer* wifiRealTimeDumpMenuRenderer = new SSD1306WiFiRealTimeDumpMenuRenderer(&display, &g_dataLogger);
  MenuRenderer* NTPSyncMenuRenderer = new SSD1306NTPSyncMenuRenderer(&display, &g_timeSync);
  MenuRenderer* showTimeMenuRenderer = new SSD1306ShowTimeMenuRenderer(&display);
  MenuRenderer* CalStartMenuRenderer = new SSD1306CalStartMenuRenderer(&display);

  MenuRenderer* ZEROMenuRenderer = new SSD1306ZEROMenuRenderer(&display, dataSource, &g_gasManager);
  MenuRenderer* CalGasMenuRenderer = new SSD1306CalGasMenuRenderer(&display, dataSource, &g_calvalue, &g_gasManager);
  MenuRenderer* CalResMenuRenderer = new SSD1306CalResMenuRenderer(&display, &g_gasManager);

#endif

  // Run Menus
  vector<Menu*> runMenus;

  runMenus.push_back(new RunMenuItem(" ", "RUN", 0, &g_gasManager, runMenuRenderer));
  runMenus.push_back(new RunMenuItem("L", "RUN", 1, &g_gasManager, runMenuRenderer));
  runMenus.push_back(new RunMenuItem("P", "RUN", 1, &g_gasManager, runMenuRenderer));

  CompositeMenu* runMenu = new CompositeMenu("RUN", "Main Menu", runMenus);

  // Gas Menus
  vector<Menu*> gasMenus;


  gasMenus.push_back(new GasMenuItem("Benzene", "LIBRARY",  0, &g_gasManager, gasMenuRenderer));
  gasMenus.push_back(new GasMenuItem("H2S", "LIBRARY", 1, &g_gasManager, gasMenuRenderer));
  gasMenus.push_back(new GasMenuItem("CO", "LIBRARY", 2, &g_gasManager, gasMenuRenderer));
  gasMenus.push_back(new GasMenuItem("O2", "LIBRARY",  3, &g_gasManager, gasMenuRenderer));
  // gasMenus.push_back(new GasMenuItem("DET 5", "LIBRARY", 4, &g_gasManager, gasMenuRenderer));
  //  gasMenus.push_back(new GasMenuItem("DET 6", "LIBRARY", 5, &g_gasManager, gasMenuRenderer));

  CompositeMenu* libraryMenu = new CompositeMenu("LIBRARY", "Main Menu" , gasMenus);

  // Timer Menus
  vector<Menu*> sleepTimerMenus;
  /*
    sleepTimerMenus.push_back(new SleepTimerMenuItem("5", "TIMER",  0, &g_sleepTimer, sleepTimerMenuRenderer));
    sleepTimerMenus.push_back(new SleepTimerMenuItem("60", "TIMER", 1, &g_sleepTimer, sleepTimerMenuRenderer));
    sleepTimerMenus.push_back(new SleepTimerMenuItem("120", "TIMER", 2, &g_sleepTimer, sleepTimerMenuRenderer));
    sleepTimerMenus.push_back(new SleepTimerMenuItem("360", "TIMER", 3, &g_sleepTimer, sleepTimerMenuRenderer));
    sleepTimerMenus.push_back(new SleepTimerMenuItem("CONTINUOUS", "TIMER", 4, &g_sleepTimer, sleepTimerMenuRenderer));

    CompositeMenu* timerMenu = new CompositeMenu("TIMER","Main Menu" , sleepTimerMenus);
  */


  // Range Menus
  vector<Menu*> rangeMenus;
  rangeMenus.push_back(new RangeMenuItem("10 ppm", "Range", 0, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("100 ppm", "Range", 1, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("200 ppm", "Range", 2, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("300 ppm", "Range", 3, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("400 ppm", "Range", 4, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("500 ppm", "Range", 5, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("600 ppm", "Range", 6, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("700 ppm", "Range", 7, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("800 ppm", "Range", 8, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("900 ppm", "Range", 9, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("1000 ppm", "Range", 10, &g_range, rangeMenuRenderer));
  rangeMenus.push_back(new RangeMenuItem("1100 ppm", "Range", 11, &g_range, rangeMenuRenderer));


  CompositeMenu* rangeMenu = new CompositeMenu("Range", "Main Menu" , rangeMenus);


  // Alarm Menus
  vector<Menu*> alarmMenus;
  for (int i = 0; i <= 61; i++) {
    int ppm = 475 + i * 25;
    String label = "Alarm";
    if (ppm == 475) {
      alarmMenus.push_back(new AlarmMenuItem("Off", label, i, &g_alarm, alarmMenuRenderer));
    } else {
      alarmMenus.push_back(new AlarmMenuItem(String(ppm) + " ppm", label, i, &g_alarm, alarmMenuRenderer));
    }
  }

  CompositeMenu* alarmMenu = new CompositeMenu("Alarm", "Main Menu" , alarmMenus);

  // Hour Menus
  vector<Menu*> minuteMenus;
  for (int i = 0; i <= 59; i++) {
    String label = "Set Minute";
    minuteMenus.push_back(new MinuteMenuItem(String(i) , label, i, &g_minute, minuteMenuRenderer));
  }

  CompositeMenu* minuteMenu = new CompositeMenu("Set Minute", "Main Menu" , minuteMenus);

  // Minute Menus

  vector<Menu*> hourMenus;
  for (int i = 0; i <= 23; i++) {
    String label = "Set Hour";
    hourMenus.push_back(new HourMenuItem(String(i) , label, i, &g_hour, hourMenuRenderer));
  }

  CompositeMenu* hourMenu = new CompositeMenu("Set Hour", "Main Menu" , hourMenus);


  // calvalue Menus

  vector<Menu*> calvalueMenus;
  calvalueMenus.push_back(new CalvalueMenuItem(String(0.1) + " ppm", "Cal Gas", 0, &g_calvalue, calvalueMenuRenderer));

  for (int i = 1; i <= 100; i++) {
    int ppm = 0 + i * 10;
    String label = "Cal Gas";
    calvalueMenus.push_back(new CalvalueMenuItem(String(ppm) + " ppm", label, i, &g_calvalue, calvalueMenuRenderer));
  }

  CompositeMenu* calvalueMenu = new CompositeMenu("Calvalue", "Main Menu" , calvalueMenus);

  // DataLogger Menus
  vector<Menu*> dataLoggerMenus;

  dataLoggerMenus.push_back(new DataLoggerFlashStoreMenuItem("FLASH LOGGER", "DATALOGGER",  &g_dataLogger, flashLoggerMenuRenderer));
  dataLoggerMenus.push_back(new WiFiDumpMenuItem("WIFI DUMP", "DATALOGGER",           &g_dataLogger, wifiDumpMenuRenderer));
  dataLoggerMenus.push_back(new WiFiRealTimeDumpMenuItem("WIFI REAL-TIME DUMP", "DATALOGGER", &g_dataLogger, wifiRealTimeDumpMenuRenderer));


  CompositeMenu* dataLoggerMenu = new CompositeMenu("DATALOGGER", "Main Menu" , dataLoggerMenus);

  // DateTime menu
  vector<Menu*> dateTimeMenus;

  dateTimeMenus.push_back(new NTPSyncMenuItem("Time Sync", "DATETIME", &g_timeSync, NTPSyncMenuRenderer));
  dateTimeMenus.push_back(new ShowTimeMenuItem("Current DateTime", "DATETIME", showTimeMenuRenderer));

  CompositeMenu* dateTimeMenu = new CompositeMenu("DATETIME", "Main Menu" , dateTimeMenus);


  // Cal zero menu
  vector<Menu*> calMenus;
  calMenus.push_back(new CalMenuItemZero("Zero", dataSource, &g_gasManager, "ZeroGas", ZEROMenuRenderer));
  CompositeMenu* calMenu = new CompositeMenu("CALIBRATION Zero", "Main Menu", calMenus);

  // Cal gas menu
  vector<Menu*> calgasMenus;
  calgasMenus.push_back(new CalMenuItemCalGas("Cal Gas", dataSource, &g_gasManager, "CalGas", CalGasMenuRenderer));
  CompositeMenu* calgasMenu = new CompositeMenu("CALIBRATION Calgas", "Main Menu", calgasMenus);


  ////////////////////////////////////
  vector<Menu*> horizontalMenus;

  horizontalMenus.push_back(runMenu);
  horizontalMenus.push_back(libraryMenu);
  //horizontalMenus.push_back(timerMenu);
  horizontalMenus.push_back(rangeMenu);
  //horizontalMenus.push_back(dataLoggerMenu);
  //horizontalMenus.push_back(dateTimeMenu);
  horizontalMenus.push_back(alarmMenu);
  horizontalMenus.push_back(hourMenu);
  horizontalMenus.push_back(minuteMenu);

  horizontalMenus.push_back(calMenu);
  horizontalMenus.push_back(calvalueMenu);
  horizontalMenus.push_back(calgasMenu);

  Serial.println("horizontal menu " + String(horizontalMenus.size()));
  CompositeMenu* verticalMenu = new CompositeMenu("Main Menu", "", horizontalMenus);

  g_mainMenu = verticalMenu;

  g_mainMenu->print();

  setupButtons();

  g_webServer.init(&g_gasManager);
  //g_sleepTimer.init(&g_configurationManager);
  //g_range.init(&g_gasManager);

  g_dataLogger.init(dataSource, &g_gasManager);

  g_webServer.addParamChangeListener((ParamChangeListener*)&g_configurationManager);
  g_webServer.addParamChangeListener((ParamChangeListener*)&g_gasManager);

  g_configurationManager.addParamChangeListener((ParamChangeListener*)&g_timeSync);
  g_configurationManager.addParamChangeListener((ParamChangeListener*)&g_gasManager);
  //g_configurationManager.addParamChangeListener((ParamChangeListener*)&g_range);

  g_configurationManager.addParamChangeListener((ParamChangeListener*)g_dataLogger.getMqttFlashPublisher());
  g_configurationManager.addParamChangeListener((ParamChangeListener*)g_dataLogger.getMqttRealTimePublisher());

  g_configurationManager.init();
  g_configurationManager.loadFromEEPROM();

  g_timeSync.initTimeFromRTC();
  int range = EEPROM.read(72);
  g_range.selectRangeByIndex(range);
  int alarm = EEPROM.read(76);
  g_alarm.selectAlarmByIndex(alarm);
}

void setupButtons()
{

  Keyboard* keyboard = new Keyboard();

  keyboard->addOnDownPressedFctor([] {

    g_sleepTimer.resetIdleCounter();


    Serial.println("PRESS DOWWWNNNN");
    g_mainMenu->print();
    g_mainMenu->moveToNext();

  });
  keyboard->addOnSPressedFctor([] {

    g_sleepTimer.resetIdleCounter();


    Serial.println("PRESS S");
    g_mainMenu->action();

    g_timeSync.initTimeFromRTC();

  });
  keyboard->addOnRightPressedFctor([] {

    g_sleepTimer.resetIdleCounter();


    Serial.println("PRESS RIGHT");
    ((CompositeMenu*)g_mainMenu->getCurrentMenu())->moveToNext();
  });

  keyboard->addOnCalibrationComboPressedFctor([] {

    g_sleepTimer.resetIdleCounter();
    g_mainMenu->setCurrentMenu(6);
    Serial.println("PRESS CALIBRATION");
    return;
  });

}



void loop()
{

  ButtonPressDetector::handleTick();

  g_sleepTimer.handleTick();
  g_dataLogger.handleTick();

  g_mainMenu->render();

  delay(10);
}
