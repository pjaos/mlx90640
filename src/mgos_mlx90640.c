/**
 * @copyright (C) 2019 Paul Austen (pausten.os@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 *  Responsible for providing an interface to the mlx90640 device.
 */

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_mlx90640_api.h"
#include "mgos_mlx90640.h"

//MLX90640 EEprom data
static uint16_t eeMLX90640[832];

//The raw buffer read from the MLX90640 device
uint16_t frame[834];

//Structure used when converting raw data to a te temp array
static paramsMLX90640 mlx90640;

//The temp array holding the 32x24 array (line by line) data.
//Each value element is the temperature in degrees celsius.
static float mlx90640To[MLX90640_FRAME_BUFFER_SIZE];

/**
 * @brief Read data from the MLX90640 over the I2C bus.
 * @param slaveAddr The MLX90640 device address on the I2C bus
 * @param startAddress The start address to read data from the MLX90640
 * @param nMemAddressRead The number of 16 bit values to read from the MLX90640
 * @param data A pointer to the buffer to hold the 16 bit values read from the MLX90640
 */
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data) {
	int returnCode = 1;
	struct mgos_i2c *i2c_conn = mgos_i2c_get_global();
	int byteCount = nMemAddressRead*2;
	//Note, the mlx90640 uses 16 bit register addressing, big endian
	uint8_t buf[2] = {(startAddress&0xff00)>>8, startAddress&0xff};
	bool ok = mgos_i2c_write(i2c_conn, slaveAddr, buf, 2, false /* stop */) && mgos_i2c_read(i2c_conn, slaveAddr, data, byteCount, true /* stop */);
	if( ok ) {
		uint8_t *byte_data = (uint8_t*)data;
		//Endian swap
		uint8_t tmp=0;
		uint16_t index=0;
		for(index=0; index<byteCount ; index=index+2 ) {
			tmp=byte_data[index];
			byte_data[index]=byte_data[index+1];
			byte_data[index+1]=tmp;
		}
	}
	if( ok) {
		returnCode=0;
	}
	return returnCode;
}

/**
 * @brief Write data to the MLX90640 over the I2C bus.
 * @param slaveAddr The MLX90640 device address on the I2C bus
 * @param writeAddress The start address to write to the MLX90640
 * @param data The 16 bit value to write to the MLX90640
 */
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data) {
	int returnCode = 1;
	struct mgos_i2c *i2c_conn = mgos_i2c_get_global();
	//Note, the mlx90640 uses 16 bit register addressing, big endian
	uint8_t buf[4] = {(writeAddress&0xff00)>>8, writeAddress&0xff, (data&0xff00)>>8, data&0xff};
	bool ok = mgos_i2c_write(i2c_conn, slaveAddr, buf, 4, true /* stop */);
	if( ok) {
		returnCode=0;
	}
	return returnCode;
}

/**
 * @brief Perform all required initialisation actions.
 **/
bool mgos_mlx90640_init(void) {
	uint8_t i2c_device_addr = mgos_sys_config_get_mlx90640_i2c_addr();
	uint8_t fps = mgos_sys_config_get_mlx90640_fps();
	uint8_t adc_bits = mgos_sys_config_get_mlx90640_adc_bits();
	uint8_t refresh_rate = 0;

	MLX90640_SetSubPage(i2c_device_addr, 0);
	MLX90640_SetSubPageRepeat(i2c_device_addr, 0);

	MLX90640_SetDeviceMode(i2c_device_addr, SELECT_SUBPAGE_0);

	if( fps == 0 ) {
		refresh_rate = 0; //A Refresh rate of 0 == 0.5 frames per second refresh rate.
	}
	else if( fps == 1 ) {
		refresh_rate = FPS_1;
	}
	else if( fps == 2 ) {
		refresh_rate = FPS_2;
	}
	else if( fps == 4 ) {
		refresh_rate = FPS_4;
	}
	else if( fps == 8 ) {
		refresh_rate = FPS_8;
	}
	else if( fps == 16 ) {
		refresh_rate = FPS_16;
	}
	else if( fps == 32 ) {
		refresh_rate = FPS_32;
	}
	else if( fps == 64 ) {
		refresh_rate = FPS_64;
	}

	MLX90640_SetRefreshRate(i2c_device_addr, refresh_rate);

	MLX90640_SetResolution(i2c_device_addr, adc_bits);

	MLX90640_SetChessMode(i2c_device_addr);

	MLX90640_DumpEE(i2c_device_addr, eeMLX90640);

	MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

	LOG(LL_INFO, ("MLX90640 INIT COMPLETE\n"));

	return true;
}

/**
 * @brief Perform read of a single from from the MLX90640 device.
 * @return A pointer to the buffer containing an array of MLX90640_FRAME_BUFFER_SIZE (32x24)
 *         float values.The first 32 values comprise the first line,
 *         the second 32 values comprise the 2nd line and so on.
 *         Each value is a the temp in degrees celsius
 */
float * getMLX90640FramePtr(void) {
	float eTa = 0;
	float emissivity = 1;

	MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
	MLX90640_InterpolateOutliers(frame, eeMLX90640);
	eTa = MLX90640_GetTa(frame, &mlx90640);
	MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

	return mlx90640To;
}
