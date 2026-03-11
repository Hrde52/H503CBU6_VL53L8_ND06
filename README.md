#


## FLASH消耗情况
### 20260212加入了VL53L8CX的API,优化O0
```
Program Size: Code=40652 RO-data=88548 RW-data=12 ZI-data=6436  
```
根据 Keil 编译输出的信息，程序占用的 **Flash 大小 = Code + RO-data + RW-data**（因为 RW-data 的初始值存储在 Flash 中，运行时才复制到 RAM）。

```
Code        = 40652 字节
RO-data     = 88548 字节
RW-data     = 12 字节
-------------------------
Flash 占用  = 40652 + 88548 + 12 = 129212 字节
```

**换算**：  
129212 字节 = 129212 ÷ 1024 ≈ **126.2 KB**

**各部分含义**：  
- **Code**：程序代码（.text 段），占 Flash  
- **RO-data**：只读数据（常量、const 变量等），占 Flash  
- **RW-data**：已初始化全局变量，其初始值存储在 Flash 中，运行时被复制到 RAM，因此**初始值部分也占 Flash**  
- **ZI-data**：未初始化全局变量，仅占 RAM，**不占用 Flash**

所以最终的 Flash 占用是 **129212 字节**（约 126.2 KB）。

优化策略| O0 | O3 | OZ
--------
Code   |40652 | |



### 2026

