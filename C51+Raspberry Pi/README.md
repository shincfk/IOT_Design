为脚本增加可执行权限
```
sudo chmod +x yeetem.sh
```

crontab 
```
sudo crontab -e
```
让脚本每1分钟运行一次，即每1分钟上传一次温度数据
```
*/1 * * * * /home/pi/python/yeetem.sh
```

