# 1 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
# 24 "c:\\Users\\WenbinXu\\Desktop\\code\\BIT-RoboDragon\\bth_v2\\bth_v2.ino"
int L = 0;
int R = 0;
int L1 = 0;
int R1 = 0;

char data;

int leftPWM = 10; // L298N使能端左
int rightPWM = 11; // L298N使能端右

void Work(int c, int value){
    analogWrite(leftPWM, value * 1.1); // 设置PWM输出，设置速度  (左右是定义反了）
    analogWrite(rightPWM, value);
    switch (c)
    {
    case 1 /* 前进*/:
        Serial.println("RUN"); // 前进输出状态
        digitalWrite(2 /* 左轮正传*/, 0x1);
        digitalWrite(3 /* 左轮反转*/, 0x0);
        digitalWrite(4 /* 右轮正转*/, 0x1);
        digitalWrite(5 /* 右轮反转*/, 0x0);
        break;
    case 2 /* 后退*/:
        Serial.println("BACK"); // 后退输出状态
        digitalWrite(2 /* 左轮正传*/, 0x0);
        digitalWrite(3 /* 左轮反转*/, 0x1);
        digitalWrite(4 /* 右轮正转*/, 0x0);
        digitalWrite(5 /* 右轮反转*/, 0x1);
        break;
    case 3 /* 左转*/:
        Serial.println("LEFT"); // 左转输出状态
        digitalWrite(2 /* 左轮正传*/, 0x0);
        digitalWrite(3 /* 左轮反转*/, 0x0);
        digitalWrite(4 /* 右轮正转*/, 0x1);
        digitalWrite(5 /* 右轮反转*/, 0x0);
        break;
    case 4 /* 右转*/:
        Serial.println("RIGHT"); // 右转弯输出状态
        digitalWrite(2 /* 左轮正传*/, 0x1);
        digitalWrite(3 /* 左轮反转*/, 0x0);
        digitalWrite(4 /* 右轮正转*/, 0x0);
        digitalWrite(5 /* 右轮反转*/, 0x0);
        break;
    default:
        Serial.println("STOP"); // 停止输出状态
        digitalWrite(2 /* 左轮正传*/, 0x0);
        digitalWrite(3 /* 左轮反转*/, 0x0);
        digitalWrite(4 /* 右轮正转*/, 0x0);
        digitalWrite(5 /* 右轮反转*/, 0x0);
    }
}

void setup(){
    // 设置电机引脚为输出模式
    pinMode(2 /* 左轮正传*/, 0x1);
    pinMode(3 /* 左轮反转*/, 0x1);
    pinMode(4 /* 右轮正转*/, 0x1);
    pinMode(5 /* 右轮反转*/, 0x1);
    pinMode(leftPWM, 0x1);
    pinMode(rightPWM, 0x1);
    pinMode(A2 /* 左边的超声波模块的触发引脚*/, 0x1);
    pinMode(A3 /* 左边的超声波模块的回响引脚*/, 0x0);
    pinMode(A4 /* 中间的超声波模块的触发引脚*/, 0x1);
    pinMode(A5 /* 中间的超声波模块的回响引脚*/, 0x0);
    pinMode(A0 /* 右边的超声波模块的触发引脚*/, 0x1);
    pinMode(A1 /* 右边的超声波模块的回响引脚*/, 0x0);
    pinMode(7, 0x0);
    pinMode(8, 0x0);
    pinMode(6, 0x0);
    pinMode(9, 0x0);
    pinMode(12, 0x1);
    Serial.begin(9600);
}

int bluetoothWork(){
    // 如果蓝牙有数据可读
    while (1)
    {
        if (Serial.available())
        {
            // 读取一个字节的数据并存储在data变量中
            data = Serial.read();

            // 根据不同的数据执行不同的动作
            switch (data)
            {
            case 'F': // 前进
                Work(1 /* 前进*/, 200);
                break;
            case 'B': // 后退
                Work(2 /* 后退*/, 150);
                break;
            case 'L': // 左转
                Work(3 /* 左转*/, 150);
                break;
            case 'R': // 右转
                Work(4 /* 右转*/, 100);
                break;
            case 'S': // 停止
                Work(0 /* 停止*/, 0);
                break;
            case 'A':
                digitalWrite(12, 0x1);
                delay(2000);
                digitalWrite(12, 0x0);
                break;
            default: // 其他情况，停止
                Work(0 /* 停止*/, 0);
                break;
            }
        }
    }
}

