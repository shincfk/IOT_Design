##About##
get Json data from [http://www.xianuniversity.com/athome/1][1] and communcation with Arduino use pyserial.

###curl

1.post

    curl --data "_method=PUT&led1=1&sensors1=22&sensors2=12&temperature=14" http://b.phodal.com/athome/1

2.get 
  
    curl http://b.phodal.com/athome/1
    
###Python Module###
 - JSON
 - urllib2
 - serial

###HOW TO###
    pip install pyserial

OR

    easy_install pyserial

if Windows download from Internet
###!Not Work###
Find which is you Arduino Serial Port
if Linux is /dev/ttyACM*



[1]:http://www.xianuniversity.com/athome/1 