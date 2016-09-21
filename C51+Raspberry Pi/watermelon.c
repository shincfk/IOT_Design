#include <reg52.h>
#include <intrins.h>

typedef unsigned char unint8;
typedef unsigned char unint16;
unsigned char flag=0; //紧急报警标示位
unsigned char flag1=0; //传感器检测报警标示位
unsigned char str1[]={"        "};
unsigned char str2[]={"        "};
unsigned char code dis1[] = {"TRH RH: "};
unsigned char code dis2[] = {"TRH  T: "};
sbit CS=P2^4;       
sbit CLK=P1^1;      
sbit DIO=P1^2;       
sbit K1=P3^3;
sbit LED1=P2^0;		 //警情指示灯
sbit LED2=P2^1;		 //正常指示灯
sbit BEEP=P2^2;
sbit TRH = P3^6;//温湿度传感器DHT11数据接入
//sbit ST  = P2^1;//状态显示
sbit LCD_RS = P2^5;             
sbit LCD_RW = P2^6;
sbit LCD_EN = P2^7;
char t,buf[6],buf_cnt;
bit  f,send_f;
unint8 TH_data,TL_data,RH_data,RL_data,CK_data;
unint8 TH_temp,TL_temp,RH_temp,RL_temp,CK_temp;
unint8 com_data,untemp,temp;
unint8 respond;

/*********************************************************************************/
void delayNOP()
{
         _nop_();
         _nop_();
         _nop_();
         _nop_();
}
/*********************************************************************************/


/*********************************************************************************/
// 毫秒级延时子程序
/*********************************************************************************/
void delay_ms(unsigned char ms)
{       
          unsigned char i;
          while(ms--)
          {
                    for(i = 0; i< 150; i++)
                    {
                              _nop_();
                              _nop_();
                              _nop_();
                              _nop_();
                    }
          }
}
/*********************************************************************************/


/*********************************************************************************/
//5us级延时程序
/*********************************************************************************/
void delay_us()
{
           unint8 i;
          i--;
          i--;
          i--;
          i--;
          i--;
          i--;
} 
/********************************************************************************/
void delay1ms(char t)
{
	int i,j;
	for(i=0;i<t;i++)
		for(j=0;j<1000;j++);
}

      
/********************************************************************************/
// 测试LCD忙碌状态
//lcd_busy()为1时，忙，等待。lcd_busy()为0时,闲，可写指令与数据。
/********************************************************************************/
bit lcd_busy()
{       
         bit result;
         LCD_RS = 0;
         LCD_RW = 1;
         LCD_EN = 1;
          delayNOP();
         result = (bit)(P0&0x80);
         LCD_EN = 0;
         return(result);
}
/*******************************************************************************/


/*******************************************************************************/
// 写入指令数据到LCD
/*******************************************************************************/
void lcd_wcmd(unsigned char cmd)
{       
         while(lcd_busy());
         LCD_RS = 0;
         LCD_RW = 0;
         LCD_EN = 0;
          _nop_();
          _nop_(); 
         P0 = cmd;
          delayNOP();
         LCD_EN = 1;
         delayNOP();
         LCD_EN = 0; 
}
/*****************************************************************************/


/*****************************************************************************/
//写显示数据到LCD                                                
//RS=高电平，RW=低电平，E=高脉冲，D0-D7=数据。                             
/*****************************************************************************/
void lcd_wdata(unsigned char dat)
{                          
         while(lcd_busy());
          LCD_RS = 1;
         LCD_RW = 0;
         LCD_EN = 0;
         P0 = dat;
         delayNOP();
         LCD_EN = 1;
         delayNOP();
         LCD_EN = 0; 
}
/****************************************************************************/


/****************************************************************************/
//LCD初始化设定
/****************************************************************************/
void lcd_init()
{ 
        delay_ms(15);                   
        lcd_wcmd(0x38);      //16*2显示
        delay_ms(5);
        lcd_wcmd(0x38);         
        delay_ms(5);
        lcd_wcmd(0x38);         
        delay_ms(5);

        lcd_wcmd(0x0c);      //显示开，关光标
        delay_ms(5);
        lcd_wcmd(0x06);      //移动光标
        delay_ms(5);
        //lcd_wcmd(0x01);      //清除LCD的显示内容，如果屏幕过暗，可将这俩句删除
        //delay_ms(5);
}
/****************************************************************************/


