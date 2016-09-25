#!/usr/bin/env/python
# encoding: utf-8
import serial
import time
port = serial.Serial("/dev/ttyAMA0",baudrate=9600,timeout=8.0)
#port.open()
#while True:
stop = False
data=port.readline()
print "tem:",data
# 清空接收缓冲区  
port.flushInput()
# 必要的软件延时  
time.sleep(0.1)
#数据转换成JSON格式
res =  '{"value":%s}' %data

#打开文件
output = open('/home/pi/python/mytest/tem_data.txt','w')
#写数据到文本文件中
output.write(res)
#关闭文件
output.close