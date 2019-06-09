# db_final
## todolist
#### 多表联合
#### union
#### sql算术运算符
#### 多表连接join
#### 数据库存档
#### Makefile

# 测试客户端和服务端
0. 在你的IDE内部链接库ws2_32.lib

1. 运行final_project.exe  
> 命令行参数：9999

2. 运行client.exe
> 命令行参数：127.0.0.1 9999 <你要输入的测试文件名

3. 如果你不知道如何配置IDE使之链接ws2_32.lib，那么：
> g++ -o -xx -xx.cpp -L./ -lWS2_32
##### 将WS2_32.LIB放在xx.cpp同级目录下。
##### 对于服务端，可能你需要修改Makefile。

## 正常情况下，服务端返回结果打印在client的std::cout上。

![server端正常界面](https://github.com/worsecoder/db_final/blob/master/server_ui.PNG)
![client端正常界面](https://github.com/worsecoder/db_final/blob/master/client_ui.PNG)
