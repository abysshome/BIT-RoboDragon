#include <Arduino.h>
#line 1 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
#define STOP 0  // 停止
#define RUN 1   // 前进
#define BACK 2  // 后退
#define LEFT 3  // 左转
#define RIGHT 4 // 右转
#define a1 2 // 左轮正传
#define a2 3 // 左轮反转
#define b1 4 // 右轮正转
#define b2 5 // 右轮反转
#define bluetooth Serial
#define L_SENSE 7
#define R_SENSE 8
#define L_SENSE1 6
#define R_SENSE1 9
#define TRIG1 A2     // 左边的超声波模块的触发引脚
#define ECHO1 A3     // 左边的超声波模块的回响引脚
#define TRIG2 A4     // 中间的超声波模块的触发引脚
#define ECHO2 A5     // 中间的超声波模块的回响引脚
#define TRIG3 A0     // 右边的超声波模块的触发引脚
#define ECHO3 A1     // 右边的超声波模块的回响引脚
#define THRESHOLD 20 // 定义避障的阈值（单位为厘米）
#define shoot 12

int L = 0;
int R = 0;
int L1 = 0;
int R1 = 0;

char data;

int leftPWM = 10;  // L298N使能端左
int rightPWM = 11; // L298N使能端右

#line 34 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
void Work(int c, int value);
#line 76 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
void setup();
#line 98 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
int bluetoothWork();
#line 139 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
float measureDistance(int trigPin, int echoPin);
#line 155 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
void hongwai();
#line 256 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
int chaosb();
#line 312 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
void loop();
#line 34 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
void Work(int c, int value){
    analogWrite(leftPWM, value * 1.1); // 设置PWM输出，设置速度  (左右是定义反了）
    analogWrite(rightPWM, value);
    switch (c)
    {
    case RUN:
        Serial.println("RUN"); // 前进输出状态
        digitalWrite(a1, HIGH);
        digitalWrite(a2, LOW);
        digitalWrite(b1, HIGH);
        digitalWrite(b2, LOW);
        break;
    case BACK:
        Serial.println("BACK"); // 后退输出状态
        digitalWrite(a1, LOW);
        digitalWrite(a2, HIGH);
        digitalWrite(b1, LOW);
        digitalWrite(b2, HIGH);
        break;
    case LEFT:
        Serial.println("LEFT"); // 左转输出状态
        digitalWrite(a1, LOW);
        digitalWrite(a2, LOW);
        digitalWrite(b1, HIGH);
        digitalWrite(b2, LOW);
        break;
    case RIGHT:
        Serial.println("RIGHT"); // 右转弯输出状态
        digitalWrite(a1, HIGH);
        digitalWrite(a2, LOW);
        digitalWrite(b1, LOW);
        digitalWrite(b2, LOW);
        break;
    default:
        Serial.println("STOP"); // 停止输出状态
        digitalWrite(a1, LOW);
        digitalWrite(a2, LOW);
        digitalWrite(b1, LOW);
        digitalWrite(b2, LOW);
    }
}

void setup(){
    // 设置电机引脚为输出模式
    pinMode(a1, OUTPUT);
    pinMode(a2, OUTPUT);
    pinMode(b1, OUTPUT);
    pinMode(b2, OUTPUT);
    pinMode(leftPWM, OUTPUT);
    pinMode(rightPWM, OUTPUT);
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
    pinMode(TRIG3, OUTPUT);
    pinMode(ECHO3, INPUT);
    pinMode(L_SENSE, INPUT);
    pinMode(R_SENSE, INPUT);
    pinMode(L_SENSE1, INPUT);
    pinMode(R_SENSE1, INPUT);
    pinMode(shoot, OUTPUT);
    bluetooth.begin(9600);
}

int bluetoothWork(){
    // 如果蓝牙有数据可读
    while (1)
    {
        if (bluetooth.available())
        {
            // 读取一个字节的数据并存储在data变量中
            data = bluetooth.read();

            // 根据不同的数据执行不同的动作
            switch (data)
            {
            case 'F': // 前进
                Work(RUN, 200);
                break;
            case 'B': // 后退
                Work(BACK, 150);
                break;
            case 'L': // 左转
                Work(LEFT, 150);
                break;
            case 'R': // 右转
                Work(RIGHT, 100);
                break;
            case 'S': // 停止
                Work(STOP, 0);
                break;
            case 'A':
                digitalWrite(shoot, HIGH);
                delay(2000);
                digitalWrite(shoot, LOW);
                break;
            default: // 其他情况，停止
                Work(STOP, 0);
                break;
            }
        }
    }
}

