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
#ifndef _MLX640_I2C_IF_H_
#define _MLX640_I2C_IF_H_

//The default MLX90640 device address
#define MLX_I2C_ADDR 0x33

#define ENABLE_SUBPAGES_MODE_BIT 0
#define ENABLE_DATA_HOLD_BIT 2
#define ENABLE_SUBPAGES_REPEAT_BIT 3
#define SELECT_SUBPAGE_BASE_BIT 4
#define REFRESH_RATE_CONTROL_BASE_BIT 7
#define RESOLUTION_CONTROL_BASE_BIT 10
#define READING_PATTERN_BIT 12

#define SELECT_SUBPAGE_0 0b000
#define SELECT_SUBPAGE_1 0b001

  //Frame refresh rates
#define FPS_0_5 0b000
#define FPS_1 0b001
#define FPS_2 0b010
#define FPS_4 0b011
#define FPS_8 0b100
#define FPS_16 0b101
#define FPS_32 0b110
#define FPS_64 0b111

#define RESOLUTION_ADC_16_BIT 0b00
#define RESOLUTION_ADC_17_BIT 0b01
#define RESOLUTION_ADC_18_BIT 0b10
#define RESOLUTION_ADC_19_BIT 0b11

#define READING_PATTERN_CHESS 1
#define READING_PATTERN_INTERLEAVED 0

#define MLX90640_FRAME_BUFFER_SIZE 768

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data);
bool mgos_mlx90640_init(void);
float *getMLX90640FramePtr(void);

#endif
