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
# ��ս��ջ�����  
port.flushInput()
# ��Ҫ�������ʱ  
time.sleep(0.1)
#����ת����JSON��ʽ
res =  '{"value":%s}' %data

#���ļ�
output = open('/home/pi/python/mytest/tem_data.txt','w')
#д���ݵ��ı��ļ���
output.write(res)
#�ر��ļ�
output.close