// 定义一个函数，用于测量超声波模块返回的距离（单位为厘米）
float measureDistance(int trigPin, int echoPin){
    // 发送一个10微秒的高电平信号到触发引脚
    digitalWrite(trigPin, 0x1);
    delayMicroseconds(10);
    digitalWrite(trigPin, 0x0);
    delayMicroseconds(10);
    // 测量回响引脚返回的高电平信号的持续时间（单位为微秒）
    long duration = pulseIn(echoPin, 0x1);

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
        L = digitalRead(7);
        R = digitalRead(8);
        L1 = digitalRead(6);
        R1 = digitalRead(9);

        if (L1 == 0x0 && L == 0x0 && R == 0x0 && R1 == 0x0)
        { // 直走
            if (lastDir == -2)
            {
                Work(3 /* 左转*/, 110);
            }
            else if (lastDir == 2)
            {
                Work(4 /* 右转*/, 110);
            }else{
              Work(1 /* 前进*/,80);
              lastDir=0;
            }
        }
        else if ((L1 == 0x0 && L == 0x0 && R == 0x0 && R1 == 0x1) || (L1 == 0x0 && L == 0x0 && R == 0x1 && R1 == 0x1) || (L1 == 0x0 && L == 0x1 && R == 0x1 && R1 == 0x1))
        { // 大幅左转
            Work(3 /* 左转*/, 110);
            lastDir = -2;
        }
        else if (L1 == 0x0 && L == 0x0 && R == 0x1 && R1 == 0x0)
        { // 一般左转
            Work(3 /* 左转*/, 110);
            lastDir = -1;
        }
        else if ((L1 == 0x1 && L == 0x0 && R == 0x0 && R1 == 0x0) || (L1 == 0x1 && L == 0x1 && R == 0x0 && R1 == 0x0) || (L1 == 0x1 && L == 0x1 && R == 0x1 && R1 == 0x0))
        { // 大幅右转
            Work(4 /* 右转*/, 110);
            lastDir = 2;
        }
        else if (L1 == 0x0 && L == 0x1 && R == 0x0 && R1 == 0x0)
        { // 一般右转
            Work(4 /* 右转*/, 110);
            lastDir = 1;
        }
        else if ((L1 == 0x0 && L == 0x1 && R == 0x0 && R1 == 0x1))
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0;// 停止
        }
        else if (L1 == 0x0 && L == 0x1 && R == 0x1 && R1 == 0x0)
        { // 直走
            Work(1 /* 前进*/, 80);
            lastDir = 0;
        }
        else if (L1 == 0x1 && L == 0x0 && R == 0x0 && R1 == 0x1)
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == 0x1 && L == 0x0 && R == 0x1 && R1 == 0x0)
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == 0x1 && L == 0x0 && R == 0x1 && R1 == 0x1)
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == 0x1 && L == 0x1 && R == 0x0 && R1 == 0x1)
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0; // 停止
        }
        else if (L1 == 0x1 && L == 0x1 && R == 0x1 && R1 == 0x1)
        {
            Work(0 /* 停止*/, 0);
            lastDir = 0; // 停止
        }
        else
        {
            lastDir = 0;
        }

        if (Serial.available())
        {
            char btCommand = Serial.read();
            if (btCommand == 'C')
            {
                Work(0 /* 停止*/, 0);
                return;
            }
        }

        delay(20);
        Work(0 /* 停止*/, 0);
        delay(20);
        Serial.println("红外循迹");
    }
}

int chaosb(){
    while (1)
    {
        // 测量三个超声波模块返回的距离
        float distance1 = measureDistance(A2 /* 左边的超声波模块的触发引脚*/, A3 /* 左边的超声波模块的回响引脚*/);
        float distance2 = measureDistance(A4 /* 中间的超声波模块的触发引脚*/, A5 /* 中间的超声波模块的回响引脚*/);
        float distance3 = measureDistance(A0 /* 右边的超声波模块的触发引脚*/, A1 /* 右边的超声波模块的回响引脚*/);
        Serial.println(distance1);
        Serial.println(distance2);
        Serial.println(distance3);

        // 特别处理distance1过小的情况
        if (distance2 < 10)
        { // 假设10cm是后退的临界距离
            Work(2 /* 后退*/, 100); // 后退
            delay(50); // 给足够时间后退
            Work(0 /* 停止*/, 0);
            delay(50);
            // continue; // 跳过其余的逻辑，重新测量距离
        }

        // 基于中间和右边距离决定行动
        if (distance1 < 20 || distance3 < 20)
        { // 假设20cm是临界距离
            if (distance1 > distance3)
            {
                Work(4 /* 右转*/, 120); // 向左转
            }
            else
            {
                Work(3 /* 左转*/, 120); // 向右转
            }
            delay(20); // 转向一段时间后再继续前进
            Work(0 /* 停止*/, 0);
            delay(20);
        }
        else
        {
            Work(1 /* 前进*/, 90); // 如果没有检测到障碍物，就直行
            delay(20);
        }
        Serial.println("超声波");

        // 检查蓝牙信号'C'退出功能
        if (Serial.available())
        {
            char btCommand = Serial.read();
            if (btCommand == 'C')
            {
                Work(0 /* 停止*/, 0);
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
