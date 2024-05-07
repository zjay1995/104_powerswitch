#pragma once

#include "GasManager.h"
#include <vector>

#include "Globals.h"
#include "ConfigurationManager.h"

#define DEBUG_PRINT_ON

class WebServer
{

	const int c_HEADER_BUFFER_SIZE = 1436;
	const int c_MAX_PARAM_NAME_LEN = 30;
	const int c_MAX_PARAM_VALUE_LEN = 30;
	
	WiFiServer m_wifiServer;
	WiFiClient m_client;
	
	GasManager* m_gasManager;
	
	unsigned int m_timeoutMillis = 500;


	std::vector<ParamChangeListener*> m_paramChangeListeners;
	
public:


	WebServer() : m_gasManager(nullptr)
	{
		
		
	}
	
	~WebServer() = default;
	
	void init(GasManager* gasManager)
	{
		m_gasManager = gasManager;
	}
	
	void stop()
	{
		m_wifiServer.stop();
	}
	
	void begin(int port = 0)
	{
		m_wifiServer.begin(port);
	}
	
	void notifyOnParamFound(String param, String value)
	{
		#ifdef DEBUG_PRINT_ON
		Serial.println(String(param) + "=" + String(value));
		#endif

		for (auto& l : m_paramChangeListeners)
			l->onParamChange(param, value);

		//if(m_gasManager != nullptr)
		//	m_gasManager->onGasParamChange(param, paramLen, value, valueLen);
	}
	
	int processHeaderLine(char* buf, int len)
	{
		int paramCount = 0;
		
		char paramName[c_MAX_PARAM_NAME_LEN];
		int paramNamePos = 0;
		
		char paramValue[c_MAX_PARAM_VALUE_LEN];
		int paramValuePos = 0;
		
		bool paramFound = false;
		bool valueFound = false;
		bool qFound = false;
		for(int i=0; i < len; i++)
		{
			if(buf[i] == '&')
			{
				if(!valueFound)
				{
					//ERROR
					return paramCount;
				}
				
				paramValue[paramValuePos] = 0;

				#ifdef DEBUG_PRINT_ON
				Serial.println("processHeaderLine :: paramValue=" + String(paramValue)); 
				#endif
				notifyOnParamFound(String(paramName), String(paramValue));
				paramNamePos = 0;
				paramValuePos = 0;
				
				valueFound = false;
				paramFound = true;
				
				paramCount++;
			}
			else if(buf[i] == '?')
			{
				#ifdef DEBUG_PRINT_ON
				Serial.println("processHeaderLine :: ?"); 
				#endif
				paramFound = true;
				qFound = true;
			}
			else if(buf[i] == '=')
			{
				if(!paramFound)
				{
					//ERROR
					return paramCount;
				}
				
				
				
				paramName[paramNamePos] = 0;
				
				#ifdef DEBUG_PRINT_ON
				Serial.println("processHeaderLine :: paramName=" + String(paramName)); 
				#endif
				valueFound = true;
				paramFound = false;
				
				paramValuePos = 0;
				
			}
			else if(buf[i] == ' ' || 
					(buf[i] == 'r' && i + 2 == len && buf[i + 1] == 'n')
					)
			{
				#ifdef DEBUG_PRINT_ON
				Serial.println("processHeaderLine :: r n"); 
				#endif
				if(valueFound)
				{
					paramValue[paramValuePos] = 0;
					notifyOnParamFound(String(paramName), String(paramValue));
					paramCount++;			
					return paramCount;
				}
				
				if(i + 4 < len && 
					buf[i + 1] == 'H' &&
					buf[i + 2] == 'T' &&
					buf[i + 3] == 'T' &&
					buf[i + 4] == 'P'
					)
					return paramCount;
			}
			else
			{
				#ifdef DEBUG_PRINT_ON
				Serial.println("processHeaderLine :: else"); 
				#endif
				if(paramFound)
					paramName[paramNamePos++] = buf[i];
				else if(valueFound)
					paramValue[paramValuePos++] = buf[i];
			}
			
		}
		
		return paramCount;
	}
	
