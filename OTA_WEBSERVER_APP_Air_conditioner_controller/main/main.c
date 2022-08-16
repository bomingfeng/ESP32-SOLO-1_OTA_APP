#include "ir_rx_task.h"
#include "app_init.h"
#include "led_Task.h"
#include "app_inclued.h"
#include "tcp_client.h"
#include "ir_tx_Task.h"
#include "temperature_control_task.h"
#include "BLE_Client.h"
#include "cpu_timer.h"
#include "MultiButton/MultiButton_poll_Task.h"
#include "sntp_task.h"
#include "LED_Seg7Menu/LED_Seg7Menu.h"
#include "ADC1_single_read_Task.h"

EventGroupHandle_t APP_event_group;

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
/*
断电保持位
*/
RTC_DATA_ATTR uint32_t sleep_keep,test;
RTC_DATA_ATTR uint8_t sleep_ir_data[13];
 
uint8_t ip_addr1 = 0,ip_addr2 = 0,ip_addr3 = 0,ip_addr4 = 0;

extern char * tcprx_buffer;
extern MessageBufferHandle_t tcp_send_data;
extern MessageBufferHandle_t ir_rx_data;
extern MessageBufferHandle_t ir_tx_data;
extern MessageBufferHandle_t ble_degC;  //换算2831 = 28.31
extern MessageBufferHandle_t ble_humidity;
extern MessageBufferHandle_t ble_Voltage;
extern MessageBufferHandle_t ds18b20degC;   //换算2831 = 28.31
extern rmt_channel_t example_rx_channel;
extern rmt_channel_t example_tx_channel;
extern MessageBufferHandle_t IRPS_temp;
extern MessageBufferHandle_t time_hour_min;
extern MessageBufferHandle_t HtmlToMcuData;

extern int32_t BLe_battery;
extern nvs_handle_t BLe_battery_handle;

void test_test(void * arg)
{
    char data[96];
    uint8_t i,ssidOK,datalen,ssidBit = 0,passBit = 5,ssidlen,passlen;

    uint8_t ssid[32];      
    uint8_t password[64];

    //vTaskDelete(NULL);
    while(1)
    {
        datalen = xMessageBufferReceive(HtmlToMcuData,&data,80,portMAX_DELAY);
        printf("datalen:%d;data:%s;\r\n",datalen,data);
        ssidOK = 0;
        for(i = 0;i < 80;i++){
            if(('s' == data[i]) && ('s' == data[i+1]) && ('i' == data[i+2]) && ('d' == data[i+3]) && (':' == data[i+4])){
                ssidBit = i;
                ssidOK++;
            }
            if(('p' == data[i]) && ('a' == data[i+1]) && ('s' == data[i+2]) && ('s' == data[i+3]) && (':' == data[i+4])){
                passBit = i;
                ssidOK++;
                break;
            }
        }
        if(ssidOK == 2){
            printf("ssidBit:%d;passBit:%d;\r\n",ssidBit,passBit);
            ssidlen = passBit - (ssidBit + 5);
            passlen = datalen - (passBit + 5);
            printf("ssidlen:%d;passlen:%d;\r\n",ssidlen,passlen);
            if(ssidlen > 0){
                memset(ssid,'\0',sizeof(ssid));
                for(i = 0;i < ssidlen;i++){
                    ssid[i] = data[ssidBit + i + 5];
                }

                memset(password,'\0',sizeof(password));
                for(i = 0;i < passlen;i++){
                    password[i] = data[passBit + i + 5];
                }
                printf("ssid:%s;password:%s;\r\n",ssid,password);

                if(strcmp(&ssid,CONFIG_STATION_SSID) == 0){
                    printf("strcmp ssid OK\r\n");
                }
                if(strcmp(&password,CONFIG_STATION_PASSPHRASE) == 0){
                    printf("strcmp password OK\r\n");
                }
            }   
        }
    }
} 
       
/*
 *
 * void app_main()
 *
 **/