/****************************************************************************/
//设定LCD显示位置
/****************************************************************************/
void lcd_dis_pos(unsigned char pos)
{       
          lcd_wcmd(pos | 0x80); //数据指针=80+地址变量
}
/****************************************************************************/


/****************************************************************************/
//收发信号检测，数据读取
/****************************************************************************/
char receive()
{    
          unint8 i;
          //ST=0;
          com_data=0;
          for(i=0;i<=7;i++)    
         {
                    respond=2;
                    while((!TRH)&&respond++);
                    delay_us();
                    delay_us();
                    delay_us();
                    if(TRH)
                    {
                              temp=1;
                              respond=2;
                              while((TRH)&&respond++); 
                    } 
                    else
                              temp=0;
                    com_data<<=1;
                    com_data|=temp;    
          }
          return(com_data);  
}
/****************************************************************************/


/****************************************************************************/
//湿度读取子程序 
//温度高8位== TL_data
//温度低8位== TH_data
//湿度高8位== RH_data
//湿度低8位== RH_data
//校验 8位 == CK_data
//调用的程序有 delay();, Delay_5us();,RECEIVE(); 
/***************************************************************************/

void read_TRH()
{
          //主机拉低18ms 
          TRH=0;
         delay_ms(18);
         TRH=1;
          //DATA总线由上拉电阻拉高 主机延时20us
         delay_us();
         delay_us();
         delay_us();
         delay_us();
          //delay_us();
          //delay_us();delay_us();delay_us();delay_us();
          //主机设为输入 判断从机响应信号 
             TRH=1;
          //判断DHT11是否有低电平响应信号 如不响应则跳出，响应则向下运行   
         if(!TRH)   
         {
                  respond=2;
                  //判断DHT11发出 80us 的低电平响应信号是否结束 
                  while((!TRH)&& respond++);
                 respond=2;
                 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
                while(TRH && respond++);
               //数据接收状态   
                RH_temp = receive(); 
                RL_temp = receive(); 
                TH_temp = receive(); 
                TL_temp = receive(); 
                CK_temp = receive();
                TRH=1;//ST=1;     
                 //数据校验 
                 untemp=(RH_temp+RL_temp+TH_temp+TL_temp);
                 if(untemp==CK_temp)
                 {
                          RH_data = RH_temp;
                          RL_data = RL_temp;
                         TH_data = TH_temp; 
                          TL_data = TL_temp;
                          CK_data = CK_temp;
                }
        }
         //湿度整数部分
         str1[0] = (char)(0X30+RH_data/10); 
         str1[1] = (char)(0X30+RH_data%10);
         str1[2] =  0x2e; //小数点
          //湿度小数部分
         str1[3] = (char)(0X30+RL_data/10); 
 		 str1[4] =0x20;
          str1[5] = 0X25; //"%"
         str1[6] = 0X52; //"R"
         str1[7] = 0X48; //"H"
         //温度整数部分
         str2[0] = (char)(0X30+TH_data/10); 
         str2[1] = (char)(0X30+TH_data%10);
         str2[2] =  0x2e; //小数点
         //温度小数部分
          str2[3] = (char)(0X30+TL_data/10); 
		  	 str2[4] =0x20;
          str2[5] = 0Xdf;  //"'"
          str2[6] = 0X43;  //"C"
}

void key()
{
	if(K1==0)
	{
		delay_ms(5);
		if(K1==0)
		{
			while(!K1);
			flag++;
			if(flag==2)
			flag=0;
		}
	}
}
/*****************************************************

 函数功能：报警子程序

*****************************************************/
void baojing()
{
	if(flag==1||flag1==1)
	{
		LED2=1;
		LED1=0;
		BEEP=0;
		delay_ms(100);
		BEEP=1;
		delay_ms(100);
	}
	if(flag==0&&flag1==0)
	{
		LED2=0;
		LED1=1;
		BEEP=1;
	}
}
/*****************************************************
函数功能：将模拟信号转换成数字信号
***************************************************/ 
unsigned int  A_D()
{
  unsigned char i,dat;
   CS=1;   //一个转换周期开始
   CLK=0;  //为第一个脉冲作准备
   CS=0;  //CS置0，片选有效

   DIO=1;    //DIO置1，规定的起始信号  
   CLK=1;   //第一个脉冲
   CLK=0;   //第一个脉冲的下降沿，此前DIO必须是高电平
   DIO=1;   //DIO置1， 通道选择信号  
   CLK=1;   //第二个脉冲，第2、3个脉冲下沉之前，DI必须跟别输入两位数据用于选择通道，这里选通道CH0 
   CLK=0;   //第二个脉冲下降沿 
   DIO=0;   //DI置0，选择通道0
   CLK=1;    //第三个脉冲
   CLK=0;    //第三个脉冲下降沿 
   DIO=1;    //第三个脉冲下沉之后，输入端DIO失去作用，应置1
   CLK=1;    //第四个脉冲
   for(i=0;i<8;i++)  //高位在前
    {
      CLK=1;         //第四个脉冲
      CLK=0; 
      dat<<=1;       //将下面储存的低位数据向右移
		dat|=(unsigned char)DIO; 	 //将输出数据DIO通过或运算储存在dat最低位 
    }	  		        
    CS=1;          
	 return dat;	 //将读书的数据返回     
  }