	void handleTick()
	{
		unsigned int startMillis = millis();
		
		char headerBuf[1436];
		int headerBufPos = 0;

		if(!m_wifiServer.hasClient())
			return;
		
		m_client = m_wifiServer.available();   // Listen for incoming clients

		// If a new client connects,
		Serial.println("New Client.");          // print a message out in the serial port
		while (m_client.connected() && millis() - startMillis < m_timeoutMillis ) 
		{            // loop while the client's connected
			Serial.println("try read fd=" + String(m_client.fd()));    
 
			if(headerBufPos >= c_HEADER_BUFFER_SIZE)
			{
				#ifdef DEBUG_PRINT_ON
				Serial.println("Tick :: headerBufPos >= c_HEADER_BUFFER_SIZE=" + String(headerBufPos)); 
				#endif
				m_client.flush();
				m_client.stop();
				return;
			}
	
			#ifdef DEBUG_PRINT_ON
			Serial.println("Tick :: m_client.read"); 
			#endif
			int bytesRead = m_client.read((uint8_t*)headerBuf + headerBufPos, 1436 - headerBufPos);// read a byte, then
			#ifdef DEBUG_PRINT_ON
			Serial.println("Tick :: bytesRead=" + String(bytesRead) + " pos=" + String(headerBufPos)); 
			#endif
			
			if(bytesRead < 0)
			{
				break;
			}
			
			for(int i=0; i < bytesRead; i++)
			{
				#ifdef DEBUG_PRINT_ON
				Serial.print(headerBuf[headerBufPos + i]);
				#endif
				if(i + 1 < bytesRead &&
					headerBuf[headerBufPos + i] == '\r' &&
					headerBuf[headerBufPos + i + 1] == '\n'								
					)
				{
					#ifdef DEBUG_PRINT_ON
					Serial.println("Tick :: before processHeaderLine "); 
					#endif
					int paramsFound = processHeaderLine(headerBuf, headerBufPos + bytesRead);		
					m_client.flush();
					sendHtmlPage(m_client);		
					m_client.stop();	
					
					return;
				}
			}

			headerBufPos += bytesRead;
		}
		
		m_client.flush();
		m_client.stop();
					
	}
	
	void sendHtmlPage(WiFiClient client) {
	
		// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
		// and a content-type so the client knows what's coming, then a blank line:
		client.println("HTTP/1.1 200 OK");
		client.println("Content-type:text/html");
		client.println("Connection: close");
		client.println();							
		// Display the HTML web page
		client.println("<!DOCTYPE html><html>");
		client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		client.println("<link rel=\"icon\" href=\"data:,\">");
		// CSS to style the on/off buttons 
		// Feel free to change the background-color and font-size attributes to fit your preferences
		client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
		client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
		client.println(".button2 {background-color: #555555;}</style></head>");
		
		// Web Page Heading
		client.println("<body><h1>PID Analyzers M70 Calibration</h1>");
		
		client.println("<form action=\"/\">");
		
		std::vector<Gas>& gases = m_gasManager->getAllGases();
		
		client.println("<hr style=\"height:2px;border-width:0;color:gray;background-color:gray\">");
		client.println("<label for=\"fname\">Calibration parameters:</label><br>");
        client.println("<input type=\"text\" id=\"slope\" name=\"SLOPE\" value=\"" + String(m_gasManager->getSlope(), 6) + "\"></br>");

		client.println("<label for=\"fname\">Intersect:</label>");
		client.println("<input type=\"text\" id=\"intersect\" name=\"INTERCEPT\" value=\"" + String(m_gasManager->getIntercept(), 6) + "\"><p>");
	
		for(int i = 0; i < gases.size(); i++)
		{
			client.println("<hr style=\"height:2px;border-width:0;color:gray;background-color:gray\">");
			client.println("<label for=\"fname\">" + gases[i].getName() + "</label><br>");
			client.println("<label for=\"fname\">Thermal conductivity::</label>");
			client.println("<input type=\"text\" id=\"tc" + String(i) + "\" name=\"" + gases[i].getName() + "\" value=\"" + String(gases[i].getThermalConductivity(), 6) + "\"></p>");
		}

		client.println("<hr style=\"height:2px;border-width:0;color:gray;background-color:gray\">");
		client.println("<label for=\"fname\">WiFi credentials:</label><br>");
		client.println("<label for=\"fname\">SSID:</label>");
		client.println("<input type=\"text\"name=\"" + String(c_WIFI_SSID_PARAM_NAME) + "\" value=\"" + g_configurationManager.getWifiSsid() + "\"></p>");
		client.println("<label for=\"fname\">PASSWORD:</label>");
		client.println("<input type=\"text\" name=\"" + String(c_WIFI_PASSWORD_PARAM_NAME) + "\" value=\"" + g_configurationManager.getWifiPassword() + "\"><br>");

		client.println("<input type=\"submit\" value=\"Modify\" method=\"GET\">");
		client.println("</form>");

		client.println("</body></html>");
		// The HTTP response ends with another blank line
		client.println();
	}

	void addParamChangeListener(ParamChangeListener* l) { m_paramChangeListeners.push_back(l); }
};