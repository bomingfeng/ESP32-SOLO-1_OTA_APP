set(COMPONENT_SRCS ./MultiButton/multi_button.c ./MultiButton/MultiButton_poll_Task.c cpu_timer.c BLE_Client.c temperature_control_task.c ir_tx_task.c ir_rx_task.c ds18b20_Task.c led_Task.c app_init.c MyWiFi.c OTAServer.c tcp_client.c main.c)
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES WebsiteFiles/index.html WebsiteFiles/favicon.ico WebsiteFiles/jquery-3.4.1.min.js)

register_component()