// 定义一个函数，用于测量超声波模块返回的距离（单位为厘米）
float measureDistance(int trigPin, int echoPin){
    // 发送一个10微秒的高电平信号到触发引脚
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);
    // 测量回响引脚返回的高电平信号的持续时间（单位为微秒）
    long duration = pulseIn(echoPin, HIGH);

    // 根据声速计算距离（单位为厘米）
    float distance = duration * 0.034 / 2;

    // 返回距离值
    return distance;
}

void hongwai(){
    int lastDir = 0;
    // 红外循迹
    while (1)
    {
        L = digitalRead(L_SENSE);
        R = digitalRead(R_SENSE);
        L1 = digitalRead(L_SENSE1);
        R1 = digitalRead(R_SENSE1);

        if (L1 == LOW && L == LOW && R == LOW && R1 == LOW)
        { // 直走
            if (lastDir == -2)
            {
                Work(LEFT, 110);
            }
            else if (lastDir == 2)
            {
                Work(RIGHT, 110);
            }else{
              Work(RUN,80);
              lastDir=0;
            }
        }
        else if ((L1 == LOW && L == LOW && R == LOW && R1 == HIGH) || (L1 == LOW && L == LOW && R == HIGH && R1 == HIGH) || (L1 == LOW && L == HIGH && R == HIGH && R1 == HIGH))
        { // 大幅左转
            Work(LEFT, 110);
            lastDir = -2;
        }
        else if (L1 == LOW && L == LOW && R == HIGH && R1 == LOW)
        { // 一般左转
            Work(LEFT, 110);
            lastDir = -1;
        }
        else if ((L1 == HIGH && L == LOW && R == LOW && R1 == LOW) || (L1 == HIGH && L == HIGH && R == LOW && R1 == LOW) || (L1 == HIGH && L == HIGH && R == HIGH && R1 == LOW))
        { // 大幅右转
            Work(RIGHT, 110);
            lastDir = 2;
        }
        else if (L1 == LOW && L == HIGH && R == LOW && R1 == LOW)
        { // 一般右转
            Work(RIGHT, 110);
            lastDir = 1;
        }
        else if ((L1 == LOW && L == HIGH && R == LOW && R1 == HIGH))
        {
            Work(STOP, 0);
            lastDir = 0;// 停止
        }
        else if (L1 == LOW && L == HIGH && R == HIGH && R1 == LOW)
        { // 直走
            Work(RUN, 80);
            lastDir = 0;
        }
        else if (L1 == HIGH && L == LOW && R == LOW && R1 == HIGH)
        {
            Work(STOP, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == HIGH && L == LOW && R == HIGH && R1 == LOW)
        {
            Work(STOP, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == HIGH && L == LOW && R == HIGH && R1 == HIGH)
        {
            Work(STOP, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == HIGH && L == HIGH && R == LOW && R1 == HIGH)
        {
            Work(STOP, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == HIGH && L == HIGH && R == HIGH && R1 == HIGH)
        {
            Work(STOP, 0);
            lastDir = 0; // 停止
        }
        else
        {
            lastDir = 0;
        }

        if (bluetooth.available())
        {
            char btCommand = bluetooth.read();
            if (btCommand == 'C')
            {
                Work(STOP, 0);
                return;
            }
        }

        delay(20);
        Work(STOP, 0);
        delay(20);
        Serial.println("红外循迹");
    }
}

int chaosb(){
    while (1)
    {
        // 测量三个超声波模块返回的距离
        float distance1 = measureDistance(TRIG1, ECHO1);
        float distance2 = measureDistance(TRIG2, ECHO2);
        float distance3 = measureDistance(TRIG3, ECHO3);
        Serial.println(distance1);
        Serial.println(distance2);
        Serial.println(distance3);

        // 特别处理distance1过小的情况
        if (distance2 < 10)
        {                    // 假设10cm是后退的临界距离
            Work(BACK, 100); // 后退
            delay(50);       // 给足够时间后退
            Work(STOP, 0);
            delay(50);
            // continue; // 跳过其余的逻辑，重新测量距离
        }

        // 基于中间和右边距离决定行动
        if (distance1 < 20 || distance3 < 20)
        { // 假设20cm是临界距离
            if (distance1 > distance3)
            {
                Work(RIGHT, 120); // 向左转
            }
            else
            {
                Work(LEFT, 120); // 向右转
            }
            delay(20); // 转向一段时间后再继续前进
            Work(STOP, 0);
            delay(20);
        }
        else
        {
            Work(RUN, 90); // 如果没有检测到障碍物，就直行
            delay(20);
        }
        Serial.println("超声波");

        // 检查蓝牙信号'C'退出功能
        if (bluetooth.available())
        {
            char btCommand = bluetooth.read();
            if (btCommand == 'C')
            {
                Work(STOP, 0);
                return 0; // 退出函数
            }
        }
    }
}

void loop(){
    hongwai();//红外
    chaosb();//超声波
    bluetoothWork();//蓝牙
}


