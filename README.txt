体感服程序分为两个部分，一是esp32c3组成的辅核，主要任务是对肢体配件进行的信息发送，通讯协议是espnow
		另一个是esp32组成的主核，主要任务是接收服务器数据并将消息进行分发，主要是进行上至服务器，下至辅核的通信分发，主要用到的是udp协议

主核用到的技术：iic，udp，串口
用到的库：Adafruit_PWMServoDriver

辅核用到的技术：espnow
用到的库：espnow

体感服是基于arduino进行开发的，主要用到esp32 和esp32c3两款芯片

