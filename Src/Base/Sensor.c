#include "Common.h"
#include "Sensor.h"

void Sensor_SetPower(Sensor* pSensor, PWR_STATE state)
{
	Printf("%s Power %s\n.", pSensor->name, state == PWR_ON ? "ON" : "OFF");
}

void Sensor_Start(Sensor* pSensor)
{
}

void Sensor_Stop(Sensor* pSensor)
{
}

void Sensor_Run(Sensor* pSensor)
{
}

void Sensor_Init(Sensor* pSensor, const char* name, uint8 id, SensorType type)
{
	pSensor->id = id;
	pSensor->type = type;
	pSensor->name = name;
}
