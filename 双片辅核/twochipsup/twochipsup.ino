#include<esp_now.h>
#include <WiFi.h>

// rxPin = 20;
// txPin = 21;
int i;
byte Data[1]={1};
byte buf_recv[6];
esp_now_peer_info_t peerInfo[8];
char read_num[2];
/////////// 接收设备的 MAC 地址（esp_now）///////////////////////////////////////////////
uint8_t broadcastAddress[8][6] = {{0x7C, 0xDF, 0xA1, 0xB0, 0xBD, 0x01},                //右小腿
                                  {0x7C, 0xDF, 0xA1, 0xB0, 0xBD, 0x01},                //右大腿
                                  {0x7C, 0xDF, 0xA1, 0xBA, 0x21, 0x5D},                 //左小腿
                                  {0x7C, 0xDF, 0xA1, 0xBA, 0x21, 0x5D},                 //左大腿
                                  {0x7C, 0xDF, 0xA1, 0xBA, 0x2D, 0x79},                 //右小臂
                                  {0x7C, 0xDF, 0xA1, 0xBA, 0x2D, 0x79},                 //右大臂
                                  {0x7C, 0xDF, 0xA1, 0xB9, 0xCC, 0x99},                 //左小臂
                                  {0x7C, 0xDF, 0xA1, 0xB9, 0xCC, 0x99}};                 //左大臂

// 数据发送回调函数
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  Serial.print("Send status: ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println();
}
///////////////////////////////////////////////判断绑定是否成功/////////////////////////////////////
void judge_bind(){
    for (int i = 0; i < 8; i++)
    {
        Serial.print(i);Serial.print(":");
        if (esp_now_add_peer(&peerInfo[i]) != ESP_OK) {
            Serial.println("Failed to add peer!");
        }else{
            Serial.println("add peer success!");
        }
    }
}

// 绑定数据接收端
void bind_send(){
    for(int i=0;i<=7;i++){
        memcpy(peerInfo[i].peer_addr, broadcastAddress[i], 6);
        peerInfo[i].channel = 1;
        peerInfo[i].encrypt = false;
    }
}

/////////////////////////////////循环打印///////////////////////////////////////////////////
void loop_print()
{
    Serial.println("MAC:");
    for(int i=0;i<=5;i++){
        Serial.printf("%02X",int(broadcastAddress[i]));
        if(i==5){
            Serial.println(" ");
        }
    }
}


//////////////////////////////判断哪个需要发送信息/////////////////////////////////////////////////////////
void judgement(){
    while(Serial.available()>0){
        read_num[i]=Serial.read();
        i++;
    }
    i=0;
    Serial.println(read_num);
    if(read_num[0]=='0'){
        Serial.print("send to:");
        esp_now_send(peerInfo[0].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='1'){
        Serial.print("send to:");
        esp_now_send(peerInfo[1].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='2'){
        Serial.print("send to:");
        esp_now_send(peerInfo[2].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='3'){
        Serial.print("send to:");
        esp_now_send(peerInfo[3].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='4'){
        Serial.print("send to:");
        esp_now_send(peerInfo[4].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='5'){
        Serial.print("send to:");
        esp_now_send(peerInfo[5].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='6'){
        Serial.print("send to:");
        esp_now_send(peerInfo[6].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    if(read_num[0]=='7'){
        Serial.print("send to:");
        esp_now_send(peerInfo[7].peer_addr, (uint8_t *) &Data, sizeof(Data));
    }
    
}
void setup() {
  Serial.begin(115200);

  // 初始化 ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    bind_send();
    judge_bind();
  // 设置发送数据回调函数
  esp_now_register_send_cb(OnDataSent);
  Serial.print("MAC:");Serial.println(WiFi.macAddress());
}

void loop() {
    if(Serial.available()){
        // 发送数据
        
        judgement();
        memset(read_num,0,1);
        Serial.flush();
    }
    
}
