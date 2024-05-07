#pragma once

#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ConfigurationManager.h"

#define wifi_ssid "1134ea"
#define wifi_password "DNm11i223344"

void MQTTFlashPublisher_Task(void* param);

class MQTTFlashPublisher : public ParamChangeListener
{
	WiFiClient m_wifi;
	MQTTClient m_client;
	
	TaskHandle_t m_task;

	File m_file;

	bool m_running = false;

public:

	MQTTFlashPublisher() {}

	void runTask()
	{
		setup();

		while (m_running)
		{
			handleTick();
		}

		m_running = false;
	}
	
	void start()
	{
		if (m_running)
			Serial.println("MQTTFlashPublisher :: start() -- Task is already running!!");

		if (!xTaskCreatePinnedToCore(
			MQTTFlashPublisher_Task,     // Function that should be called
			"MQTTFlashPublisher_Task",   // Name of the task (for debugging)
			15000,					// Stack size (bytes)
			this,					// Parameter to pass
			1,						// Task priority
			&m_task,				// Task handle
			0						// Core you want to run the task on (0 or 1)
			)
			)
		{
			Serial.println("Failed to create MQTTFlashPublisher_Task!!!!");
			return;
		}
			
		m_running = true;
	}

	void stop()
	{
		m_running = false;
	}

	bool isRunning() const
	{
		return m_running;
	}

	void connect() 
	{
		Serial.print("MQTTFlashPublisher :: checking wifi...");
		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(".");
			delay(1000);
		}

		Serial.print("\n MQTTFlashPublisher :: connecting...");
		while (!m_client.connect("pid-flow-meter-01")) {
			Serial.print(".");
			delay(1000);
		}

		Serial.println("\n MQTTFlashPublisher :: connected!");
	  
