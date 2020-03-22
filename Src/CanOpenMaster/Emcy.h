
#ifndef _EMCY_H_
#define _EMCY_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define	EMCY_OVER_CURRENT	 0x2301 
#define	EMCY_OVER_POWER_LOST 0x3100 

#define	EMCY_SDO_LOST 0x3100 
	/**
	 * @defgroup CO_Emergency Emergency
	 * @ingroup CO_CANopen
	 * @{
	 *
	 * CANopen Emergency protocol.
	 *
	 * Error control and Emergency is used for control internal error state
	 * and for sending a CANopen Emergency message.
	 *
	 * In case of error condition stack or application calls CO_errorReport()
	 * function with indication of the error. Specific error condition is reported
	 * (with CANopen Emergency message) only the first time after it occurs.
	 * Internal state of the error condition is controlled with
	 * @ref CO_EM_errorStatusBits. Specific error condition can also be reset by
	 * CO_errorReset() function. If so, Emergency message is sent with
	 * CO_EM_NO_ERROR indication.
	 *
	 * Some error conditions are informative and some are critical. Critical error
	 * conditions sets the #CO_errorRegisterBitmask_t.
	 *
	 * Latest errors can be read from _Pre Defined Error Field_ (object dictionary,
	 * index 0x1003). @ref CO_EM_errorStatusBits can also be read form CANopen
	 * object dictionary.
	 *
	 * ###Emergency message contents:
	 *
	 *   Byte | Description
	 *   -----|-----------------------------------------------------------
	 *   0..1 | @ref CO_EM_errorCodes.
	 *   2    | #CO_errorRegisterBitmask_t.
	 *   3    | Index of error condition (see @ref CO_EM_errorStatusBits).
	 *   4..7 | Additional argument informative to CO_errorReport() function.
	 *
	 * ####Contents of _Pre Defined Error Field_ (object dictionary, index 0x1003):
	 * bytes 0..3 are equal to bytes 0..3 in the Emergency message.
	 *
	 * @see #CO_Default_CAN_ID_t
	 */


	 /**
	  * CANopen Error register.
	  *
	  * In object dictionary on index 0x1001.
	  *
	  * Error register is calculated from critical internal @ref CO_EM_errorStatusBits.
	  * Generic and communication bits are calculated in CO_EM_process
	  * function, device profile or manufacturer specific bits may be calculated
	  * inside the application.
	  *
	  * Internal errors may prevent device to stay in NMT Operational state. Details
	  * are described in _Error Behavior_ object in Object Dictionary at index 0x1029.
	  */
	typedef enum {
		CO_ERR_REG_GENERIC_ERR = 0x01U, /**< bit 0, generic error */
		CO_ERR_REG_CURRENT = 0x02U, /**< bit 1, current */
		CO_ERR_REG_VOLTAGE = 0x04U, /**< bit 2, voltage */
		CO_ERR_ERG_TEMPERATUR = 0x08U, /**< bit 3, temperature */
		CO_ERR_REG_COMM_ERR = 0x10U, /**< bit 4, communication error (overrun, error state) */
		CO_ERR_REG_DEV_PROFILE = 0x20U, /**< bit 5, device profile specific */
		CO_ERR_REG_RESERVED = 0x40U, /**< bit 6, reserved (always 0) */
		CO_ERR_REG_MANUFACTURER = 0x80U  /**< bit 7, manufacturer specific */
	}CO_errorRegisterBitmask_t;


	/**
	 * @defgroup CO_EM_errorCodes CANopen Error codes
	 * @{
	 *
	 * Standard error codes according to CiA DS-301 and DS-401.
	 */