void app_main()
{
    EventBits_t staBits;

    //printf("welcome ！！！Compiled at:");printf(__TIME__);printf(" ");printf(__DATE__);printf("\r\n");

    //printf("Create the event group,Message......\r\n");
    // Init the event group
	APP_event_group = xEventGroupCreate();

    ble_degC = xMessageBufferCreate(8);
    ble_humidity = xMessageBufferCreate(8);
    ble_Voltage = xMessageBufferCreate(8);
    ds18b20degC = xMessageBufferCreate(8);
    time_hour_min = xMessageBufferCreate(6);
    tcp_send_data  = xMessageBufferCreate(132);
    ir_rx_data  = xMessageBufferCreate(17);
    ir_tx_data =  xMessageBufferCreate(17);
    IRPS_temp = xMessageBufferCreate(8);
    HtmlToMcuData  = xMessageBufferCreate(100);

    //printf("Init GPIO & nvs_flash.....\r\n");
    app_init();

    //printf("Create Task.....\r\n");
    OTA_Task_init();
    // Need this task to spin up, see why in task			
    xTaskCreate(systemRebootTask, "rebootTask", 2048, NULL, ESP_TASK_PRIO_MIN + 1, NULL);

    LED_Task_init();
    xTaskCreate(led_instructions, "led_instructions", 4596, NULL, ESP_TASK_PRIO_MIN + 1, NULL);

	MyWiFi_init();
    xTaskCreate(wifi_ap_sta, "wifi_ap_sta", 2048, NULL, ESP_TASK_PRIO_MIN + 1, NULL);
   
    xTaskCreate(ds18x20_task,      "ds18x20",3072, NULL, ESP_TASK_PRIO_MIN + 1, NULL);

    ir_rx_task_init();
    xTaskCreate(example_ir_rx_task,"ir_rx",  3072, NULL, ESP_TASK_PRIO_MIN + 2, NULL);

    ir_tx_task_init();
    xTaskCreate(example_ir_tx_task,"ir_tx", 3072, NULL, ESP_TASK_PRIO_MIN + 1, NULL);//????

    tempps_task_init();
    xTaskCreate(IRps_task,"IRps_task",  3072, NULL, ESP_TASK_PRIO_MIN + 2,NULL);
    xTaskCreate(tempps_task,"tempps",  3072, NULL, ESP_TASK_PRIO_MIN + 1,NULL);
    xTaskCreate(test_test, "test_test", 4096, NULL, ESP_TASK_PRIO_MIN + 1, NULL);
    xTaskCreate(LED_Seg7Menu_Task, "LED_Seg7Menu", 4096, NULL, ESP_TASK_PRIO_MIN + 1, NULL);//????
    
/*   释放BT mode模式，释放内存   */
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    if((sleep_keep & sleep_keep_WIFI_AP_OR_STA_BIT) == sleep_keep_WIFI_AP_OR_STA_BIT)
    {
        vTaskDelay(8000 / portTICK_PERIOD_MS);
        xMessageBufferSend(ir_rx_data,sleep_ir_data,13,portMAX_DELAY);//
		vTaskDelay(8000 / portTICK_PERIOD_MS);
        staBits = xEventGroupWaitBits(APP_event_group,APP_event_run_BIT,\
                                        pdFALSE,pdFALSE,10000 / portTICK_PERIOD_MS);
        if((staBits & APP_event_run_BIT ) == APP_event_run_BIT)
        {                                        
            xMessageBufferSend(ir_tx_data,sleep_ir_data,13,portMAX_DELAY);//
        }
    }


        // Open
    //printf("\n");
    //printf("Opening Non-Volatile Storage (NVS) handle... ");
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &BLe_battery_handle);
    if (err != ESP_OK) 
    {
       //printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else 
    {
        //printf("Done\n");

        // Read
        //printf("Reading restart counter from NVS ... \n");
        
        err = nvs_get_i32(BLe_battery_handle, "BLe_battery", &BLe_battery);
        switch (err) 
        {
            case ESP_OK:
                //printf("Done\n");
                //printf("Restart counter = %d\n", BLe_battery);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                //printf("The value is not initialized yet!\n");
                break;
            default :
                //printf("Error (%s) reading!\n", esp_err_to_name(err));
                break;
        }
        // Close
        nvs_close(BLe_battery_handle);
    }

    xTaskCreate(MultiButton_poll_Task, "Button_poll_Task", 2048, NULL, ESP_TASK_PRIO_MIN + 1, NULL);
    xTaskCreate(sntp_task, "sntp_task", 2048, NULL, ESP_TASK_PRIO_MIN + 1, NULL);
    xTaskCreate(ADC1_single_read_Task, "ADC1", 2048, NULL, ESP_TASK_PRIO_MIN + 1, NULL);

#ifdef  XL0801
    extern void ble_adv_scan_Task(void * arg);
    xTaskCreate(ble_adv_scan_Task, "adv_scan", 6144, NULL, ESP_TASK_PRIO_MIN + 1, NULL);

#endif

#ifdef  LYWSD03MMC
    staBits = xEventGroupWaitBits(APP_event_group,APP_event_run_BIT | APP_event_30min_timer_BIT,\
                                                pdFALSE,pdTRUE,portMAX_DELAY);
    if((staBits & (APP_event_run_BIT | APP_event_30min_timer_BIT)) == (APP_event_run_BIT | APP_event_30min_timer_BIT))
    {
        xTaskCreate(ble_init, "ble_init", 6144, NULL, ESP_TASK_PRIO_MIN + 1, NULL); 
    }
    //printf("Create ble_init Task.....\r\n");
#endif

}
