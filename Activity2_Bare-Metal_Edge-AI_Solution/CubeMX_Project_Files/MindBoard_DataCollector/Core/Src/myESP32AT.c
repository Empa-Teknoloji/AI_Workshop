/* Includes ------------------------------------------------------------------*/
#include "myESP32AT.h"
#include "common.h"

/* Private variables ---------------------------------------------------------*/
extern char mqttPacketBuffer[MQTT_DATA_PACKET_BUFF_SIZE];
extern MQTT_Config mqttConfig;
char mqtt_osc_ssid[32];
char mqtt_osc_password[32];
volatile FlagStatus flag_mqtt_error = RESET;
volatile FlagStatus flag_mqtt_init_done = RESET;
HAL_StatusTypeDef checkRcv;
WIFI_RespMsgTypeDef checkResp;
uint8_t mqtt_receive_data_temp[50];
static MQTT_SubInitTypeDef MQTTSubInitCase = MQTT_SUB_INIT;
MQTT_ErrorDataTypeDef mqttErrorData;

volatile uint8_t capturedPacketDataSize = 0;

int cnt_receive_it = 0;
int cnt_parse = 0;
static MQTT_CallbackParserTypeDef callbackParserState = MQTT_CB_SYNC_START1;
//static uint8_t rcvPacketSize = 0;
extern char mqttDataBuffer[1];
uint8_t index_databuffer = 0;


/* Responses and Commands */
char *RESP_OK                   = "OK\r\n";
char *RESP_READY                = "ready\r\n";
char *RESP_WIFIDISCONNECT       = "\r\nWIFI DISCONNECT\r\n";
char *RESP_WIFICONNECT          = "WIFI CONNECTED\r\nWIFI GOT IP\r\n";
char *RESP_SEND_OK              = "SEND OK\r\n";
char *RESP_ERROR                = "ERROR\r\n";
char *RESP_CIPSEND              = "OK\r\n";
char *RESP_PORTCONNECT          = "CONNECT\r\n";
char *RESP_WIFICONNECTED        = "WIFI CONNECTED\r\n";
char *RESP_RESET_READY          = "OK\r\nWIFI DISCONNECT\r\n\r\nready\r\n";
char *RESP_MAC_ID               = "+CIPSTAMAC:\"XX:XX:XX:XX:XX:XX\"\r\nOK\r\n";
char *RESP_BROKER_ADDRESS       = MQTT_BROKER_ADDRESS;
char *MQTT_SUBRECV              = "+MQTTSUBRECV";
char *RESP_BUSY                 = "busy p...\r\n";
char *RESP_PUB_RAW_OK           = "+MQTTPUB:OK";
char *RESP_PUB_RAW_FAIL         = "+MQTTPUB:FAIL";
char *MQTTDISCONNECTED			= "MQTTDISCONNECTED";
char *MQTTCONNECTED				= "MQTTCONNECTED";
char *RESP_FAIL                 = "FAIL";


/* External variables --------------------------------------------------------*/

extern volatile uint8_t mqtt_timer;
extern volatile uint8_t mqtt_timer_en;
volatile FlagStatus flag_mqtt_rx_done;
volatile uint8_t flag_mqtt_connect = RESET;
volatile FlagStatus flag_mqtt_rx_done;


MQTT_InitTypeDef MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;

