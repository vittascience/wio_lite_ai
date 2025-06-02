/**
 ********************************************************************************
 * @file    vittascience_i2c.h
 * @author  clement.fmy@gmail.com
 * @date    23/02/2024
 * @brief
 ********************************************************************************
 */

#ifndef __VITTASCIENCE_I2C_H__
#define __VITTASCIENCE_I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif

  /************************************
   * INCLUDES
   ************************************/

#include "i2c.h"

/************************************
 * MACROS AND DEFINES
 ************************************/
#define VITTASCIENCE_I2C_TARGET_ADDRESS 0x20
#define VITTASCIENCE_I2C_VERSION 1
#define VITTASCIENCE_I2C_INFO_SIZE 2
#define VITTASCIENCE_I2C_RESULT_SIZE 10 // Maximum 10 Classes
#ifdef BSP_CONFIG_VITTASCIENCE
#define VITTASCIENCE_I2C_INSTANCE I2C1
#else
#define VITTASCIENCE_I2C_INSTANCE I2C4
#endif
  /************************************
   * TYPEDEFS
   ************************************/
  typedef enum
  {
    VITTA_I2C_OK = 0x00,
    VITTA_I2C_ERROR,
  } vittascience_i2c_return_t;

  typedef enum
  {
    VITTA_I2C_NO_COMMAND = 0x00,
    VITTA_I2C_STATUS = 0x01,
    VITTA_I2C_IA_LAUNCH = 0x02,
    VITTA_I2C_IA_RESULTS = 0x03,
  } vittascience_i2c_command_t;

  typedef enum
  {
    VITTA_I2C_STATUS_NOT_READY = 0x00,
    VITTA_I2C_STATUS_READY = 0x01,
    VITTA_I2C_STATUS_RESULT_AVAILABLE = 0x02,
  } vittascience_i2c_status_t;

  typedef struct
  {
    vittascience_i2c_status_t status;
    uint8_t version;
  } vittascience_i2c_info_t;

  /************************************
   * EXPORTED VARIABLES
   ************************************/

  /************************************
   * GLOBAL FUNCTION PROTOTYPES
   ************************************/
  vittascience_i2c_return_t vittascience_i2c_init(void);
  vittascience_i2c_return_t vittascience_i2c_get_info(vittascience_i2c_info_t *buf);
  vittascience_i2c_return_t vittascience_i2c_set_results(uint8_t *buf, uint8_t size);
  vittascience_i2c_return_t vittascience_i2c_get_results(uint8_t *buf);

  vittascience_i2c_return_t vittascience_i2c_write_request(void);
  vittascience_i2c_return_t vittascience_i2c_read_request(void);
  vittascience_i2c_return_t vittascience_i2c_receive_cb(void);

#ifdef __cplusplus
}
#endif

#endif