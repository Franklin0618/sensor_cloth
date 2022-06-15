/******************************************************************
*
*
*
******************************************************************/

//ip的预编译处理
#define FIRST
//#define SECOND

//wifi宏定义
//#define TEST
#define PLAY
//#define COMPANY

//头文件
#include<WiFi.h>
#include<esp_now.h>
#include<Adafruit_PWMServoDriver.h>
#include<esp_task_wdt.h>

//宏定义
#define FRONT_RIGHT_UP      0x40    
#define FRONT_RIGHT_MID     0x41   
#define FRONT_RIGHT_DOWN    0x42    
#define FRONT_LEFT_UP       0x43    
#define FRONT_LEFT_MID      0x44    
#define FRONT_LEFT_DOWN     0x45    
#define BEHIND_LEFT_UP      0x46    
#define BEHIND_LEFT_MID     0x47    
#define BEHIND_LEFT_DOWN    0x48 
#define BEHIND_RIGHT_UP     0x49    
#define BEHIND_RIGHT_MID    0x4a    
#define BEHIND_RIGHT_DOWN   0x4b  

#define SERVOMIN 0
#define SERVOMAX 4095

//wifi配置
#ifdef TEST
const char *ssid_to_ap = "TP-LINK_6CC2";             //需要连接的wifi的名字
const char *password_to_ap = "987654321";            //需要连接的wifi的密码
#endif

#ifdef PLAY
const char *ssid_to_ap = "angel";                           //需要连接的wifi的名字
const char *password_to_ap = "1qaz2wsx3edc";                //需要连接的wifi的密码
#endif

#ifdef COMPANY
const char *ssid_to_ap = "wifi名字长才能穿透墙";        //需要连接的wifi的名字
const char *password_to_ap = "1q2w3e4r";                //需要连接的wifi的密码
#endif

//配置本地地址和端口号及目标ip地址
#ifdef FIRST
char* ip = "192.168.31.200";                                //目标服务器的ip地址（服务端）
uint16_t port = 50005;                                      //目标端口号（服务端）
IPAddress sta_ip(192,168,31,60);                           //本地IP
IPAddress sta_gateway(192,168,31,1);                        //本地网关
IPAddress sta_subnet(255,255,255,0);                        //本地子网掩码
unsigned int sta_localPort=50004;                           //本地端口
#endif

#ifdef SECOND
char* ip = "192.168.31.201";                                //目标服务器的ip地址（服务端）
uint16_t port = 50003;                                      //目标端口号（服务端）
IPAddress sta_ip(192,168,31,120);                           //本地IP
IPAddress sta_gateway(192,168,31,1);                        //本地网关
IPAddress sta_subnet(255,255,255,0);                        //本地子网掩码
unsigned int sta_localPort=50002;                           //本地端口
#endif



//全局变量
int loop_i=0;                                   //循环变量
byte Serial_inputbyte[13];                      //nfc串口接收缓冲区的数组和udp发送的缓冲区（背甲的发送只有nfc会用到）
char Serial_macStr[25];                         //串口接收的数据转为字符串的数组
byte buf_recv[3];                               //udp接收区
byte buf_send[3];                               //udp发送区
char macStr[18];                                //esp_now发送的回调函数进行打印用的字符串
byte udp_send_data_to_body[1]={1};              //espnow发送的缓冲区数组
esp_now_peer_info_t peerInfo[8];                //esp_now对象
int j=0;
int k=0;
char uart[1];
char cmd_recv0[8];
char cmd_recv1[8];
char cmd_recv2[8];                              
char cmd_recv[25];                              //二进制转字符串
WiFiUDP Udp_recv;                               //创建UDP接收对象
WiFiUDP Udp_send;                               //创建UDP发送对象