#define CO_EMC_NO_ERROR                 0x0000U /**< 0x00xx, error Reset or No Error */
#define CO_EMC_GENERIC                  0x1000U /**< 0x10xx, Generic Error */
#define CO_EMC_CURRENT                  0x2000U /**< 0x20xx, Current */
#define CO_EMC_CURRENT_INPUT            0x2100U /**< 0x21xx, Current, device input side */
#define CO_EMC_CURRENT_INSIDE           0x2200U /**< 0x22xx, Current inside the device */
#define CO_EMC_CURRENT_OUTPUT           0x2300U /**< 0x23xx, Current, device output side */
#define CO_EMC_VOLTAGE                  0x3000U /**< 0x30xx, Voltage */
#define CO_EMC_VOLTAGE_MAINS            0x3100U /**< 0x31xx, Mains Voltage */
#define CO_EMC_VOLTAGE_INSIDE           0x3200U /**< 0x32xx, Voltage inside the device */
#define CO_EMC_VOLTAGE_OUTPUT           0x3300U /**< 0x33xx, Output Voltage */
#define CO_EMC_TEMPERATURE              0x4000U /**< 0x40xx, Temperature */
#define CO_EMC_TEMP_AMBIENT             0x4100U /**< 0x41xx, Ambient Temperature */
#define CO_EMC_TEMP_DEVICE              0x4200U /**< 0x42xx, Device Temperature */
#define CO_EMC_HARDWARE                 0x5000U /**< 0x50xx, Device Hardware */
#define CO_EMC_SOFTWARE_DEVICE          0x6000U /**< 0x60xx, Device Software */
#define CO_EMC_SOFTWARE_INTERNAL        0x6100U /**< 0x61xx, Internal Software */
#define CO_EMC_SOFTWARE_USER            0x6200U /**< 0x62xx, User Software */
#define CO_EMC_DATA_SET                 0x6300U /**< 0x63xx, Data Set */
#define CO_EMC_ADDITIONAL_MODUL         0x7000U /**< 0x70xx, Additional Modules */
#define CO_EMC_MONITORING               0x8000U /**< 0x80xx, Monitoring */
#define CO_EMC_COMMUNICATION            0x8100U /**< 0x81xx, Communication */
#define CO_EMC_CAN_OVERRUN              0x8110U /**< 0x8110, CAN Overrun (Objects lost) */
#define CO_EMC_CAN_PASSIVE              0x8120U /**< 0x8120, CAN in Error Passive Mode */
#define CO_EMC_HEARTBEAT                0x8130U /**< 0x8130, Life Guard Error or Heartbeat Error */
#define CO_EMC_BUS_OFF_RECOVERED        0x8140U /**< 0x8140, recovered from bus off */
#define CO_EMC_CAN_ID_COLLISION         0x8150U /**< 0x8150, CAN-ID collision */
#define CO_EMC_PROTOCOL_ERROR           0x8200U /**< 0x82xx, Protocol Error */
#define CO_EMC_PDO_LENGTH               0x8210U /**< 0x8210, PDO not processed due to length error */
#define CO_EMC_PDO_LENGTH_EXC           0x8220U /**< 0x8220, PDO length exceeded */
#define CO_EMC_DAM_MPDO                 0x8230U /**< 0x8230, DAM MPDO not processed, destination object not available */
#define CO_EMC_SYNC_DATA_LENGTH         0x8240U /**< 0x8240, Unexpected SYNC data length */
#define CO_EMC_RPDO_TIMEOUT             0x8250U /**< 0x8250, RPDO timeout */
#define CO_EMC_EXTERNAL_ERROR           0x9000U /**< 0x90xx, External Error */
#define CO_EMC_ADDITIONAL_FUNC          0xF000U /**< 0xF0xx, Additional Functions */
#define CO_EMC_DEVICE_SPECIFIC          0xFF00U /**< 0xFFxx, Device specific */

