#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static char* TAG= "MQTT info";
static void mqtt_event_handler (void* event_handler_arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
static esp_mqtt_client_handle_t client ;
int mqtt_send_data(char * topic, char * payload);
static void send_sensor_data (void *params);

void app_main(void)
{
//Initialising the flash memory and wifi connection
ESP_ERROR_CHECK(nvs_flash_init());
wifi_connect_init();
//Input the respective ssid and pasword wifi 
wifi_connect_sta("", "", 10000);

//Input the uri of the MQTT broker eclipse or mosquito: with port number as specified 
esp_mqtt_client_config_t esp_mqtt_client_config = {
    .broker.address.uri = "mqtt://:1883"
};

client = esp_mqtt_client_init(&esp_mqtt_client_config);

//Register MQTT events : ESP_EVENT_ANY_ID to grab all the events in huzzah32
esp_mqtt_client_register_event(client,ESP_EVENT_ANY_ID,mqtt_event_handler,NULL);

//To start MQTT communication 
esp_mqtt_client_start(client);
}


//MQTT Event handler 
static void mqtt_event_handler (void* event_handler_arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    //Handler to recieve the event data 
    esp_mqtt_event_handle_t event_received = event_data;
  
  //Casting the event id to ESP type to andle the various events.
  switch((esp_mqtt_event_id_t)event_id)
 {
   case MQTT_EVENT_CONNECTED:
       ESP_LOGI(TAG,"MQTT_EVENT_CONNECTED");
     //  esp_mqtt_client_subscribe(client,"Temperature/#",1);
     //  esp_mqtt_client_subscribe(client,"Humidity/+/Percepetation",0);
   break;
   case MQTT_EVENT_DISCONNECTED:
       ESP_LOGI(TAG,"MQTT_EVENT_DISCONNECTED");
   break;
   case MQTT_EVENT_SUBSCRIBED:
       ESP_LOGI(TAG,"MQTT_EVENT_SUBSCRIBED");
   break;
   case MQTT_EVENT_UNSUBSCRIBED:
       ESP_LOGI(TAG,"MQTT_EVENT_UNSUBSCRIBED");
   break;
   case MQTT_EVENT_PUBLISHED:
       ESP_LOGI(TAG,"MQTT_EVENT_PUBLISHED");
   break;
   case MQTT_EVENT_DATA   :
       ESP_LOGI(TAG,"MQTT_EVENT_DATA   ");
     //  printf("Event : %.*s \n",event_received->topic_len,event_received->topic);
     //  printf("Data : %.*s \n",event_received->data_len,event_received->data);
   break;
   case MQTT_EVENT_BEFORE_CONNECT:
       ESP_LOGI(TAG,"MQTT_EVENT_BEFORE_CONNECT");
   break;
   case MQTT_EVENT_DELETED:
        ESP_LOGI(TAG,"MQTT_EVENT_DELETED");
   break;
   case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG,"ERROR %s", strerror(event_received->error_handle->esp_transport_sock_errno));
    break;
   default:
   break;
 }

 int mqtt_send_data(char * topic, char * payload)
{
  return esp_mqtt_client_publish(client,topic,payload,strlen(payload),1,0);
}

static void send_sensor_data (void *params)
{
 int sensor_value = 0;
 char message[25];
while(true)
{
    sprintf(message, "Sensor : %d", sensor_value);
    mqtt_send_data("sensor pot_value", message);
}

 vTaskDelay(pdMS_TO_TICKS(10000));
}
}