Adafruit_PWMServoDriver *pwm_front_right_up   = new Adafruit_PWMServoDriver( FRONT_RIGHT_UP   );   //创建pwm对象，12块pca9685板，创建12个对象
Adafruit_PWMServoDriver *pwm_front_right_mid  = new Adafruit_PWMServoDriver( FRONT_RIGHT_MID  );
Adafruit_PWMServoDriver *pwm_front_right_down = new Adafruit_PWMServoDriver( FRONT_RIGHT_DOWN );
Adafruit_PWMServoDriver *pwm_front_left_up    = new Adafruit_PWMServoDriver( FRONT_LEFT_UP    );
Adafruit_PWMServoDriver *pwm_front_left_mid   = new Adafruit_PWMServoDriver( FRONT_LEFT_MID   );
Adafruit_PWMServoDriver *pwm_front_left_down  = new Adafruit_PWMServoDriver( FRONT_LEFT_DOWN  );
Adafruit_PWMServoDriver *pwm_behind_right_up  = new Adafruit_PWMServoDriver( BEHIND_RIGHT_UP  );
Adafruit_PWMServoDriver *pwm_behind_right_mid = new Adafruit_PWMServoDriver( BEHIND_RIGHT_MID );
Adafruit_PWMServoDriver *pwm_behind_right_down= new Adafruit_PWMServoDriver( BEHIND_RIGHT_DOWN);
Adafruit_PWMServoDriver *pwm_behind_left_up   = new Adafruit_PWMServoDriver( BEHIND_LEFT_UP   );
Adafruit_PWMServoDriver *pwm_behind_left_mid  = new Adafruit_PWMServoDriver( BEHIND_LEFT_MID  );
Adafruit_PWMServoDriver *pwm_behind_left_down = new Adafruit_PWMServoDriver( BEHIND_LEFT_DOWN );


Adafruit_PWMServoDriver*pwm[12];

//初始化pca9685
void init_pca9685(){
    
    pwm[0]=pwm_front_right_up;
    pwm[1]=pwm_front_right_mid;
    pwm[2]=pwm_front_right_down;
    pwm[3]=pwm_front_left_up;
    pwm[4]=pwm_front_left_mid;
    pwm[5]=pwm_front_left_down;
    pwm[6]=pwm_behind_right_up;
    pwm[7]=pwm_behind_right_mid;
    pwm[8]=pwm_behind_right_down;
    pwm[9]=pwm_behind_left_up;
    pwm[10]=pwm_behind_left_mid;
    pwm[11]=pwm_behind_left_down;
    for(int time=0;time<=11;time++){
        pwm[time]->begin();
    }
    for(int time=0;time<=11;time++){
        pwm[time]->setPWMFreq(50);
    }
    for(int time=0;time<=11;time++){
        for(int servo;servo<=15;servo++){
            pwm[time]->setPWM(servo,0,SERVOMIN);
        }
    }

}

//初始化数组
void init_parament(){
    memset(Serial_inputbyte,0,13);
    memset(Serial_macStr,0,25);
    memset(buf_recv,0,3);
    memset(macStr,0,18); 
    memset(cmd_recv,0,25); 
    memset(buf_send,0,3);
}

//上电之后与服务器进行第一次通讯
void send_msg_to_ip(){
    Udp_send.beginPacket(ip,port);  //准备发送数据
    bitWrite(buf_send[0],7,1);
    Udp_send.write((const uint8_t*)buf_send, 3);               //复制数据到buff
    Udp_send.endPacket();           //发送数据
}

//16进制转字符串函数
void snprintf_buf(){
    snprintf(Serial_macStr, sizeof(Serial_macStr), "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
           Serial_inputbyte[0],Serial_inputbyte[1],Serial_inputbyte[2],Serial_inputbyte[3],Serial_inputbyte[4],Serial_inputbyte[5],
           Serial_inputbyte[6],Serial_inputbyte[7],Serial_inputbyte[8],Serial_inputbyte[9],Serial_inputbyte[10],Serial_inputbyte[11]);
}

//字节转换成字符串函数
void snprintf_byte_binary_to_string(){
    itoa( buf_recv[0], cmd_recv0, 2 );
    itoa( buf_recv[1], cmd_recv1, 2 );
    itoa( buf_recv[2], cmd_recv2, 2 );
    snprintf(cmd_recv, sizeof(cmd_recv), "%08s%08s%08s",cmd_recv0,cmd_recv1,cmd_recv2);
    memset(cmd_recv0,0,sizeof(cmd_recv0));
    memset(cmd_recv1,0,sizeof(cmd_recv1));
    memset(cmd_recv2,0,sizeof(cmd_recv2));
    Serial.println(cmd_recv);
}

