/*
 * AppSettings.h
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: Anakod
 */

#include <SmingCore.h>

#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

#define APP_SETTINGS_FILE ".settings.conf" // leading point for security reasons :)

struct ApplicationSettingsStorage {
	String ssid;
	String password;

	bool dhcp = true;

	IPAddress ip;
	IPAddress netmask;
	IPAddress gateway;

	void load()
	{
		DynamicJsonDocument doc(1024);
		if(exist()) {
			int size = fileGetSize(APP_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_SETTINGS_FILE, jsonString, size + 1);
			auto error = deserializeJson(doc, jsonString);
			if(!error) {
				JsonObject network = doc["network"].as<JsonObject>();
				ssid = network["ssid"].as<String>();
				password = network["password"].as<String>();

				dhcp = network["dhcp"];

				ip = network["ip"].as<String>();
				netmask = network["netmask"].as<String>();
				gateway = network["gateway"].as<String>();
			}

			delete[] jsonString;
		}
	}

	void save()
	{
		DynamicJsonDocument doc(1024);

		auto network = doc.createNestedObject("network");
		network["ssid"] = ssid.c_str();
		network["password"] = password.c_str();

		network["dhcp"] = dhcp;

		// Make copy by value for temporary string objects
		network["ip"] = ip.toString();
		network["netmask"] = netmask.toString();
		network["gateway"] = gateway.toString();

		//TODO: add direct file stream writing
		String rootString;
		serializeJson(doc, rootString);
		fileSetContent(APP_SETTINGS_FILE, rootString);
	}

	bool exist()
	{
		return fileExist(APP_SETTINGS_FILE);
	}
};

static ApplicationSettingsStorage AppSettings;

#endif /* INCLUDE_APPSETTINGS_H_ */
