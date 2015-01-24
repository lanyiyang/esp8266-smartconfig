esp8266-smartconfig
=======

Instruction in English is followed by Chinese.
### 说明
这只是一个测试demo，仅供学习交流。如果你测试出软件的bug 或者有任何改进意见，请联系我。
QQ：491505215
Email：lanyiyang_dh@163.com

### Introductions
This just a test demo, only for study and communication. If you test the software bugs or have any improvement, please contact me.
QQ：491505215
Email：lanyiyang_dh@163.com

### 名词定义
*Smartconfig* 用于在物理网设备没有外部输入端时，使用手机对其进行配置的一种通用的说法。  

### Definition of Smartconfig
*Smartconfig* is a generic phrase when you configure your IoT devices by mobile phone, PC etc. instead of physical external devices.

### 使用方法
在需要使用 `smartconfig` 的地方调用

```c
smartconfig_init();
```

**手机端**: 有配套的Android demo。IOS的demo麻烦万能的网友帮忙实现了。

**SDK 版本**: 0.9.3 by Espressif
由于espressif在v0.9.4上面修改了sniffer的函数，导致用户层无法得到一个完整的数据包，所以目前只能在0.9.3的版本上测试。

### Usage
Call `smartconfig_init();` when you want to use Smarkconfig to configure.

**Mobile**: Just have Android demo. If you have realized the IOS demo,please contact me.

**SDK Version**: 0.9.3 by Espressif

### 原理
UDP packet len。

### Principle
UDP packet len。

### 协议
The MIT License (MIT)

Copyright (c) 2014 younger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


### License
See above.

### Reference
在这里特别感谢kali的blog，从他的代码中进行了修改，欢迎大家讨论。