//一个模块振动的函数
void one_shake_on(int block){
    for(int servo=0;servo<=15;servo++){
        pwm[block]->setPWM(servo,0,SERVOMIN);
    }
}

//重置所有震动
void all_off_shake(){
    for(int time=0;time<=11;time++){
        for(int servo;servo<=15;servo++){
            pwm[time]->setPWM(servo,0,SERVOMIN);
        }
    }
}
//循环遍历字符串并发送命令给执行单位
void loop_check_string(){
    for(int i=0;i<=23;i++){
        if(cmd_recv[i]=='1'){
            if(i<=11){
                if(i<=3){
//                    if(i==0){
//                        all_on_shake();
//                        for(int l=0;i<=7;i++){
//                            itoa(l,uart,10);
//                            Serial.println(uart[0]);
//                            Serial2.println(uart[0]);
//                        }
//                        break;
//                    }
                }else if(i>=4&&i<=11){
                    j=i-4;
                    itoa(j,uart,10);
                    Serial.println(uart[0]);
                    Serial2.println(uart[0]);
                    
                }
            }else if(i>=12&&i<=23){
                k=i-12;
                Serial.println(k);
                one_shake_on(k);
                //one_shake(k,100);
                Serial.println("iic work!");
            }
        }
    }
    delay(100);
    all_off_shake();
    j=0;
    memset(uart,0,1);
}

//初始化网络
void init_net(){
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    
    WiFi.config(sta_ip,sta_gateway,sta_subnet);             //设置本地网络参数
    WiFi.begin(ssid_to_ap,password_to_ap);                  //连接到指定路由
    
    while(WiFi.status()!=WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    digitalWrite(GPIO_NUM_2,HIGH);
    Udp_recv.begin(sta_localPort);                           //建立UDP Server ，监听指定端口
    Serial.print("staip:");
    Serial.print(WiFi.localIP());
}

//主程序

void setup(){
    pinMode(GPIO_NUM_2,OUTPUT);
    digitalWrite(GPIO_NUM_2,LOW);
    init_parament();
    Serial.begin(9600);                         //rx:3    tx:1
    Serial2.begin(115200);                      //rx:16   tx:17
    init_pca9685();
    init_net();
    send_msg_to_ip();
    esp_task_wdt_init(60,0);
    esp_task_wdt_add(NULL);
}

void loop(){
    esp_task_wdt_reset();
//    if(Serial.available()>0){  
//        Serial.readBytes(Serial_inputbyte,sizeof(Serial_inputbyte)); 
//        Udp_send.beginPacket(ip,port);                                      //初始化发送目标
//        Serial.print("send to");Serial.print(ip);Serial.print(":");
//        Serial.print(port);Serial.print("————>");
//        /*将数据(16进制的数)写入缓冲区，这里本来是要做一个buf_send数组的，
//        然后把inputbyte的内容赋值给buf_send但是考虑到没有必要，和内存管理还有数据格式转换的问题所以没有做*/
//        Udp_send.write((const uint8_t*)Serial_inputbyte,sizeof(Serial_inputbyte));     
//        Udp_send.endPacket();                                               //发送数据
//        memset(Serial_inputbyte,0,sizeof(Serial_inputbyte));
//        memset(Serial_macStr,0,25);
//    }
    //接收数据
    int packetSize = Udp_recv.parsePacket();                                //获取当前队首数据包长度
    if (packetSize)                                                         //如果有数据可用
    {
        Udp_recv.read(buf_recv, packetSize);                                //读取当前包数据
        Serial.println();
        Serial.print("Received: ");
        Serial.print(buf_recv[0]);
        Serial.print(buf_recv[1]);
        Serial.println(buf_recv[2]);
        Serial.print("From IP: ");
        Serial.println(Udp_recv.remoteIP());
        Serial.print("From Port: ");
        Serial.println(Udp_recv.remotePort());
        snprintf_byte_binary_to_string();
        loop_check_string();
        Udp_send.beginPacket(ip,port);    //准备发送数据
        bitWrite(buf_send[0], 7, 1);
        Udp_send.write((const uint8_t*)buf_send, 3);               //复制数据到发送缓存
        Udp_send.endPacket();                                               //发送数据
        memset(buf_recv,0,3);
        memset(buf_send,0,3);
        memset(cmd_recv,0,25); 
    }
}
