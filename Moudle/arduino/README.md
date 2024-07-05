前置条件：

使用Arduino IDE进行开发，下载开发板资源和库

![image-20240705121529001](C:/Users/瓜子的皮/AppData/Roaming/Typora/typora-user-images/image-20240705121529001.png)

![image-20240705121540888](C:/Users/瓜子的皮/AppData/Roaming/Typora/typora-user-images/image-20240705121540888.png)

根据如下示例进行

[ESP32cam系列教程001：使用webcam摄像头实时查看视频_esp32 cam使用教程-CSDN博客](https://blog.csdn.net/qiaoyurensheng/article/details/131726234)

将CameraWebServer.ino的代码修改为文件夹内代码即可

其中HaveWebserver.ino的代码为具有网页可以访问摄像头代码

NoHaveWebserver.ino为删除网页功能只有串口给hi3861发送图片的代码