FUNC_InitTypeDef MQTT_Init(MQTT_Config *config){

  uint8_t mqttConnTryCount = 0;
  MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;
  while(MQTTInitCase != MQTT_INIT_STATE_END_CASE){
    FUNC_StatusTypeDef checkFunc = FUNC_OK;

    switch (MQTTInitCase){
    case MQTT_INIT_STATE_WIFI_RESET:

      mqtt_timer_en = SET;
      flag_mqtt_error = RESET;

      /* ----------------------------------------- */
      checkFunc = Wifi_Reset2((char*)config->mqttPacketBuffer, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        MQTTInitCase = MQTT_INIT_STATE_WIFI_MODE;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_RESET;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_WIFI_MODE:

      checkFunc = Wifi_SetWifiMode2((char*)config->mqttPacketBuffer, STATION_MODE, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SMARTCONFIG;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_MODE;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_MODE;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_MODE;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_MODE;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_WIFI_SMARTCONFIG:

      if (config->OSC_enable == 1)
      {
        if (HAL_TIM_Base_Start_IT(TMR_ONESECOND) != HAL_OK)
        {
          Error_Handler();
        }
        checkFunc = Wifi_StartSmartConfig2((char*)config->mqttPacketBuffer, POLLING_MODE);

        if(checkFunc == FUNC_OK)
        {
          mqtt_timer = 0;

          while(mqtt_timer < 250 && MQTTInitCase != MQTT_INIT_STATE_WIFI_SET_TIME)
          {
            Wifi_Receive((char*)config->mqttPacketBuffer, 200, 5000, POLLING_MODE);

            if(Wifi_CheckResponse((char*)config->mqttPacketBuffer, "Smart get wifi info") == RESP_MSG_OK)
            {
              mqtt_timer = 0;
              parse_wifi_info((char*)config->mqttPacketBuffer, mqtt_osc_ssid, mqtt_osc_password);
            }
            else if(Wifi_CheckResponse((char*)config->mqttPacketBuffer, "smartconfig connected wifi") == RESP_MSG_OK)
            {
              mqtt_timer = 0;

              HAL_Delay(1000);

              while(MQTTInitCase == MQTT_INIT_STATE_WIFI_SMARTCONFIG)
              {
                checkFunc = Wifi_StopSmartConfig2((char*)config->mqttPacketBuffer, POLLING_MODE);

                if(checkFunc == FUNC_OK)
                {
                  mqtt_timer = 0;
                  MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_TIME;
                  break;
                }
                else if(checkFunc == FUNC_TX_ERROR)
                {
                  mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;            // Fire an error message on OLED and save log error
                  MQTTInitCase = MQTT_INIT_STATE_WIFI_SMARTCONFIG;
                }
                else if(checkFunc == FUNC_RX_ERROR)
                {
                  mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;            // Fire an error message on OLED and save log error
                  MQTTInitCase = MQTT_INIT_STATE_WIFI_SMARTCONFIG;
                }
                else if(checkFunc == FUNC_TIMEOUT)
                {
                  mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;       // Fire an error message on OLED and save log error
                  MQTTInitCase = MQTT_INIT_STATE_WIFI_SMARTCONFIG;
                }
              }
            }
          }
          HAL_TIM_Base_Stop_IT(TMR_ONESECOND);
          if(MQTTInitCase != MQTT_INIT_STATE_WIFI_SET_TIME)
          {
            MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
          }
        }
        else if(checkFunc == FUNC_TX_ERROR)
        {
          mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;            // Fire an error message on OLED and save log error
          MQTTInitCase= MQTT_INIT_STATE_WIFI_SMARTCONFIG;
        }
        else if(checkFunc == FUNC_RX_ERROR)
        {
          mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;            // Fire an error message on OLED and save log error
          MQTTInitCase= MQTT_INIT_STATE_WIFI_SMARTCONFIG;
        }
        else if(checkFunc == FUNC_TIMEOUT)
        {
          mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;       // Fire an error message on OLED and save log error
          MQTTInitCase = MQTT_INIT_STATE_WIFI_SMARTCONFIG;
        }
      }
      else
      {
        MQTTInitCase = MQTT_INIT_STATE_WIFI_DISC_AP;
      }

      if(mqtt_timer > 65)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_SMARTCONFIG;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_WIFI_DISC_AP:

      checkFunc = Wifi_QAP2((char*)config->mqttPacketBuffer, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_AP;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_DISC_AP;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_DISC_AP;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_DISC_AP;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_DISC_AP;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_WIFI_SET_AP:

      checkFunc = Wifi_SetAP2((char*)config->mqttPacketBuffer, config->wifiID, config->wifiPassword, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;

        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_TIME;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_AP;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_AP;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_AP;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_SET_AP;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_WIFI_SET_TIME:

      checkFunc = Wifi_SetTime2((char*)config->mqttPacketBuffer, config->timezone, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        MQTTInitCase = MQTT_INIT_STATE_MQTT_USER_CONFIG;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_TIME;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_TIME;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_WIFI_SET_TIME;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_WIFI_SET_TIME;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_MQTT_USER_CONFIG:

      checkFunc = Wifi_MqttUserConfig2((char*)config->mqttPacketBuffer, config->mode_mqtt, config->clientID,
    		  config->username, config->mqttPassword, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        flag_mqtt_init_done = SET;
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_USER_CONFIG;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_USER_CONFIG;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        MQTTInitCase = MQTT_INIT_STATE_MQTT_USER_CONFIG;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_MQTT_USER_CONFIG;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_MQTT_CONN_CONFIG:

      checkFunc = Wifi_MqttConnConfig2((char*)config->mqttPacketBuffer, config->keepAlive,
    		  config->cleanSession, config->qos, config->retain, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        mqtt_timer = 0;
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN;
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN_CONFIG;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN_CONFIG;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN_CONFIG;
      }

      if(mqtt_timer > 20)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_MQTT_CONN_CONFIG;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }

      break;

    case MQTT_INIT_STATE_MQTT_CONN:

      checkFunc = Wifi_MqttConn2((char*)config->mqttPacketBuffer, config->brokerAddress,config->reconnect, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        while (MQTTInitCase == MQTT_INIT_STATE_MQTT_CONN)
        {
          checkFunc = Wifi_GetMqttConn2((char*)config->mqttPacketBuffer, POLLING_MODE);

          if(checkFunc == FUNC_OK && config->mqttPacketBuffer[26] == '4')
          {
            mqtt_timer = 0;
            mqtt_timer_en = RESET;
            flag_mqtt_connect = SET;
            flag_mqtt_init_done = SET;
            MQTTInitCase = MQTT_INIT_STATE_END_CASE;
            MQTTSubInitCase = MQTT_SUB_INIT;
            return FUNC_SUCCESSFUL;
          }
          else if(mqtt_timer > 200)
          {
            mqtt_timer = 0;
            MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
          }
        }
      }
      else if(checkFunc == FUNC_TX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_RX;              // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
        mqttErrorData.errorCode = MQTT_INIT_ERROR_FUNC_TIMEOUT;         // Fire an error message on OLED and save log error
        MQTTInitCase = MQTT_INIT_STATE_MQTT_CONN;
      }

      if(mqtt_timer > 100)
      {
        mqtt_timer = 0;
        mqttErrorData.errorCode = MQTT_INIT_ERROR_MQTT_CONN_FAIL;
        MQTTInitCase = MQTT_INIT_STATE_TIMEOUT;
      }
      //HAL_UART_Receive_IT(&UART_WIFI , (uint8_t*)mqttPacketBuffer, 1);

      break;

    case MQTT_INIT_STATE_TIMEOUT:
      while(mqttConnTryCount < MQTT_CONN_MAX_TRY)
      {
        mqttConnTryCount++;
        MQTTInitCase = MQTT_INIT_STATE_WIFI_RESET;
        break;
      }

      if(mqttConnTryCount == MQTT_CONN_MAX_TRY)
      {
        mqtt_timer_en = RESET;
        flag_mqtt_error = FUNC_ERROR;

        mqttConnTryCount = 0;
        MQTTInitCase = MQTT_INIT_STATE_END_CASE;
      }

      break;

    case MQTT_INIT_STATE_END_CASE:
      break;
    }
  }
  return flag_mqtt_error;
}


FUNC_StatusTypeDef Wifi_Reset2(char *buffer, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+RST\r\n");

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, 4000, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
      {
    	  Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, 4000, mode);
    	  return FUNC_OK;
      }

      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}


FUNC_StatusTypeDef Wifi_SetWifiMode2(char *buffer, WIFI_ModeTypeDef mode, MQTT_DataRecvModeTypeDef recvMode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[14] = {0};

  sprintf(cmd, "AT+CWMODE=%d\r\n", mode);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, WIFI_FUNCS_STD_TIMEOUT, recvMode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}


FUNC_StatusTypeDef Wifi_StartSmartConfig2(char *buffer, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+CWSTARTSMART\r\n");

  if(checkCmd == HAL_OK)
  {

    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}


HAL_StatusTypeDef Wifi_Receive(char * buffer, uint16_t len, uint16_t timeout, MQTT_DataRecvModeTypeDef mode)
{

  HAL_StatusTypeDef halStatusCheck;

  memset(mqttPacketBuffer, 0, sizeof(mqttPacketBuffer));
  while(UART_CheckResponse() == TX_BUSY);
  switch(mode)
  {
  case POLLING_MODE:
    halStatusCheck = HAL_UART_Receive(&UART_WIFI , (uint8_t*)buffer, len, timeout);
    break;
  case INTERRUPT_MODE:
    len = 1;
    halStatusCheck = HAL_UART_Receive_IT(&UART_WIFI , (uint8_t*)buffer, len);
    break;
  }

  return halStatusCheck;
}


WIFI_RespMsgTypeDef Wifi_CheckResponse(char *buffer, char *response)
{
  WIFI_RespMsgTypeDef checkResponse;

  if(strstr(buffer, response) != NULL)
    checkResponse = RESP_MSG_OK;
  else if(strstr(buffer, response) == NULL)
  {
    if(strstr(buffer, RESP_BUSY))
      checkResponse = RESP_MSG_BUSY;
    else if(strstr(buffer, RESP_ERROR))
      checkResponse = RESP_MSG_ERROR;
    else if(strstr(buffer, "+"))
      checkResponse = RESP_MSG_CMD;
    else if(strstr(buffer, RESP_FAIL))
      checkResponse = RESP_MSG_FAIL;
    else
      checkResponse = RESP_MSG_NONE;
  }

  return checkResponse;
}

void parse_wifi_info(char *buffer, char *ssid, char *password) {
  char *ssid_start = strstr(buffer, "ssid:") + 5;
  char *ssid_end = strstr(ssid_start, "\n");
  strncpy(ssid, ssid_start, ssid_end - ssid_start);
  ssid[ssid_end - ssid_start] = '\0';

  char *password_start = strstr(buffer, "password:") + 9;
  char *password_end = strstr(password_start, "\n");
  strncpy(password, password_start, password_end - password_start);
  password[password_end - password_start] = '\0';
}

FUNC_StatusTypeDef Wifi_StopSmartConfig2(char *buffer, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+CWSTOPSMART\r\n");

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return  FUNC_TX_ERROR;

  return FUNC_OK;
}

FUNC_StatusTypeDef Wifi_QAP2(char *buffer, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+CWQAP\r\n");

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 120, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}


FUNC_StatusTypeDef Wifi_SetAP2(char *buffer, char *ssid , char *password, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[50] = {0};

  sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 100, 6000, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = (WIFI_RespMsgTypeDef)(Wifi_CheckResponse(buffer, RESP_WIFICONNECT) | Wifi_CheckResponse(buffer, RESP_OK));

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}



FUNC_StatusTypeDef Wifi_SetTime2(char *buffer, uint8_t timezone, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[50] = {0};

  sprintf(cmd, "AT+CIPSNTPCFG=1,%d,\"pool.ntp.org\"\r\n", timezone);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}


FUNC_StatusTypeDef Wifi_MqttUserConfig2(char *buffer, MQTT_UserConfigSchemeTypeDef mode,
                                        char *clientID , char *username , char *password,
                                        MQTT_DataRecvModeTypeDef recvMode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[70] = {0};

  sprintf(cmd, "AT+MQTTUSERCFG=0,%d,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", mode, clientID, username, password);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 120, WIFI_FUNCS_STD_TIMEOUT, recvMode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}

FUNC_StatusTypeDef Wifi_MqttConnConfig2(char *buffer, uint16_t keepAlive , MQTT_CC_ClsTypeDef cleanSession,
                                        MQTT_CC_QosTypeDef qos, MQTT_CC_RtnTypeDef retain, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[70] = {0};

  sprintf(cmd, "AT+MQTTCONNCFG=0,%d,%d,\"lwtt\",\"lwtt\",%d,%d\r\n", keepAlive, cleanSession, qos, retain);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}

FUNC_StatusTypeDef Wifi_MqttConn2(char *buffer, char *brokerAddress , uint8_t reconnect, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[100] = {0};

  sprintf(cmd, "AT+MQTTCONN=0,\"%s\",8883,%d\r\n", brokerAddress, reconnect);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 200, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}

FUNC_StatusTypeDef Wifi_GetMqttConn2(char *buffer, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+MQTTCONN?\r\n");

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 199, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = (WIFI_RespMsgTypeDef)(Wifi_CheckResponse(buffer, mqttConfig.brokerAddress) | Wifi_CheckResponse(buffer, RESP_OK));

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}

HAL_StatusTypeDef Wifi_SendCommand(const char* cmd)
{
  HAL_StatusTypeDef checkStatus;
  //while(UART_CheckResponse() != TX_READY);

  checkStatus = HAL_UART_Transmit(&UART_WIFI, (uint8_t*) cmd, strlen (cmd), 300);

  return checkStatus;
}

UART_RespMsgTypeDef UART_CheckResponse()
{
  UART_RespMsgTypeDef checkUART;

  if(__HAL_UART_GET_FLAG(&UART_WIFI, UART_FLAG_TXE) && !__HAL_UART_GET_FLAG(&UART_WIFI, UART_FLAG_RXNE))
  {
      checkUART = TX_READY;
  }
  else if(!__HAL_UART_GET_FLAG(&UART_WIFI, UART_FLAG_TXE))
  {
      checkUART = TX_BUSY;
  }
  else if (__HAL_UART_GET_FLAG(&UART_WIFI, UART_FLAG_RXNE))
  {
      checkUART = RX_BUSY;
  }

  return checkUART;
}


FUNC_StatusTypeDef Wifi_MqttSub2(char *buffer, const char *topic , uint8_t qos, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[100] = {0};

  sprintf(cmd, "AT+MQTTSUB=0,\"%s\",%d\r\n", topic, qos);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, 200, WIFI_FUNCS_STD_TIMEOUT, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}



void Wifi_MqttPubInit(char *buffer, const char *topic, MQTT_MacIdTypeDef *deviceID, MQTT_FwVersionDataTypeDef *fwVersion,
                      MQTT_CC_QosTypeDef qos , MQTT_CC_RtnTypeDef retain)
{
static MQTT_PubInitTypeDef MQTTPubInitCase = MQTT_PUB_INIT;
  char data[100];
  uint8_t respSize = 0;
  sprintf(data, "START/MAC_ID=%.2X:%.2X:%.2X:%.2X:%.2X:%.2X/FW_VER=%d.%d.%d", deviceID->hexMacID[0], deviceID->hexMacID[1], deviceID->hexMacID[2],
                                                                              deviceID->hexMacID[3], deviceID->hexMacID[4], deviceID->hexMacID[5],
                                                                              fwVersion->major, fwVersion->minor, fwVersion->patch);
  respSize = strlen(topic) + strlen(data) + PUB_RESP_DATA_RMNG_CHAR_COUNT;

  while(MQTTPubInitCase != MQTT_PUB_INIT_STATE_END_CASE){
    FUNC_StatusTypeDef checkFunc = FUNC_OK;

    switch(MQTTPubInitCase)
    {
    case MQTT_PUB_INIT:
      checkFunc = Wifi_MqttPub2(buffer, topic, data, respSize, qos, retain, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        //Wifi_Receive(buffer, 1, WIFI_FUNCS_STD_TIMEOUT, INTERRUPT_MODE);
        MQTTPubInitCase = MQTT_PUB_INIT_STATE_END_CASE;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        MQTTPubInitCase = MQTT_PUB_INIT;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {

      }

      break;
    }
  }
}

FUNC_StatusTypeDef Wifi_MqttPub2(char *buffer, const char *topic,const char *data , uint8_t respSize, MQTT_CC_QosTypeDef qos,
                                 MQTT_CC_RtnTypeDef retain, MQTT_DataRecvModeTypeDef mode)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;
  char cmd[200] = {0};

  sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",%d,%d\r\n", topic, data, qos, retain);
  checkCmd = Wifi_SendCommand(cmd);

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, respSize, 1000, mode);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)

        return FUNC_OK;
      else if(checkResp == RESP_MSG_BUSY)
        return FUNC_BUSY;
      else if(checkResp == RESP_MSG_ERROR /*|| checkResp == RESP_MSG_CMD*/)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
  {
	  return FUNC_TX_ERROR;
  }


  return FUNC_OK;
}

HAL_StatusTypeDef Wifi_SendCommand2(const char* cmd)
{
  HAL_StatusTypeDef checkStatus;

  checkStatus = HAL_UART_Transmit(&UART_WIFI, (uint8_t*) cmd, strlen (cmd), 500);

  return checkStatus;
}

FUNC_StatusTypeDef Wifi_MqttPubRaw2( char *buffer, char *topic, uint16_t dataSize, char *data,
								   MQTT_CC_QosTypeDef qos, MQTT_CC_RtnTypeDef retain, MQTT_DataRecvModeTypeDef mode){
	MQTT_PubRawDataTypeDef PubRawDataCase = MQTT_PUB_RAW_AT_COMMAND_SEND;
	HAL_StatusTypeDef checkCmd;
	HAL_StatusTypeDef checkRcv;
	WIFI_RespMsgTypeDef checkResp;

	uint8_t atCommandTimeoutCounter = 0;
	uint8_t atCommandBusyCounter = 0;
	uint8_t rawDataBusyCounter = 0;
    uint8_t respSize = strlen(topic) + PUB_RESP_DATA_RMNG_CHAR_COUNT;
	char cmd[200] = {0};
	HAL_UART_AbortReceive_IT(&UART_WIFI);

	while(PubRawDataCase != MQTT_PUB_RAW_END_CASE)
		{
			switch(PubRawDataCase)
			{
			case MQTT_PUB_RAW_AT_COMMAND_SEND:

				sprintf(cmd, "AT+MQTTPUBRAW=0,\"%s\",%d,%d,%d\r\n", topic, dataSize , qos, retain);
				checkCmd = Wifi_SendCommand2(cmd);

				PubRawDataCase = MQTT_PUB_RAW_AT_COMMAND_RECEIVE;

				break;

			case MQTT_PUB_RAW_AT_COMMAND_RECEIVE:

				if(checkCmd == HAL_OK)
				{
					checkRcv = Wifi_Receive(buffer, respSize, 500, mode);

					if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
					{
						checkResp = Wifi_CheckResponse(buffer, RESP_OK);

						if(checkResp == RESP_MSG_OK)
							PubRawDataCase = MQTT_PUB_RAW_DATA_SEND;

						else if(checkResp == RESP_MSG_BUSY)
						{
							atCommandBusyCounter++;
							if(atCommandBusyCounter <= 3)
								PubRawDataCase = MQTT_PUB_RAW_AT_COMMAND_SEND;
							else
								PubRawDataCase = MQTT_PUB_RAW_DATA_SEND;
						}

						else if(checkResp == RESP_MSG_ERROR /*|| checkResp == RESP_MSG_CMD*/)
							PubRawDataCase = MQTT_PUB_RAW_DATA_SEND;


						else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
						{
							atCommandTimeoutCounter++;
							if(atCommandTimeoutCounter <= 10)
								PubRawDataCase = MQTT_PUB_RAW_AT_COMMAND_RECEIVE;
							else
								PubRawDataCase = MQTT_PUB_RAW_DATA_SEND;
						}

            else if(checkResp == RESP_MSG_FAIL){

              PubRawDataCase = MQTT_PUB_RAW_AT_COMMAND_SEND;
            }
					}
				}  else
				    return FUNC_TX_ERROR;

				break;

			case MQTT_PUB_RAW_DATA_SEND:
				Wifi_Receive(buffer, 300, 1000, mode);
				checkCmd = Wifi_SendCommand2(data);
				PubRawDataCase = MQTT_PUB_RAW_DATA_RECEIVE;

				break;

			case MQTT_PUB_RAW_DATA_RECEIVE:

				if(checkCmd == HAL_OK)
				{
					checkRcv = Wifi_Receive(buffer, 400, 1000, mode);

					if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
					{
						checkResp = Wifi_CheckResponse(buffer, RESP_PUB_RAW_OK);

						if(checkResp == RESP_MSG_OK)
							PubRawDataCase = MQTT_PUB_RAW_END_CASE;

						else if(checkResp == RESP_MSG_BUSY)
						{
							rawDataBusyCounter++;
							if(rawDataBusyCounter <= 3){
								PubRawDataCase = MQTT_PUB_RAW_DATA_RECEIVE;
							}else{
								PubRawDataCase = MQTT_PUB_RAW_END_CASE;
								return FUNC_BUSY;

							}
						}else if(checkResp == RESP_MSG_ERROR /*|| checkResp == RESP_MSG_CMD*/){
							PubRawDataCase = MQTT_PUB_RAW_END_CASE;
							return FUNC_RX_ERROR;

						}else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT){
							PubRawDataCase = MQTT_PUB_RAW_END_CASE;
							return FUNC_TIMEOUT;

						}
            else if(checkResp == RESP_MSG_FAIL){
              PubRawDataCase = MQTT_PUB_RAW_DATA_RECEIVE;

              return FUNC_FAIL;
            }
					}
				}  else
				    return FUNC_TX_ERROR;

				break;
			}
		}
	return FUNC_OK;
}


void UART_MqttCallbackPacketCapture(UART_HandleTypeDef *huart, const char *dataBuffer, char *packetBuffer)
{
  if (flag_mqtt_init_done == SET)
  {

    //static MQTT_CallbackParserTypeDef callbackParserState = MQTT_CB_SYNC_START1;
    static uint16_t rcvPacketSize = 0;
    //mqttpub_enable = SET;
    switch(callbackParserState)
    {
    case MQTT_CB_SYNC_START1:                                           // Capture start sync1
      if(dataBuffer[0] == '+')
      {
        memset(packetBuffer, NULL, MQTT_DATA_PACKET_BUFF_SIZE);
        packetBuffer[rcvPacketSize++] = dataBuffer[0];
        callbackParserState = MQTT_CB_SYNC_START2;
      }

      break;

    case MQTT_CB_SYNC_START2:                                           // Capture start sync2
      if(dataBuffer[0] == 'M')
      {
        packetBuffer[rcvPacketSize++] = dataBuffer[0];
        callbackParserState = MQTT_CB_PACKET_CAPTURE;
      }
      else
      {
        rcvPacketSize = 0;
        callbackParserState = MQTT_CB_SYNC_START1;
      }

      break;

    case MQTT_CB_PACKET_CAPTURE:                                        // Capture packet (all data)
      if(dataBuffer[0] == '\r')
      {
        packetBuffer[rcvPacketSize++] = dataBuffer[0];
        callbackParserState = MQTT_CB_SYNC_STOP;
      }
      else
      {
        packetBuffer[rcvPacketSize++] = dataBuffer[0];
      }

      break;

    case MQTT_CB_SYNC_STOP:                                             // Capture stop sync
      if(dataBuffer[0] == '\n')
      {
        packetBuffer[rcvPacketSize++] = dataBuffer[0];
        flag_mqtt_rx_done = SET;
        capturedPacketDataSize = rcvPacketSize;
        rcvPacketSize = 0;
        callbackParserState = MQTT_CB_SYNC_START1;
      }

      break;

    default:
      callbackParserState = MQTT_CB_SYNC_START1;

      break;
    }

    HAL_UART_Receive_IT(huart, (uint8_t*)dataBuffer, 1);
  }
}




FUNC_StatusTypeDef Wifi_MqttSubInit(char *buffer, const char *topic , uint8_t QoS)
{
  MQTTSubInitCase = MQTT_SUB_INIT;
  while(MQTTSubInitCase != MQTT_SUB_INIT_STATE_END_CASE){
    FUNC_StatusTypeDef checkFunc = FUNC_OK;

    switch(MQTTSubInitCase)
    {
    case MQTT_SUB_INIT:
      checkFunc = Wifi_MqttSub2(buffer, topic, QoS, POLLING_MODE);

      if(checkFunc == FUNC_OK)
      {
        MQTTSubInitCase = MQTT_SUB_INIT_STATE_END_CASE;
        return FUNC_OK;
      }
      else if(checkFunc == FUNC_RX_ERROR)
      {
        MQTTSubInitCase = MQTT_SUB_INIT;
        return FUNC_RX_ERROR;
      }
      else if(checkFunc == FUNC_TIMEOUT)
      {
    	  return FUNC_TIMEOUT;
      }

      break;
    }
  }
}

void UART_MqttPacketParser(MQTT_MsgDataTypeDef *messageData, const char *dataPacket, uint16_t dataSize)
{
  if(strstr(dataPacket, MQTT_SUBRECV) != NULL)
  {
    UART_MqttSubRecvParser(messageData, dataPacket, dataSize);
  }
  else if(strstr(dataPacket, MQTTCONNECTED) != NULL)
  {
    flag_mqtt_connect = SET;
    flag_mqtt_error = RESET;
  }
  else if(strstr(dataPacket, MQTTDISCONNECTED) != NULL)
  {
	  flag_mqtt_connect = RESET;
  }
}

void UART_MqttSubRecvParser(MQTT_MsgDataTypeDef *messageData, const char *dataBuffer, const uint16_t dataBufferSize)
{

  for(uint16_t i = 0; i < dataBufferSize; i++)
  {
    static MQTT_CallbackParserTypeDef subRecvParserState = MQTT_CB_SYNC_START1;
    static char dataLength[3] = {0};
    static uint8_t rcvDataSize = 0;

    switch(subRecvParserState)
    {
    case MQTT_CB_SYNC_START1:                                           // Capture start sync1
      if(dataBuffer[i] == '+')
      {
        memset(dataLength, 0, 3);
        subRecvParserState = MQTT_CB_SYNC_START2;
      }

      break;

    case MQTT_CB_SYNC_START2:                                           // Capture start sync2
      if(dataBuffer[i] == 'M')
      {
    	index_databuffer = i;
        subRecvParserState = MQTT_CB_SYNC_START3;
      }
      else
      {
        rcvDataSize = 0;
        subRecvParserState = MQTT_CB_SYNC_START1;
      }

      break;

    case MQTT_CB_SYNC_START3:                                           // Capture start sync2
      if(dataBuffer[index_databuffer + 4] == 'S')
      {

        subRecvParserState = MQTT_CB_TOPIC_CAPTURE_START;
      }
      else
      {
        rcvDataSize = 0;
        subRecvParserState = MQTT_CB_SYNC_START1;
      }

      break;

    case MQTT_CB_TOPIC_CAPTURE_START:                                   // Start of topic capture
      if(dataBuffer[i] == '"')
      {
        subRecvParserState = MQTT_CB_TOPIC_CAPTURE;
      }

      break;

    case MQTT_CB_TOPIC_CAPTURE:                                         // Capture topic
      if(dataBuffer[i] == '"')
      {
        rcvDataSize = 0;
        subRecvParserState = MQTT_CB_DATASIZE_CAPTURE_START;
      }
      else
      {
        messageData->topic_id[rcvDataSize++] = dataBuffer[i];
      }

      break;

    case MQTT_CB_DATASIZE_CAPTURE_START:                                // Start of data size capture
      if(dataBuffer[i] == ',')
      {
        subRecvParserState = MQTT_CB_DATASIZE_CAPTURE;
      }

      break;

    case MQTT_CB_DATASIZE_CAPTURE:                                      // Capture data size
      if(dataBuffer[i] == ',')
      {
        rcvDataSize = 0;
        charToInt((uint8_t*)dataLength, &messageData->data_length, 3);
        subRecvParserState = MQTT_CB_DATA_CAPTURE;
      }
      else
      {
        dataLength[rcvDataSize++] = dataBuffer[i];
      }

      break;

    case MQTT_CB_DATA_CAPTURE:                                          // Capture data
      if(dataBuffer[i] == '\r')
      {
        subRecvParserState = MQTT_CB_SYNC_STOP;
      }
      else
      {
        messageData->data[rcvDataSize++] = dataBuffer[i];
      }

      break;

    case MQTT_CB_SYNC_STOP:                                             // Capture stop sync
      if(dataBuffer[i] == '\n')
      {
      	index_databuffer = 0;
        rcvDataSize = 0;
        subRecvParserState = MQTT_CB_SYNC_START1;
      }

      break;

    default:
      subRecvParserState = MQTT_CB_SYNC_START1;

      break;
    }
  }
}

void charToInt(uint8_t *charArray, uint8_t *intNum, uint8_t length)
{
  uint8_t tempCharArray[3] = {0};
  uint8_t tempIntArray[3] = {0};

  for(uint8_t i = 0; i < length; i++)
  {
    tempCharArray[i] = charArray[i];

    if (tempCharArray[i] >= '0' && tempCharArray[i] <= '9') {
      tempIntArray[i] = tempCharArray[i] - '0';
    }
  }

  switch(length)
  {
  case 1:
    intNum[0] = tempIntArray[0];
    break;

  case 2:
    intNum[0] = 10 * tempIntArray[0] + tempIntArray[1];
    break;

  case 3:
    intNum[0] = 100 * tempIntArray[0] + 10 * tempIntArray[1] + tempIntArray[2];
    break;
  }
}




void getUniqueID3(char charUniqueID[], const char *buffer)
{
  FUNC_StatusTypeDef checkFunc = Wifi_GetMAC((char*)buffer);

  if(checkFunc == FUNC_OK)
  {
    charUniqueID[0] = toupper(buffer[27]);
    charUniqueID[1] = toupper(buffer[28]);
    charUniqueID[2] = toupper(buffer[29]);
    charUniqueID[3] = toupper(buffer[30]);
    charUniqueID[4] = toupper(buffer[31]);
    charUniqueID[5] = toupper(buffer[32]);
    charUniqueID[6] = toupper(buffer[33]);
    charUniqueID[7] = toupper(buffer[34]);
    charUniqueID[8] = toupper(buffer[35]);
    charUniqueID[9] = toupper(buffer[36]);
    charUniqueID[10] = toupper(buffer[37]);
    charUniqueID[11] = toupper(buffer[38]);
    charUniqueID[12] = toupper(buffer[39]);
    charUniqueID[13] = toupper(buffer[40]);
    charUniqueID[14] = toupper(buffer[41]);
    charUniqueID[15] = toupper(buffer[42]);
    charUniqueID[16] = toupper(buffer[43]);
  }
  else if(checkFunc == FUNC_TX_ERROR)
  {

  }
  else if(checkFunc == FUNC_RX_ERROR)
  {

  }
  else if(checkFunc == FUNC_TIMEOUT)
  {

  }
}

FUNC_StatusTypeDef Wifi_GetMAC(char *buffer)
{
  HAL_StatusTypeDef checkCmd;
  HAL_StatusTypeDef checkRcv;
  WIFI_RespMsgTypeDef checkResp;

  checkCmd = Wifi_SendCommand("AT+CIPSTAMAC?\r\n");

  if(checkCmd == HAL_OK)
  {
    checkRcv = Wifi_Receive(buffer, WIFI_FUNCS_STD_BUFF_SIZE, 200, POLLING_MODE);

    if(checkRcv == HAL_OK || checkRcv == HAL_TIMEOUT)
    {
      checkResp = Wifi_CheckResponse(buffer, RESP_OK);

      if(checkResp == RESP_MSG_OK)
        return FUNC_OK;
      else if(checkResp == RESP_MSG_ERROR || checkResp == RESP_MSG_CMD)
        return FUNC_RX_ERROR;
      else if(checkResp == RESP_MSG_NONE && checkRcv == HAL_TIMEOUT)
        return FUNC_TIMEOUT;
    }
    else
      return (FUNC_StatusTypeDef)checkRcv;
  }
  else
    return FUNC_TX_ERROR;

  return FUNC_OK;
}

void Wifi_WaitMqttData()
{
	 HAL_UART_Receive_IT(&UART_WIFI , (uint8_t*)mqttDataBuffer, 1);

}
void LED_MqttTXBlink()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, SET);
	HAL_Delay(300);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, RESET);
}
void LED_MqttRXBlink()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, SET);
	HAL_Delay(300);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, RESET);
}
void LED_Mqttconnected(FlagStatus set_led)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, set_led);
}




//void ESP32_Init()
//{
//	HAL_GPIO_WritePin(ESP32_EN_GPIO_Port, ESP32_EN_Pin, 1);
//
//}
