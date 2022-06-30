#include "app_init.h"
#include <sys/param.h>
#include "esp_ota_ops.h"

extern int32_t BLe_battery;
extern nvs_handle_t BLe_battery_handle;

void app_init(void)
{
    uint8_t i,ii;
    const esp_partition_t *partition = NULL;
    gpio_reset_pin(0);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(0, GPIO_MODE_INPUT);
    gpio_set_level(0,0);
    gpio_reset_pin(22);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(22, GPIO_MODE_OUTPUT);
    gpio_set_level(22,0);
    gpio_reset_pin(23);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(23, GPIO_MODE_OUTPUT);
    gpio_set_level(23,1);
    for(i = 0;i < 40;i++)
    {
        if(gpio_get_level(CONFIG_INPUT_GPIO) == 0x00)
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            if(gpio_get_level(CONFIG_INPUT_GPIO) == 0x00)
            {
                vTaskDelay(1950 / portTICK_PERIOD_MS);
                if(gpio_get_level(CONFIG_INPUT_GPIO) == 0x00)
                {
                    gpio_set_level(22,1);
                    partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP,    \
				    ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
                    if (partition == NULL) 
                    {
                        partition = esp_ota_get_next_update_partition(NULL);
                    }
                    printf("restart to boot,%d\r\n",gpio_get_level(CONFIG_INPUT_GPIO));
                    esp_ota_set_boot_partition(partition);
                }
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    gpio_set_level(23,0);

    /* IO 初始化 */
    WRITE_PERI_REG(GPIO_FUNC2_OUT_SEL_CFG_REG, READ_PERI_REG(GPIO_FUNC2_OUT_SEL_CFG_REG) | GPIO_FUNC2_OUT_SEL_M);
    gpio_reset_pin(2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_level(2,0);
    gpio_set_pull_mode(2,GPIO_PULLUP_ONLY);

    WRITE_PERI_REG(GPIO_FUNC15_OUT_SEL_CFG_REG, READ_PERI_REG(GPIO_FUNC15_OUT_SEL_CFG_REG) | GPIO_FUNC15_OUT_SEL_M);
    gpio_reset_pin(15);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(15, GPIO_MODE_OUTPUT);
    gpio_set_level(15,0);
    gpio_set_pull_mode(15,GPIO_PULLUP_ONLY);
    
    WRITE_PERI_REG(GPIO_FUNC13_OUT_SEL_CFG_REG, READ_PERI_REG(GPIO_FUNC13_OUT_SEL_CFG_REG) | GPIO_FUNC13_OUT_SEL_M);
    gpio_reset_pin(13);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(13, GPIO_MODE_OUTPUT);
    gpio_set_level(13,0);

    //Initialize NVS
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

        // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open("storage", NVS_READWRITE, &BLe_battery_handle);
    if (err != ESP_OK) 
    {
       printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else 
    {
        printf("Done\n");

        // Read
        printf("Reading restart counter from NVS ... \n");
        
        err = nvs_get_i32(BLe_battery_handle, "BLe_battery", &BLe_battery);
        switch (err) 
        {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %d\n", BLe_battery);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        // Close
        nvs_close(BLe_battery_handle);
    }
}