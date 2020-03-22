#ifndef __SENSOR_H_
#define __SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

	typedef enum
	{
		ST_WALTER_LEVEL,
		ST_SMOKE,
		ST_TEMP,
	}SensorType;

	typedef void (*SensorFn)();
	typedef struct
	{
		uint8 id;
		SensorType type;
		const char* name;
	}Sensor;

	void Sensor_SetPower(Sensor* pSensor, PWR_STATE state);
	void Sensor_Init(Sensor* pSensor, const char* name, uint8 id, SensorType st);
	void Sensor_Start(Sensor* pSensor);
	void Sensor_Stop(Sensor* pSensor);
	void Sensor_Run(Sensor* pSensor);

#ifdef __cplusplus
}
#endif

#endif