		m_client.publish("/hello", "", true);
		m_client.subscribe("/hello");
	}

	static void messageReceived(String &topic, String &payload) 
	{
		Serial.println("incoming: " + topic + " - " + payload);

		// Note: Do not use the client in the callback to publish, subscribe or
		// unsubscribe as it may cause deadlocks when other things arrive while
		// sending and receiving acknowledgments. Instead, change a global variable,
		// or push to a queue and handle it in the loop after calling `client.loop()`.
	}
	
	void setup() 
	{
		vTaskDelay(200);

		WiFi.begin(wifi_ssid, wifi_password);
		WiFi.waitForConnectResult();
		Serial.println(WiFi.dnsIP());
		WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), IPAddress(8, 8, 8, 8));
		delay(10);
		Serial.println(WiFi.dnsIP());
		// Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
		// by Arduino. You need to set the IP address directly.
		//
		// MQTT brokers usually use port 8883 for secure connections.
		m_client.begin("test.mosquitto.org", 1883, m_wifi);
		m_client.onMessage(messageReceived);

		connect();
	}
	
	void handleTick()
	{
		m_client.loop();
		delay(10);  // <- fixes some issues with WiFi stability

		if (!m_client.connected())
		{
			if (!m_running)
				return;

			Serial.print("lastError: ");
			Serial.println(m_client.lastError());
			connect();
		}

		sendFinishedLoggingSessions();

		if (!m_running)
			return;
	}	

	// FileName example: 
	bool extractParamsFromFile(String fileName, time_t* ts, String* gasName, int* secondsBetweenDataPoints, int* fileNdx)
	{
		int start = fileName[0] == '/' ? 1 : 0;

		int endOfTs = fileName.indexOf('_', 0);

		if (endOfTs == -1)
		{
			Serial.println("extractParamsFromFile :: failed to find endOfTsNdx: " + fileName);
			return false;
		}

		*ts = atol(fileName.substring(start, endOfTs).c_str());

		///////
		int endOfGasName = fileName.indexOf('_', endOfTs + 1);

		if (endOfGasName == -1)
		{
			Serial.println("extractParamsFromFile :: failed to find endOfSecBetweenPointsNdx: " + fileName);
			return false;
		}

		*gasName = fileName.substring(endOfTs + 1, endOfGasName);
		///

		int endOfSecBetweenPoints = fileName.indexOf('_', endOfGasName + 1);

		if (endOfSecBetweenPoints == -1)
		{
			Serial.println("extractParamsFromFile :: failed to find endOfSecBetweenPointsNdx: " + fileName);
			return false;
		}

		*secondsBetweenDataPoints = fileName.substring(endOfGasName + 1, endOfSecBetweenPoints).toInt();

		int endOfFileNdx = fileName.indexOf('.');

		if (endOfFileNdx == -1)
		{
			Serial.println("extractParamsFromFile :: failed to find endOfSecBetweenPointsNdx: " + fileName);
			return false;
		}

		*fileNdx = fileName.substring(endOfSecBetweenPoints + 1, endOfFileNdx).toInt();

		return true;
	}

	void sendFinishedLoggingSessions()
	{
		String fileName;
		while (true)
		{
			fileName = findFinishedLoggingSession();

			if (fileName == "")
			{
				Serial.println("MQTTFlashPublisher :: no finished sessions to send!");
				delay(1000);
				break;
			}

			time_t unixTime;
			int secBetweenDataPoints;
			int fileIndex;
			String gasName;

			if (!extractParamsFromFile(fileName, &unixTime, &gasName, &secBetweenDataPoints, &fileIndex))
			{
				Serial.println("MQTTFlashPublisher sendFinishedLoggingSessions :: failed to extract params for: " + fileName);
				delay(1000);
				break;
			}

			Serial.println("MQTTFlashPublisher extractParamsFromFile :: fileName: " + fileName +
				" unixTime: " + String(unixTime) +
				" gas: " + gasName +
				" secBetweenDataPoints: " + secBetweenDataPoints +
				" fileIndex: " + fileIndex
			);

			m_file = SPIFFS.open(fileName, "rb");

			if (!m_file)
			{
				Serial.println("MQTTFlashPublisher :: failed to open file for reading: " + fileName);
				break;
			}

			if (!m_file.available())
			{
				Serial.println("MQTTFlashPublisher :: no data to read in file: " + fileName);
				SPIFFS.remove(m_file.name());
				break;
			}

			int recordNdx = 0;
			while (m_file.available())
			{
				double val;
				if (m_file.read((uint8_t*)&val, sizeof(double)) != sizeof(double))
				{
					Serial.println("MQTTFlashPublisher :: failed to read double from file!!");
					m_file.close();
					SPIFFS.remove(m_file.name());
					break;
				}

				Serial.println("MQTTFlashPublisher :: read val: " + String(val));

				char output[128];
				StaticJsonDocument<128> doc;

				unsigned long ts = unixTime + recordNdx * secBetweenDataPoints;

				doc["value"] = val;
				doc["ts"] = unixTime + recordNdx * secBetweenDataPoints;


				serializeJson(doc, output);

				int len1 = measureJson(doc);

				m_client.publish("/hello", output);

				m_client.loop();

				recordNdx++;

				Serial.println("MQTTFlashPublisher :: sent data!!");

				delay(10);
			}

			SPIFFS.remove(m_file.name());
		}
	}

	String findFinishedLoggingSession()
	{
		File root = SPIFFS.open("/");
		if (!root) {
			Serial.println("findFinishedLoggingSession :: Failed to open directory");
			return "";
		}
		if (!root.isDirectory()) {
			Serial.println("findFinishedLoggingSession :: Not a directory");
			return "";
		}

		File file = root.openNextFile();
		while (file) {
			if (file.isDirectory()) {
				Serial.print("findFinishedLoggingSession ::  DIR : ");
				Serial.print(file.name());
			}
			else {

				String fname = String(file.name());

				if (fname.endsWith(".done"))
				{
					Serial.println("file to send: " + fname + " size: " + String(file.size()));
					return fname;
				}
			}
			file = root.openNextFile();
		}

		return "";
	}

	String findRunningLoggingSession()
	{
		Serial.printf("findRunningLoggingSession :: Listing directory: %s\n", "/");

		File root = SPIFFS.open("/");
		if (!root) {
			Serial.println("findRunningLoggingSession :: Failed to open directory");
			return "";
		}
		if (!root.isDirectory()) {
			Serial.println("findRunningLoggingSession :: Not a directory");
			return "";
		}

		Serial.println("findRunningLoggingSession :: start");
		File file = root.openNextFile();
		while (file) {
			if (file.isDirectory()) {
				Serial.print("findRunningLoggingSession ::  DIR : ");
				Serial.print(file.name());
			}
			else {
				Serial.print("findRunningLoggingSession ::  FILE: ");
				Serial.print(file.name());
				Serial.print("findRunningLoggingSession ::  SIZE: ");
				Serial.print(file.size());

				String fname = String(file.name());
				if (fname.endsWith(".running"))
				{
					return fname;
				}
			}
			file = root.openNextFile();
		}

		Serial.println("findFinishedLoggingSession :: listdir Exit");

		return "";
	}


	void onParamChange(String param, String value)
	{


	}

};




void MQTTFlashPublisher_Task(void* param)
{
	MQTTFlashPublisher* mqttFlashPublisher = (MQTTFlashPublisher*)param;

	mqttFlashPublisher->runTask();

	vTaskDelete(0);
}
