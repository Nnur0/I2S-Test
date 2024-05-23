#include <Arduino.h>
#include "driver/i2s.h"
#include "freertos/queue.h"
#include <cmath>

i2s_port_t i2s_in = I2S_NUM_0; // i2s port number
i2s_port_t i2s_out = I2S_NUM_1; // i2s port number

static const i2s_config_t i2s_config_in = {
     .mode = i2s_mode_t(I2S_MODE_SLAVE | I2S_MODE_RX),
     .sample_rate = 48000 ,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = I2S_COMM_FORMAT_STAND_I2S,
     .intr_alloc_flags = 0, // default interrupt priority
     .dma_buf_count = 8,
     .dma_buf_len = 1024,
     .use_apll = false,
     .tx_desc_auto_clear = false
};

static const i2s_config_t i2s_config_out = {
     .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX), //i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
     .sample_rate = 48000,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
     .communication_format = I2S_COMM_FORMAT_STAND_I2S, //i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S | I2S_COMM_FORMAT_STAND_MSB),
     .intr_alloc_flags = 1, // default interrupt priority
     .dma_buf_count = 8,
     .dma_buf_len = 1024,
     .use_apll = false,
     .tx_desc_auto_clear = false
};

static const i2s_pin_config_t pin_config = {
    .bck_io_num = 5,
    .ws_io_num = 25,
    .data_out_num = 22,
    .data_in_num = 26
};

static const i2s_pin_config_t pin_config2 = {
    .bck_io_num = 32,
    .ws_io_num = 33,
    .data_out_num = 27,
    .data_in_num = 35
};
// put function declarations here:

void setup() {
  
  i2s_driver_install(i2s_in, &i2s_config_in, 0, NULL);   //install and start i2s driver
  i2s_driver_install(i2s_out, &i2s_config_out, 0, NULL);   //install and start i2s driver

  i2s_set_pin(i2s_in, &pin_config);
  i2s_set_pin(i2s_out, &pin_config2);
  //Serial.begin(115200);
}

#define type 1
const int expon = 27;
const double f = 1000; 
TickType_t ticks = 0;

size_t num;
const int32_t amplitude = pow(2, expon); 
int32_t sample = amplitude * -1; 
int64_t t = 0;
int64_t last_t = 0;
const double T = 1000.0/f;
const double m = (amplitude*2.0)/(T*1000.0);

void loop() {
  t = esp_timer_get_time();       //double(esp_timer_get_time())/1000000.0;

  if (type == 0)
  {
    sample = amplitude * sin(2.0*M_PI*f*(t/1000000.0));
  }
  else if (type == 1)
  {
    sample = sin(2.0 * M_PI*f*(t/1000000.0))>=0.0 ? amplitude:-amplitude;
  }
  else if (type == 2)
  {
    if ((t - last_t) >= (T*1000))
    {
      last_t = t;
      sample = amplitude * -1;
    }
    else
    {
      sample += m*(t - last_t);
    }
  }
  
  i2s_write(i2s_out, &sample, sizeof(sample), &num, ticks);
  //sample *=-1; 
  //i2s_write(i2s_out, &sample, sizeof(sample), &num, ticks);
  //sample *=-1; 
  //Serial.println(sample);
 }
