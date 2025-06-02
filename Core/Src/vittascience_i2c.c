/**
 ********************************************************************************
 * @file    vittascience_i2c.c
 * @author  clement.fmy@gmail.com
 * @date    23/02/2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "vittascience_i2c.h"

#include <string.h>
#include "app_x-cube-ai.h"

/************************************
 * EXTERN VARIABLES
 ************************************/
extern bool do_ai;
#ifdef BSP_CONFIG_VITTASCIENCE
extern I2C_HandleTypeDef hbus_i2c1;
#else
extern I2C_HandleTypeDef hbus_i2c4;
#endif
/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
static uint8_t result_buffer[VITTASCIENCE_I2C_RESULT_SIZE] = {0};
static uint8_t info_buffer[VITTASCIENCE_I2C_INFO_SIZE] = {VITTA_I2C_STATUS_NOT_READY, VITTASCIENCE_I2C_VERSION};

static vittascience_i2c_command_t current_command = VITTA_I2C_NO_COMMAND;
#ifdef BSP_CONFIG_VITTASCIENCE
static I2C_HandleTypeDef *i2c_vitta = &hbus_i2c1;
#else
static I2C_HandleTypeDef *i2c_vitta = &hbus_i2c4;
#endif

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
vittascience_i2c_return_t vittascience_i2c_init(void)
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    printf("%s\r\n", __FUNCTION__);
    info_buffer[0] = VITTA_I2C_STATUS_READY;
    info_buffer[1] = VITTASCIENCE_I2C_VERSION;
    HAL_I2C_EnableListen_IT(i2c_vitta);
    return ret;
}

vittascience_i2c_return_t vittascience_i2c_get_info(vittascience_i2c_info_t *buf)
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    buf = &info_buffer;

    return ret;
}

vittascience_i2c_return_t vittascience_i2c_set_results(uint8_t *buf, uint8_t size)
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    memset(result_buffer, 0, sizeof(result_buffer));
    if (size <= sizeof(result_buffer))
    {
        memcpy(result_buffer, buf, size);
        info_buffer[0] = VITTA_I2C_STATUS_RESULT_AVAILABLE;
    }
    else
    {
        ret = VITTA_I2C_ERROR;
    }
    //printf("BUFFER %d %d %d %d \r\n", result_buffer[0], result_buffer[1], result_buffer[2], result_buffer[3]);
    return ret;
}

vittascience_i2c_return_t vittascience_i2c_get_results(uint8_t *buf)
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    buf = result_buffer;
    info_buffer[0] = VITTA_I2C_STATUS_READY;

    return ret;
}

vittascience_i2c_return_t vittascience_i2c_write_request()
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    HAL_I2C_Slave_Seq_Receive_IT(i2c_vitta, &current_command, 1, I2C_FIRST_AND_LAST_FRAME);
    return ret;
}

vittascience_i2c_return_t vittascience_i2c_read_request()
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    switch (current_command)
    {
    case VITTA_I2C_NO_COMMAND:
        ret = VITTA_I2C_ERROR;
        break;
    case VITTA_I2C_STATUS:
        HAL_I2C_Slave_Seq_Transmit_IT(i2c_vitta, info_buffer, sizeof(info_buffer), I2C_FIRST_AND_LAST_FRAME);
        break;
    case VITTA_I2C_IA_LAUNCH:
        ret = VITTA_I2C_ERROR;
        break;
    case VITTA_I2C_IA_RESULTS:
        // if (info_buffer[0] == VITTA_I2C_STATUS_RESULT_AVAILABLE) {
        HAL_I2C_Slave_Seq_Transmit_IT(i2c_vitta, result_buffer, sizeof(result_buffer), I2C_FIRST_AND_LAST_FRAME);
        //}
        break;
    default:
        ret = VITTA_I2C_ERROR;
        break;
    }
    return ret;
}

vittascience_i2c_return_t vittascience_i2c_receive_cb(void)
{
    vittascience_i2c_return_t ret = VITTA_I2C_OK;
    switch (current_command)
    {
    case VITTA_I2C_NO_COMMAND:
        ret = VITTA_I2C_ERROR;
        break;
    case VITTA_I2C_STATUS:
        ret = VITTA_I2C_ERROR;
        break;
    case VITTA_I2C_IA_LAUNCH:
        if (info_buffer[0] == VITTA_I2C_STATUS_READY)
        {
            //do_ai = true;
        }
        break;
    case VITTA_I2C_IA_RESULTS:
        break;
    default:
        ret = VITTA_I2C_ERROR;
        break;
    }
    return ret;
}

/************************************
 * STATIC FUNCTIONS
 ************************************/