#define CO_EMC401_OUT_CUR_HI            0x2310U /**< 0x2310, DS401, Current at outputs too high (overload) */
#define CO_EMC401_OUT_SHORTED           0x2320U /**< 0x2320, DS401, Short circuit at outputs */
#define CO_EMC401_OUT_LOAD_DUMP         0x2330U /**< 0x2330, DS401, Load dump at outputs */
#define CO_EMC401_IN_VOLT_HI            0x3110U /**< 0x3110, DS401, Input voltage too high */
#define CO_EMC401_IN_VOLT_LOW           0x3120U /**< 0x3120, DS401, Input voltage too low */
#define CO_EMC401_INTERN_VOLT_HI        0x3210U /**< 0x3210, DS401, Internal voltage too high */
#define CO_EMC401_INTERN_VOLT_LO        0x3220U /**< 0x3220, DS401, Internal voltage too low */
#define CO_EMC401_OUT_VOLT_HIGH         0x3310U /**< 0x3310, DS401, Output voltage too high */
#define CO_EMC401_OUT_VOLT_LOW          0x3320U /**< 0x3320, DS401, Output voltage too low */
	 /** @} */


	 /**
	  * @defgroup CO_EM_errorStatusBits Error status bits
	  * @{
	  *
	  * Internal indication of the error condition.
	  *
	  * Each error condition is specified by unique index from 0x00 up to 0xFF.
	  * Variable  (from manufacturer section in the Object
	  * Dictionary) contains up to 0xFF bits (32bytes) for the identification of the
	  * specific error condition. (Type of the variable is CANopen OCTET_STRING.)
	  *
	  * If specific error occurs in the stack or in the application, CO_errorReport()
	  * sets specific bit in the _Error Status Bits_ variable. If bit was already
	  * set, function returns without any action. Otherwise it prepares emergency
	  * message.
	  *
	  * CO_errorReport(), CO_errorReset() or CO_isError() functions are called
	  * with unique index as an argument. (However CO_errorReport(), for example, may
	  * be used with the same index on multiple places in the code.)
	  *
	  * Macros defined below are combination of two constants: index and
	  * @ref CO_EM_errorCodes. They represents specific error conditions. They are
	  * used as double argument for CO_errorReport(), CO_errorReset() and
	  * CO_isError() functions.
	  *
	  * Stack uses first 6 bytes of the _Error Status Bits_ variable. Device profile
	  * or application may define own macros for Error status bits using
	  * @ref CO_EM_MANUFACTURER_START and @ref CO_EM_MANUFACTURER_END values. Note that
	  * _Error Status Bits_ must be large enough (up to 32 bytes).
	  */
#define CO_EM_NO_ERROR                  0x00U /**< 0x00, Error Reset or No Error */
#define CO_EM_CAN_BUS_WARNING           0x01U /**< 0x01, communication, info, CAN bus warning limit reached */
#define CO_EM_RXMSG_WRONG_LENGTH        0x02U /**< 0x02, communication, info, Wrong data length of the received CAN message */
#define CO_EM_RXMSG_OVERFLOW            0x03U /**< 0x03, communication, info, Previous received CAN message wasn't processed yet */
#define CO_EM_RPDO_WRONG_LENGTH         0x04U /**< 0x04, communication, info, Wrong data length of received PDO */
#define CO_EM_RPDO_OVERFLOW             0x05U /**< 0x05, communication, info, Previous received PDO wasn't processed yet */
#define CO_EM_CAN_RX_BUS_PASSIVE        0x06U /**< 0x06, communication, info, CAN receive bus is passive */
#define CO_EM_CAN_TX_BUS_PASSIVE        0x07U /**< 0x07, communication, info, CAN transmit bus is passive */
#define CO_EM_NMT_WRONG_COMMAND         0x08U /**< 0x08, communication, info, Wrong NMT command received */
#define CO_EM_09_unused                 0x09U /**< 0x09, (unused) */
#define CO_EM_0A_unused                 0x0AU /**< 0x0A, (unused) */
#define CO_EM_0B_unused                 0x0BU /**< 0x0B, (unused) */
#define CO_EM_0C_unused                 0x0CU /**< 0x0C, (unused) */
#define CO_EM_0D_unused                 0x0DU /**< 0x0D, (unused) */
#define CO_EM_0E_unused                 0x0EU /**< 0x0E, (unused) */
#define CO_EM_0F_unused                 0x0FU /**< 0x0F, (unused) */

