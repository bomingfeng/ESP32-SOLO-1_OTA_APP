void ds18b20_task(void){
    ds18b20_timer_init();
    printf("This is from ds18b20\r\n");
    float t = 0.0;
    while(1){
        t = ds18b20_read_single(8);//pin 8 pull up 10k
        printf("got temprsure %.2f\r\n",t);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
 
void app_main(void)
{
    ds18b20_task();
}