/*****************************************************

  函数功能：警情检测子程序

*****************************************************/
void jq()
{
	unsigned int AD_val;    //储存A/D转换后的值
	AD_val=A_D();    //进行A/D转换
	if(AD_val>=50)
	flag1=1;
	else
	flag1=0;
}

//********************************中断服务函数块*********************************************

void ComSer() interrupt  4		  //串行口中断服务函数（串口数据接收）
{	
	if(RI)
	{  
	
		RI=0;
		
	} 
}	
//********************************中断服务函数块*********************************************
//************************串行口字符（字符串）发送块******************************************	
void putchar(unsigned char n)
{ 
   SBUF=n;
   while(!TI);
   TI=0;
} 
void puts(unsigned char *q)
{
   while(*q)
      putchar(*q++);
}						  
void Enter()	  //换行函数
{
   putchar(0x0d);
   putchar(0x0a); 
}
void DispNum(unsigned char n)
{
	unsigned char t[8]={0};
	t[0]=n/10+'0';
	t[1]=n%10+'0';
	//t[2]=0; 
	putchar(t[0]);
    putchar(t[1]);
   // putchar(t[2]);

//	puts(t); 
   Enter();
}
//************************串行口字符（字符串）发送块******************************************	
//********************************串行口请求判断块**************************************** 
/*void Dill()
{
	if(buf[0]=='?')	 	 
	{
		delay1ms(50);
		send_f=1; 
	}
} */
//********************************串行口请求判断块****************************



//********************************初始化函数块*********************************************

void init_com()			 //寄存器初始化函数
{
   SCON=0x50;   	//串行口工作方式0，多机通信，允许接收
   IE|=0X90;		//定时器T1和总中断允许
   PCON=0X80;		//SMOD为1
   TMOD|=0X21;		//T1工作在方式2，T0工作在方式1
   TH1=250; 		//T1高八位赋值
   TR1=1; 			//T1打开
   TR0=1;  			//T0关闭
}
/****************************************************************************/
//主函数
/****************************************************************************/
void main()
{
	       
          unsigned char i,n=0x40,m;
          LED2=1;
		  LED1=1;
	      BEEP=1; 
          init_com();
          lcd_init(); 
          delay_us(); 
          while(1)
          {
                    
                    //写字符
                     for(i=0;i<=7;i++)
                    {
                            lcd_dis_pos(i);    //显示字符
                            lcd_wdata(dis1[i]);
                            lcd_dis_pos(n+i);    //显示字符
                            lcd_wdata(dis2[i]);
                    }
                   //写湿度数据
                   m=0x08;
                   for(i=0;i<=7;i++)      
                   {
                           lcd_dis_pos(m); 
                           lcd_wdata(str1[i]);
                           m++;
                  }
                  //写温度数据
                  m=0x48;
                  for(i=0;i<=7;i++)      
                  {
                           lcd_dis_pos(m); 
                           lcd_wdata(str2[i]);
                           m++;
                   }
                   //延时
                  delay_ms(500) ;

                 
                    /*if(send_f)
	            	{
		             	send_f=0;

		             	puts("Wendu:");
			            DispNum(TH_data);
			            puts("Shidu:");
		            	DispNum(RH_data);
		            	Enter();
		             } 
		           if(f)	
	               {
		        	f=0;
			        ES=0;
			        Dill();	   //无线设置函数
			        ES=1;
	            	}*/
				jq();
		        key();
		        baojing();
				
				read_TRH();
			   
		           // puts("Wendu:");
			        DispNum(TH_data);
			       // puts("Shidu:");
		           // DispNum(RH_data);
		           // Enter();
					delay_ms(500);
        }    
	
} 