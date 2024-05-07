#pragma once

#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DataSource.h"
#include "ConfigurationManager.h"

#define wifi_ssid "1134ea"
#define wifi_password "DNm11i223344"

void MQTTRealTimePublisher_Task(void* param);

class MQTTRealTimePublisher : public ParamChangeListener
{
	WiFiClient m_wifi;
	MQTTClient m_client;
	
	TaskHandle_t m_task;

	DataSource* m_dataSource;

	bool m_running = false;

public:

	MQTTRealTimePublisher(DataSource* dataSource) : m_dataSource(dataSource) {}

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
			Serial.println("MQTTRealTimePublisher :: start() -- Task is already running!!");

		if (!xTaskCreatePinnedToCore(
			MQTTRealTimePublisher_Task,     // Function that should be called
			"MQTTRealTimePublisher_Task",   // Name of the task (for debugging)
			15000,					// Stack size (bytes)
			this,					// Parameter to pass
			1,						// Task priority
			&m_task,				// Task handle
			0						// Core you want to run the task on (0 or 1)
			)
			)
		{
			Serial.println("Failed to create MQTTRealTimePublisher_Task!!!!");
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
		Serial.print("checking wifi...");
		while (WiFi.status() != WL_CONNECTED) {
			Serial.print(".");
			delay(1000);
		}

		Serial.print("\nconnecting...");
		while (!m_client.connect("pid-flow-meter-01")) {
			Serial.print(".");
			delay(1000);
		}

		Serial.println("\nconnected!");
	  
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

		publishData();

		if (!m_running)
			return;
	}	

	void publishData()
	{
		String fileName;
		while (true)
		{
			char output[128];
			StaticJsonDocument<128> doc;

			// Read sensor value!
			double val = m_dataSource->getDoubleValue();

			struct timeval tv;
			gettimeofday(&tv, NULL);

			doc["value"] = val;
			doc["ts"] = tv.tv_sec;

			serializeJson(doc, output);

			int len1 = measureJson(doc);

			bool success = m_client.publish("/hello", output);

			if (!success)
				break;

			m_client.loop();

			Serial.println("MQTTPublisher :: sent data!!");

			vTaskDelay(1000);

		}
	}


	void onParamChange(String param, String value)
	{
	
	}

};


void MQTTRealTimePublisher_Task(void* param)
{
	MQTTRealTimePublisher* mqttRealTimePublisher = (MQTTRealTimePublisher*)param;

	mqttRealTimePublisher->runTask();

	vTaskDelete(0);
}
