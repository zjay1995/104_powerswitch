#include "inc/DataLogger.h"
#include "inc/GasManager.h"
#include "inc/MQTTFlashPublisher.h"
#include "inc/MQTTRealTimePublisher.h"
#include "inc/DataSource.h"

//portMUX_TYPE DataLogger::s_mtxLogger = portMUX_INITIALIZER_UNLOCKED;


void DataLogger::init(DataSource* dataSource, GasManager* gasManager)
{
	m_mqttFlashPublisher = new MQTTFlashPublisher();
	m_mqttRealTimePublisher = new MQTTRealTimePublisher(dataSource);

	m_gasManager = gasManager;
	m_dataSource = dataSource;

	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Check if logging session wasn't stopped gracefully
	checkIfGracefulShutdown();
}

bool DataLogger::shouldExecTick()
{
	unsigned long lastTickTime = m_lastTickMillis;

	unsigned long millisPassed = millis() - lastTickTime;

	//Serial.println("Millis passed: " + String(millisPassed));

	if (millisPassed < m_secondsBetweenDataPoints * 1000)
		return false;
	else
	{
		m_lastTickMillis = millis();
		return true;
	}

}

void DataLogger::handleTick()
{
	// Read dataSource here anyway to have fresh value for dataloggers when we're off the RUN menu screen
	double val = m_dataSource->getDoubleValue();

	bool execTick = shouldExecTick();

	if (!m_sessionRunning || !execTick)
	{
		return;
	}

	

	if (!m_file)
	{
		Serial.println("handleTick:: can't open file with name:" + m_currentFileName);
		stopFlashStoreSession();
		return;
	}

	Serial.println("handleTick:: writting to file: " + String(val));

	if (m_file.write((const uint8_t*)&val, sizeof(double)) != sizeof(double))
	{
		Serial.println("handleTick:: can't write to file with name:" + m_currentFileName);
		stopFlashStoreSession();
		return;
	}

	m_file.flush();

	m_currentValueCount++;

	if (m_currentValueCount >= m_singleFileLimit)
	{
		Serial.println("handleTick:: file limit passed! closing file! ");

		m_file.close();

		String newName = m_currentFileName.substring(0, m_currentFileName.indexOf(".running"));
		newName += ".done";
		if (!SPIFFS.rename(m_currentFileName.c_str(), newName.c_str()))
			Serial.println("handleTick:: failed renaming file: " + newName);


		m_fileIndex++;
		m_currentFileName = createFileName(m_currentSessionTimestamp, m_gasManager->getSelectedGas().getName(), m_secondsBetweenDataPoints, m_fileIndex);

		m_file = SPIFFS.open(m_currentFileName, "wb");

		if (!m_file)
		{
			Serial.println("handleTick:: can't open file with name:" + m_currentFileName);
			return;
		}

		m_currentValueCount = 0;

		Serial.println("handleTick:: opening new file: " + m_currentFileName);
	}
}

bool DataLogger::isWiFiDumpRunning()
{
	return m_mqttFlashPublisher->isRunning();
}

void DataLogger::stopWiFiDumpSession()
{
	m_mqttFlashPublisher->stop();

}

void DataLogger::startWiFiDumpSession()
{
	m_mqttFlashPublisher->start();
}

void DataLogger::startWiFiRealTimeDumpSession()
{
	m_mqttRealTimePublisher->start();
}

bool DataLogger::isWiFiRealTimeDumpRunning()
{
	return m_mqttRealTimePublisher->isRunning();
}

void DataLogger::stopWiFiRealTimeDumpSession()
{
	m_mqttRealTimePublisher->stop();

}


String DataLogger::createFileName(String ts, String gas, int secondsBetweenDataPoints, int fileIndex)
{
	return  String("/") +
			m_currentSessionTimestamp +
			String("_") +
			m_gasManager->getSelectedGas().getName() +
			String("_") +
			String(secondsBetweenDataPoints) +
			String("_") +
			String(fileIndex) +
			String(".running");
}

bool DataLogger::startFlashStoreSession()
{
	if (m_sessionRunning)
	{
		Serial.println("startSession:: session is already running!");
		return false;
	}

	m_fileIndex = 0;

	m_currentSessionTimestamp = getCurrentDateTimeStr();

	m_currentFileName = createFileName(m_currentSessionTimestamp, m_gasManager->getSelectedGas().getName(), m_secondsBetweenDataPoints, m_fileIndex);

	Serial.println("startSession:: !!! " + m_currentFileName);

	m_file = SPIFFS.open(m_currentFileName, "wb");

	if (!m_file)
	{
		Serial.println("startSession:: can't open file: " + m_currentFileName);
		return false;
	}

	m_sessionRunning = true;

	m_lastTickMillis = 0;
}

bool DataLogger::stopFlashStoreSession()
{
	if (!m_sessionRunning)
	{
		Serial.println("startSession:: session you're trying to stop is not running!");
		return false;
	}

	Serial.println("stopSession:: !!!");

	String fname = String(m_file.name());

	m_file.close();

	String newName = fname.substring(0, fname.indexOf(".running"));
	newName += ".done";
	if (SPIFFS.exists(fname.c_str()) && !SPIFFS.rename(fname.c_str(), newName.c_str()))
		Serial.print("failed to rename file: " + fname + " new: " + newName);

	m_fileIndex = 0;
	m_currentValueCount = 0;
	m_currentFileName = "";
	m_currentSessionTimestamp = "";

	m_sessionRunning = false;

}

bool DataLogger::isFlashStoreSessionRunning() const { return m_sessionRunning; }

String DataLogger::getCurrentDateTimeStr()
{
	struct timeval tv;
	time_t t;
	struct tm* info;
	char buffer[64] = { 0 };

	gettimeofday(&tv, NULL);
	t = tv.tv_sec;

	/*
	info = localtime(&t);
	printf("%s",asctime (info));
	strftime (buffer, sizeof buffer, "%Y%m%d_%H%M%S", info);
	*/
	return String(tv.tv_sec);
}

void DataLogger::checkIfGracefulShutdown()
{
	Serial.printf("Listing directory:\n");

	File root = SPIFFS.open("/");
	if (!root) {
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory()) {
		Serial.println("Not a directory");
		return;
	}

	Serial.println("listdir start");
	File file = root.openNextFile();
	while (file) {
		if (file.isDirectory()) {
			Serial.print("  DIR : ");
			Serial.print(file.name());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
		}
		else {
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.print(file.size());
			time_t t = file.getLastWrite();
			struct tm* tmstruct = localtime(&t);
			Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

			String fname = String(file.name());
			if (fname.endsWith(".running"))
			{
				String newName = fname.substring(0, fname.indexOf(".running"));
				newName += ".done";
				if (!SPIFFS.rename(file.name(), newName.c_str()))
					Serial.println("failed renaming file: " + newName);
				else
					Serial.println("success renaming file: " + newName);
			}
			else if (!fname.endsWith(".done"))
				SPIFFS.remove(file.name());
		}
		file = root.openNextFile();
	}

	Serial.println("listdir Exit");
}

MQTTFlashPublisher*		 DataLogger::getMqttFlashPublisher()	const { return m_mqttFlashPublisher;	}
MQTTRealTimePublisher*	 DataLogger::getMqttRealTimePublisher()	const { return m_mqttRealTimePublisher;	}
