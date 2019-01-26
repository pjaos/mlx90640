# MLX90640
Mongoose OS driver for the MLX90640 IR Array device.

## Introduction

The MLX90640 is a 32x24 infra red array device for measuring temperature
without contact. The interface to the device is via an I2C interface.

### API

This driver attempts to provide a simple interface to the IR device via
the I2C interface. After initialisation of the device the getMLX90640FramePtr()
function can be called. This returns a pointer to 768 (32x24) float values.
Each value contains the temperature in degrees Celsius.  

#### Details

The I2C bus (SCL and SDA) must be connected to the device along with
3V3 and GND. The default I2C address for the MLX90640 device is 0x33.
The initialisation of the device is performed automagically as the
mongoose framework ensures that the mgos_mlx90640_init() function is
called. The parameters passed for initialisation are held in the
mos.yml file.


### Example Application

#### mos.yml

The driver uses the Mongoose native SPI driver. It is configured by setting
up the `MOSI`, `MISO`, `SCLK` pins and assigning one of the three
available `CS` positions, in this example we use `CS1`:

```
config_schema:
 - ["i2c.enable",   "b", true, {title: "Enable I2C"}]
 - ["i2c.sda_gpio", "i", 22, {title: "GPIO to use for SDA"}]
 - ["i2c.scl_gpio", "i", 21, {title: "GPIO to use for SCL"}]
 - ["i2c.freq",     "i", 400000, {title: "I2C bus speed (Hz)"}]
 - ["i2c.unit_no",  "i", 0, {title: "Which hardware unit to use, 0 or 1"}]
 - ["mlx90640.fps", "i", 1,  {title: "The MLX90640 device frames per second configuration."}]
```

#### Application

```
#include "mgos.h"
#include "mgos_mlx90640.h"

static void mlx90640Read_cb(void *arg) {
    int frameBufIndex=0;

    //Read frame from thermal camera
    float *framePtr = getMLX90640FramePtr();

    //Display temperature values (C) on serial debug port
    for( frameBufIndex=0 ; frameBufIndex<MLX90640_FRAME_BUFFER_SIZE ; frameBufIndex++ ) {
      LOG(LL_INFO, ("frameBufIndex[%d]=%.1fC", frameBufIndex, framePtr[frameBufIndex] ));
    }

}

enum mgos_app_init_result mgos_app_init(void) {

  //Every 5 seconds read the MLX90640
  mgos_set_timer(5000, MGOS_TIMER_REPEAT, mlx90640Read_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}
```
