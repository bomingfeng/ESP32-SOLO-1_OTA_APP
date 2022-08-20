/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "ADC1_single_read_Task.h"
#include "LED_Seg7Menu/LED_Seg7Menu.h"

//ADC Channels
#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_7

//ADC Attenuation
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11

extern TimerHandle_t Seg7Timers;
extern uint32_t sse_data[sse_len];
extern MessageBufferHandle_t adc_config;

void ADC1_single_read_Task(void *pvParam)
{
    int adc1_data;
    uint32_t adc_config1 = 168;
    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
 
    while (1) {
        xMessageBufferReceive(adc_config,&adc_config1,4,(1000 / portTICK_PERIOD_MS)/*min*/ * 10);
        adc1_data = adc1_get_raw(ADC1_EXAMPLE_CHAN0);
        sse_data[2] = adc1_data;
        if(adc1_data >= adc_config1)
        {
            xTimerReset(Seg7Timers,portMAX_DELAY);
        }
        else
        {        
            xTimerStop(Seg7Timers,portMAX_DELAY);
            SegDyn_Hidden();
        }
        //printf("adc1_data:%d.\r\n",adc1_data);
        //vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
