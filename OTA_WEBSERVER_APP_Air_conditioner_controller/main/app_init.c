#include "app_init.h"
#include <sys/param.h>
#include "esp_ota_ops.h"



void app_init(void)
{
    uint8_t i,ii;
    const esp_partition_t *partition = NULL;

    gpio_reset_pin(22);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(22, GPIO_MODE_OUTPUT);
    gpio_set_level(22,0);

    gpio_reset_pin(23);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(23, GPIO_MODE_OUTPUT);
    gpio_set_level(23,1);

    gpio_reset_pin(0);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(0, GPIO_MODE_INPUT);
    gpio_set_level(0,0);
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
                    ii = 0xaa;
                    printf("esp_ota_get_next_update_partition\r\n");
                    partition = esp_ota_get_next_update_partition(NULL);
                }
                vTaskDelay(1950 / portTICK_PERIOD_MS);
                if(gpio_get_level(CONFIG_INPUT_GPIO) == 0x00)
                {
                    gpio_set_level(23,0);
                    ii = 0xaa;
                    printf("ESP_PARTITION_SUBTYPE_APP_FACTORY\r\n");
                    partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP,    \
				    ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
                }
                if(ii == 0xaa)
                {
                    printf("restart to boot,%d\r\n",gpio_get_level(CONFIG_INPUT_GPIO));
                    esp_ota_set_boot_partition(partition);
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                    esp_restart();
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

    WRITE_PERI_REG(GPIO_FUNC12_OUT_SEL_CFG_REG, READ_PERI_REG(GPIO_FUNC12_OUT_SEL_CFG_REG) | GPIO_FUNC12_OUT_SEL_M);
    gpio_reset_pin(12);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(12, GPIO_MODE_OUTPUT);
    gpio_set_level(12,0);
    gpio_set_pull_mode(12,GPIO_PULLUP_ONLY);

    //Initialize NVS
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

    
}