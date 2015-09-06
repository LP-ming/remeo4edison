/******************************************************************
                   cmd     arg
Axģ�������       0xAx    0 ����ȡ��ѹֵ   ��0  ��ȡ��ѹֵ
Ax���������       0xDx    1 high   0 low
���1����          0xB1    1 ��ʱ�� 0 ˳ʱ��
���2����          0xB2    1 ��ʱ�� 0 ˳ʱ��
���1�ٶ�          0xC1    value �ٶ�ֵ
���2�ٶ�          0xC2    value �ٶ�ֵ
�������           0xEx    value  ռ�ձ�

��M��  0x4D
M2
D9      ENA
D7      DIRA       

M1
D10     ENB
D8      DIRB

Version: V1.3
******************************************************************/

#include <Wire.h>
#include <Servo.h> 

Servo myservo;
unsigned char cmd=0,arg=0;
int ADC_value=0;
unsigned char Check_State=0;
#define DBG 0
//�����ʼ��
void setup()
{
#if (DBG==1)
  Serial.begin(115200);
#endif
  Wire.begin(4);                         // ���� i2c ���ߣ����ôӻ���ַΪ 0xDF
  Wire.onReceive(I2C_slave_receive_Handler);//�����յ���������ʱ�Ĵ�����
  Wire.onRequest(I2C_slave_send_Handler);   //�����յ���������ʱ���ϴ����ݺ���
}

//������
void loop()
{
}

// �������߽��յ���������ʱ��ִ�иú���
void I2C_slave_receive_Handler(int None)
{
	/* 55 aa cmd arg cs*/
	int ok=0;
    int count = 0;
	unsigned char header1,header2,cmd,arg,cs;
	do{
		while( Wire.available() == 0){ // ѭ��ִ�У�ֱ�����ݰ�ֻʣ�����һ���ַ�
                  if(++count == 1000) break;
                  delay(1);
                }
		header1 = Wire.read(); //��������ĵ�һ���ֽ�
#if (DBG==1)
                Serial.print("header1=");Serial.println(header1,HEX);
#endif
		if(header1 != 0x55)
			break;
                count = 0;
		while( Wire.available() == 0){
                  if(++count == 1000) break;
                  delay(1);
                }
		header2 = Wire.read();
#if (DBG==1)
                Serial.print("header2=");Serial.println(header2,HEX);
#endif
		if(header2 != 0xaa)
			break;
                count=0;
		while( Wire.available() == 0){
                  if(++count == 1000) break;
                  delay(1);
                }
		cmd = Wire.read();
#if (DBG==1)
                Serial.print("cmd=");Serial.println(cmd,HEX);
#endif
		count=0;
                while( Wire.available() == 0){
                  if(++count == 1000) break;
                  delay(1);
                }
		arg = Wire.read();
#if (DBG==1)
                Serial.print("arg=");Serial.println(arg,HEX);
#endif
                count=0;
		while( Wire.available() == 0){
                  if(++count == 1000) break;
                  delay(1);
                }
		cs = Wire.read();
#if (DBG==1)
                Serial.print("cs=");Serial.println(cs,HEX);
#endif
		if(cs == (unsigned char)(header1+header2+cmd+arg)){
			ok = 1;
		}
	}while(0);
	if(!ok){
                //Serial.println("ok == 0");
		return;
	}
        exec(cmd,arg);
	ok=0;
}
void exec(unsigned char cmd, unsigned char arg)
{
	unsigned char x = cmd &0x0F;
	switch(cmd&0xF0){
		case 0xA0:
			if(x < 4)
				pinMode(x+14, INPUT);//��Ax��Ϊģ�������
                        if(arg)
                                ADC_value=analogRead(x+14);
		break;
		case 0xD0:
			if(x < 4){
				pinMode(x+14, OUTPUT);//��Ax��Ϊ���������	
				digitalWrite(7,  arg);
			}
		break;
        case 0xB0: //�������
			if(x == 1){//����M1���˳ʱ��ת��
				pinMode(8, OUTPUT);
				if(arg == 0)//����M1���˳ʱ��ת��
					digitalWrite(8,  LOW);
				else		  //����M1�����ʱ��ת��
					digitalWrite(8,  HIGH);
			}else if(x == 2){
				pinMode(7, OUTPUT);
				if(arg == 0)//����M1���˳ʱ��ת��
					digitalWrite(7,  LOW);
				else		  //����M1�����ʱ��ת��
					digitalWrite(7,  HIGH);				
			}
		break;
		case 0xC0: //����ٶ�
			if(x == 1)
				analogWrite(10, arg);
			else
				analogWrite(9,  arg);
		break;
		case 0xE0: //�������
			if(x < 4){
				myservo.attach(x+14);
				myservo.write(map(arg, 0, 255, 0, 180));
			}
		break;
		default:
		break;
		}
}
//������֪ͨ�ӻ��ϴ�����ʱ��ִ�и��¼�
void I2C_slave_send_Handler()
{
        unsigned char buf[5];
        buf[0]=0x55;buf[1]=0xaa;
        buf[2]=ADC_value&0xff;buf[3]=ADC_value>>8;
        buf[4]=buf[0]+buf[1]+buf[2]+buf[3];
	Wire.write((const uint8_t *)buf,5); //�����߷���������Ҫ��ȡ������
        //Wire.write(ADC_value>>8); //�����߷���������Ҫ��ȡ������
        //Serial.print("ADC_value=");Serial.println(ADC_value,HEX);
}