#define CO_EM_10_unused                 0x10U /**< 0x10, (unused) */
#define CO_EM_11_unused                 0x11U /**< 0x11, (unused) */
#define CO_EM_CAN_TX_BUS_OFF            0x12U /**< 0x12, communication, critical, CAN transmit bus is off */
#define CO_EM_CAN_RXB_OVERFLOW          0x13U /**< 0x13, communication, critical, CAN module receive buffer has overflowed */
#define CO_EM_CAN_TX_OVERFLOW           0x14U /**< 0x14, communication, critical, CAN transmit buffer has overflowed */
#define CO_EM_TPDO_OUTSIDE_WINDOW       0x15U /**< 0x15, communication, critical, TPDO is outside SYNC window */
#define CO_EM_16_unused                 0x16U /**< 0x16, (unused) */
#define CO_EM_17_unused                 0x17U /**< 0x17, (unused) */
#define CO_EM_SYNC_TIME_OUT             0x18U /**< 0x18, communication, critical, SYNC message timeout */
#define CO_EM_SYNC_LENGTH               0x19U /**< 0x19, communication, critical, Unexpected SYNC data length */
#define CO_EM_PDO_WRONG_MAPPING         0x1AU /**< 0x1A, communication, critical, Error with PDO mapping */
#define CO_EM_HEARTBEAT_CONSUMER        0x1BU /**< 0x1B, communication, critical, Heartbeat consumer timeout */
#define CO_EM_HB_CONSUMER_REMOTE_RESET  0x1CU /**< 0x1C, communication, critical, Heartbeat consumer detected remote node reset */
#define CO_EM_1D_unused                 0x1DU /**< 0x1D, (unused) */
#define CO_EM_1E_unused                 0x1EU /**< 0x1E, (unused) */
#define CO_EM_1F_unused                 0x1FU /**< 0x1F, (unused) */

#define CO_EM_EMERGENCY_BUFFER_FULL     0x20U /**< 0x20, generic, info, Emergency buffer is full, Emergency message wasn't sent */
#define CO_EM_21_unused                 0x21U /**< 0x21, (unused) */
#define CO_EM_MICROCONTROLLER_RESET     0x22U /**< 0x22, generic, info, Microcontroller has just started */
#define CO_EM_23_unused                 0x23U /**< 0x23, (unused) */
#define CO_EM_24_unused                 0x24U /**< 0x24, (unused) */
#define CO_EM_25_unused                 0x25U /**< 0x25, (unused) */
#define CO_EM_26_unused                 0x26U /**< 0x26, (unused) */
#define CO_EM_27_unused                 0x27U /**< 0x27, (unused) */

#define CO_EM_WRONG_ERROR_REPORT        0x28U /**< 0x28, generic, critical, Wrong parameters to CO_errorReport() function */
#define CO_EM_ISR_TIMER_OVERFLOW        0x29U /**< 0x29, generic, critical, Timer task has overflowed */
#define CO_EM_MEMORY_ALLOCATION_ERROR   0x2AU /**< 0x2A, generic, critical, Unable to allocate memory for objects */
#define CO_EM_GENERIC_ERROR             0x2BU /**< 0x2B, generic, critical, Generic error, test usage */
#define CO_EM_GENERIC_SOFTWARE_ERROR    0x2CU /**< 0x2C, generic, critical, Software error */
#define CO_EM_INCONSISTENT_OBJECT_DICT  0x2DU /**< 0x2D, generic, critical, Object dictionary does not match the software */
#define CO_EM_CALCULATION_OF_PARAMETERS 0x2EU /**< 0x2E, generic, critical, Error in calculation of device parameters */
#define CO_EM_NON_VOLATILE_MEMORY       0x2FU /**< 0x2F, generic, critical, Error with access to non volatile device memory */

#define CO_EM_MANUFACTURER_START        0x30U /**< 0x30, manufacturer, info, This can be used by macros to calculate error codes */
#define CO_EM_MANUFACTURER_END          0xFFU /**< 0xFF, manufacturer, info, This can be used by macros to check error codes */
	  /** @} */

	typedef struct
	{
		uint8 errorBit;
		uint16 errorCode;
		uint32 infoCode;
	}EmcyCode;

	void EmcyCode_Set(EmcyCode* p, uint8 errorBit, uint16 errorCode, uint32 infoCode);


	typedef struct
	{
		uint8 addr;
		uint8 len;
		uint16 errCode;
		uint8 errReg;
		uint8 additionalInfo[5];
	}EmcyPkt;

	EmcyPkt* Emcy_Build(uint8 nodeId, uint16 errCode, uint8 errReg);

#ifdef __cplusplus
}
#endif

#endif
