#include "control.h"
#include "MPU6050.h"
#include "remote.h"
#include "beep.h"
#include "timer.h"
#include "usart.h"

struct robot robot_zqd;
u32 uart_data[3]={0,2000,1000};			//���ڽ������ݴ洢��xλ�ã���ȣ��뾶
u32 uart3_data[3]={0,2000,1000};		//���ڽ������ݴ洢��xλ�ã���ȣ��뾶
u8 lankuang_state=0;

void control_init(void)
{
	robot_zqd.X = 0;		//������������ϵ��x����
	robot_zqd.Y = 0;		//������������ϵ��y����
	robot_zqd.theta = 0;	//�������������y��н�
	robot_zqd.Vx = 0;		//������������ϵx�����ٶ�
	robot_zqd.Vy = 0;		//������������ϵy�����ٶ�
	robot_zqd.W = 0;		//�����˽��ٶȣ�˳ʱ��������
	robot_zqd.w[1] = 0;		//��һ���ֵĽ��ٶ�
	robot_zqd.w[2] = 0;		//�ڶ����ֵĽ��ٶ�
	robot_zqd.w[0] = 0;		//�������ֵĽ��ٶ�
	robot_zqd.v[1] = 0;		//��һ���ֵ��ٶ�
	robot_zqd.v[2] = 0;		//�ڶ����ֵ��ٶ�
	robot_zqd.v[0] = 0;		//�������ֵ��ٶ�
	robot_zqd.theta_dev = 0;
	robot_zqd.theta_offset = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	
	charge(0);
	
	MPU_Init();			//MPU6050��ʼ��
	MPU_Init();
	MPU_Init();
	
	TIM_SetCompare2(TIM9,MOTOR_STATIC_2); 		//PE6
	TIM_SetCompare1(TIM9,MOTOR_STATIC_1);			//PE5
	
	LCD_Show_lcj();
	LCD_Show_v();
	LCD_Show_V();
	LCD_Show_position();
}

//���1 ת��W
void control1_W(float W)
{
	W=1000 - W;//W+=1000;
	if(W>=1900)
		W=1900;
	if(W<=100)
		W=100;
 	TIM_SetCompare1(TIM3,(uint32_t)W);			//PC6

}

//���2 ת��W
void control2_W(float W)
{
	W=1000 - W;//W+=1000;
	if(W>=1900)
		W=1900;
	if(W<=100)
		W=100;
 	TIM_SetCompare2(TIM3,(uint32_t)W);			//PC7

}

//���3 ת��W
void control3_W(float W)
{
	W=1000 - W;//W+=1000;
	if(W>=1900)
		W=1900;
	if(W<=100)
		W=100;
 	TIM_SetCompare3(TIM3,(uint32_t)W);			//PC8

}


//�����ʼ��,GPIOG7
void shot_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOEʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��

	GPIO_ResetBits(GPIOG,GPIO_Pin_7);
}

//��λ���س�ʼ��
void xianwei_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOEʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	//GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOE0
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOE0
	
	

}

//���⿪�س�ʼ��
void hongwai_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOF9

}


//��ȡ����״̬
void get_hongwai(void)
{
	// return GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9);
	while(1)
		{
		if(!GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9))
			{
			break;
			}
		}
}

//��ȡ����״̬
void get_hongwai_dixian(float dis)
{
	while(1)
	{
		if(!GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9))
			{
			break;
			}
		if(robot_zqd.X < dis)
			{
			break;
			}
	}
}


//����ת��
void get_position(void)
{
	
	
	//�����ٶ�����������
	u8 i,j,k;
	float theta;
	float L_inv[3][3];
	float theta_inv[3][3];
	float tem[3][3];
	
	//ȡTheta������ֵ
	if(fabs(robot_zqd.theta - robot_zqd.theta_dev) < PI)
		theta = (robot_zqd.theta + robot_zqd.theta_dev) / 2.0f;
	else
		theta = (robot_zqd.theta + robot_zqd.theta_dev) / 2.0f + PI;
	robot_zqd.theta_dev = robot_zqd.theta;
	
	//v(bmq)=L * Theta * V
	//theta_inv
	theta_inv[0][0]= cos(theta);	theta_inv[0][1] = -sin(theta);		theta_inv[0][2] = 0;
	theta_inv[1][0]= sin(theta);	theta_inv[1][1] = cos(theta);		theta_inv[1][2] = 0;
	theta_inv[2][0]= 0;				theta_inv[2][1] = 0;				theta_inv[2][2] = 1;
	//		[-cos(0)	-sin(0)		BMQ_L]-1
	//L_inv=[-cos(120) 	-sin(120)	BMQ_L]
	//		[-cos(-120)	-sin(-120)	BMQ_L]
	L_inv[0][0] = -0.666666666666667;		L_inv[0][1] =  0.333333333333333;		L_inv[0][2] = 0.333333333333333;
	L_inv[1][0] =  0;						L_inv[1][1] = -0.577350269189626;		L_inv[1][2] = 0.577350269189626;
	L_inv[2][0] =  1.666666666666667;		L_inv[2][1] =  1.666666666666667;		L_inv[2][2] = 1.666666666666667;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			tem[i][j] = 0;
			for(k=0;k<3;k++)
			{
				tem[i][j] += theta_inv[i][k] * L_inv[k][j];
			}
		}
	}
	
	robot_zqd.Vx = 0;
	for(j=0;j<3;j++){
		robot_zqd.Vx += tem[0][j] * robot_zqd.v[j];
	}
	robot_zqd.Vy = 0;
	for(j=0;j<3;j++){
		robot_zqd.Vy += tem[1][j] * robot_zqd.v[j];
	}
	robot_zqd.W = 0;
	for(j=0;j<3;j++){
		robot_zqd.W += tem[2][j] * robot_zqd.v[j];
	}
	
	robot_zqd.X += robot_zqd.Vx*0.01f;
	robot_zqd.Y += robot_zqd.Vy*0.01f;
	
	/*
	//������̼Ƽ���Ƕ�
	//�������Ǵ���
	robot_zqd.theta += robot_zqd.W*0.01f;
	
	while(robot_zqd.theta<0)
	{
		robot_zqd.theta += 2*PI;
	}
	while(robot_zqd.theta > 2*PI)
	{
		robot_zqd.theta -= 2*PI;
	}
	*/
	
	
	
	
	/*
	//������̼��ۼ�ֵ����������
	u8 i,j,k;
	float L_inv[3][3];
	float theta_inv[3][3];
	float tem[3][3];
	//L*theta*V=R*w
	//theta_inv
	theta_inv[0][0]= 1;		theta_inv[0][1] = 0;		theta_inv[0][2] = 0;
	theta_inv[1][0]= 0;		theta_inv[1][1] = 1;		theta_inv[1][2] = 0;
	theta_inv[2][0]= 0;		theta_inv[2][1] = 0;		theta_inv[2][2] = 1;
	//L_inv
	
	L_inv[0][0] = -0.666666666666667;		L_inv[0][1] =  0.333333333333333;		L_inv[0][2] = 0.333333333333333;
	L_inv[1][0] =  0;						L_inv[1][1] = -0.577350269189626;		L_inv[1][2] = 0.577350269189626;
	L_inv[2][0] =  1.877934272300470;		L_inv[2][1] =  1.877934272300470;		L_inv[2][2] = 1.877934272300470;
	
	//L_inv[0][0] = -0.691823899371069;		L_inv[0][1] =  0.345911949685535;		L_inv[0][2] = 0.345911949685535;
	//L_inv[1][0] =  0;						L_inv[1][1] = -0.558469178636527;		L_inv[1][2] = 0.558469178636527;
	//L_inv[2][0] =  1.736203383824963;		L_inv[2][1] =  1.948799716538223;		L_inv[2][2] = 1.948799716538223;
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			tem[i][j] = 0;
			for(k=0;k<3;k++)
			{
				tem[i][j] += theta_inv[i][k] * L_inv[k][j];
			}
		}
	}
	
	robot_zqd.X = 0;
	for(j=0;j<3;j++){
		robot_zqd.X += tem[0][j] * robot_zqd.w[j]*0.0001727875959474386f;
	}
	robot_zqd.Y = 0;
	for(j=0;j<3;j++){
		robot_zqd.Y += tem[1][j] * robot_zqd.w[j]*0.0001727875959474386f;
	}
	robot_zqd.Y *= 1.123f;
	robot_zqd.theta = 0;
	for(j=0;j<3;j++){
		robot_zqd.theta += tem[2][j] * robot_zqd.w[j]*0.0001727875959474386f;
	}
	robot_zqd.theta *= 1.14f;
	
	while(robot_zqd.theta<0)
	{
		robot_zqd.theta += 2*PI;
	}
	while(robot_zqd.theta > 2*PI)
	{
		robot_zqd.theta -= 2*PI;
	}
	*/
	/*
	//��
	u8 i;
	float L_inv[3][3];
	
	//L_inv
	L_inv[0][0] = -0.666666666666667;		L_inv[0][1] =  0.333333333333333;		L_inv[0][2] = 0.333333333333333;
	L_inv[1][0] =  0;						L_inv[1][1] = -0.577350269189626;		L_inv[1][2] = 0.577350269189626;
	L_inv[2][0] =  1.877934272300470;		L_inv[2][1] =  1.877934272300470;		L_inv[2][2] = 1.877934272300470;

	robot_zqd.X = 0;
	for(i=0;i<3;i++){
		robot_zqd.X += L_inv[0][i] * robot_zqd.w[i]*0.0001727875959474386f;
	}
	robot_zqd.X *= 1.123f;
	robot_zqd.Y = 0;
	for(i=0;i<3;i++){
		robot_zqd.Y += L_inv[1][i] * robot_zqd.w[i]*0.0001727875959474386f;
	}

	robot_zqd.W = 0;
	for(i=0;i<3;i++){
		robot_zqd.W += L_inv[2][i] * robot_zqd.v[i];
	}
	robot_zqd.theta += robot_zqd.W*0.01f;
	
	while(robot_zqd.theta<0)
	{
		robot_zqd.theta += 2*PI;
	}
	while(robot_zqd.theta > 2*PI)
	{
		robot_zqd.theta -= 2*PI;
	}
	*/
	
}

//�������ٶ�ת���ӵ�PWM
//vx���������x���ٶ�
//vy���������y���ٶ�
//w:������ԭ����ת�Ľ��ٶ�
//ע������ٶȣ�
void set_motor_vx_vy_w(float vx,float vy,float w)
{	
	u8 i,j,k;
	float L[3][3];
	float theta[3][3];
	float V[3][1];
	float tem[3][3];
					
	//v(PWM)=L*Theta*V
	//   cos(60)	sin(60)		-MOTOR_L
	//L= cos(180) 	sin(180)	-MOTOR_L
	//   cos(-60)	sin(-60)	-MOTOR_L
	L[0][0] =  0.5;					L[0][1] =  0.8660254037844386;		L[0][2] = -MOTOR_L;
	L[1][0] = -1;					L[1][1] =  0;						L[1][2] = -MOTOR_L;
	L[2][0] =  0.5;					L[2][1] = -0.8660254037844386;		L[2][2] = -MOTOR_L;
	//		 cos(theta)		sin(theta)	0
	//theta= -sin(theta)	cos(theta) 	0
	//		 0				0			1
	theta[0][0]= cos(robot_zqd.theta);	theta[0][1] = sin(robot_zqd.theta);	theta[0][2] = 0;
	theta[1][0]= -sin(robot_zqd.theta);	theta[1][1] = cos(robot_zqd.theta);	theta[1][2] = 0;
	theta[2][0]= 0;						theta[2][1] = 0;					theta[2][2] = 1;
	//V
	V[0][0] = -vx*10;
	V[1][0] = -vy;
	V[2][0] = -w;
	
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			tem[i][j] = 0;
			for(k=0;k<3;k++){
				tem[i][j] += L[i][k] * theta[k][j];
			}
		}
	}
	
	for(i=0;i<3;i++){
		robot_zqd.pwm[i] = 0;
		for(j=0;j<3;j++){
			robot_zqd.pwm[i] += tem[i][j]*V[j][0];
		}
	}
	
	/*
	for(i = 0;i<3;i++)
	{
		if(robot_zqd.pwm[i] > 700 || robot_zqd.pwm[i] < -700)
		{
			robot_zqd.pwm[0] = robot_zqd.pwm[0] * 700 /robot_zqd.pwm[i];
			robot_zqd.pwm[1] = robot_zqd.pwm[1] * 700 /robot_zqd.pwm[i];
			robot_zqd.pwm[2] = robot_zqd.pwm[2] * 700 /robot_zqd.pwm[i];
		}
	}
	*/
	
	
	LCD_Show_pwm();
}


//�������ٶ�ת���ӵ�PWM
//vx���������x���ٶ�
//vy���������y���ٶ�
//w:������ԭ����ת�Ľ��ٶ�
//ע������ٶȣ�
void set_motor_vx_vy_w_R(float vx,float vy,float w)
{	
	u8 i,j,k;
	float L[3][3];
	float theta[3][3];
	float V[3][1];
	float tem[3][3];
					
	//v(PWM)=L*Theta*V
	//L
	L[0][0] =  0.5;					L[0][1] =  8.660254037844386;		L[0][2] = -0.2013;
	L[1][0] = -1;					L[1][1] =  0;						L[1][2] = -0.2013;
	L[2][0] =  0.5;					L[2][1] = -8.660254037844386;		L[2][2] = -0.2013;
	//theta
	theta[0][0]= 1;						theta[0][1] = 0;					theta[0][2] = 0;
	theta[1][0]= 0;						theta[1][1] = 1;					theta[1][2] = 0;
	theta[2][0]= 0;						theta[2][1] = 0;					theta[2][2] = 1;
	//V
	V[0][0] = -vx*10; // ԭ�� -vx
	V[1][0] = -vy;
	V[2][0] = -w;
	
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			tem[i][j] = 0;
			for(k=0;k<3;k++){
				tem[i][j] += L[i][k] * theta[k][j];
			}
		}
	}
	
	for(i=0;i<3;i++){
		robot_zqd.pwm[i] = 0;
		for(j=0;j<3;j++){
			robot_zqd.pwm[i] += tem[i][j]*V[j][0];
		}
	}
	
	
	LCD_Show_pwm();
}


//��ȡ�½���λ��״̬
u8 xianwei_down(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1);
}

//��ȡ������λ��״̬
u8 xianwei_up(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0);
}

//����
int shot()
{
	if(xianwei_down()==1)
	{
		charge(0);
		delay_ms(10000);
		GPIO_SetBits(GPIOG,GPIO_Pin_7);
		delay_ms(10000);
		delay_ms(10000);
		delay_ms(10000);
		GPIO_ResetBits(GPIOG,GPIO_Pin_7);
		return 0;
	}
	else 
		return 1;
}

//��е���½�
void jixiebi_down(void)
{
	//ԭ�����ӣ�3000
	//V1.0��1750
	u16 i,t;
	u16 W=2700;
	u16 nms=2000;

	
	if(xianwei_down()==1)
	{
		TIM_SetCompare2(TIM9,MOTOR_STATIC_2);
		TIM_SetCompare1(TIM9,MOTOR_STATIC_1);
		return;
	}
	//EXTIX_Enable(1);
	#ifdef ZQD_DEBUG
	BEEP = 1;
	#endif
	TIM_SetCompare2(TIM9,W);  			//PE6
	TIM_SetCompare1(TIM9,MOTOR_STATIC_1);			//PE5
	LED1 = 1;
	for(i=0;i<nms;i++)
	{	  
		if(xianwei_down()==1)
		{	
			for(t=0;t<0xff;t++);
			if(xianwei_down()==1)
			{
				TIM_SetCompare2(TIM9,MOTOR_STATIC_2);
				TIM_SetCompare1(TIM9,3970);
				break;
			}
		}
		for(t=0;t<0x4fff;t++)
			if(xianwei_down() == 1)
				break;
	}
	TIM_SetCompare2(TIM9,MOTOR_STATIC_2);
	TIM_SetCompare1(TIM9,MOTOR_STATIC_1);

	#ifdef ZQD_DEBUG
	BEEP = 0;
	#endif
}

//��е������
void jixiebi_up(void)
{
	//ԭ�����ӣ�1960
	//V1.0:550
	u16 i,t;
	u16 W=2700;
	u16 nms=2000;

	
	if(xianwei_up()==1)
	{
		TIM_SetCompare2(TIM9,MOTOR_STATIC_2);
		TIM_SetCompare1(TIM9,MOTOR_STATIC_1);
		return ;
	}
	//EXTIX_Enable(0);
	#ifdef ZQD_DEBUG
	BEEP = 1;
	#endif
	TIM_SetCompare1(TIM9,W);		//PE6
	TIM_SetCompare2(TIM9,MOTOR_STATIC_2);		//PE5
	for(i=0;i<nms;i++)
	{
		if(xianwei_up() == 1)
		{
			for(t=0;t<0xff;t++);
			if(xianwei_up() == 1)
			{
				TIM_SetCompare1(TIM9,MOTOR_STATIC_1);
				TIM_SetCompare2(TIM9,MOTOR_STATIC_2);
				break;
			}
		}
		for(t=0;t<0x4fff;t++)
			if(xianwei_up() == 1)
				break;
	}
	TIM_SetCompare1(TIM9,MOTOR_STATIC_1);
	TIM_SetCompare2(TIM9,MOTOR_STATIC_2);	

	#ifdef ZQD_DEBUG
	BEEP = 0;
	#endif
}

//��е���½������䡢��������
int down_shot_up(void)
{	
	jixiebi_down();
	charge(0);
	delay_ms(30000);
	if(shot())
		return 1;
	delay_ms(45000);
	jixiebi_up();
	return 0;
}
int down_shot(void)
{
	jixiebi_down();
	charge(0);
	if(xianwei_down()==1)
	{
		charge(0);
		delay_ms(500);
		GPIO_SetBits(GPIOG,GPIO_Pin_7);
		delay_ms(500);
		GPIO_ResetBits(GPIOG,GPIO_Pin_7);
		return 0;
	}
	return 1;
	
}

//������ֱ��,�����ٶ�
//Y_I:ֱ�߾���
//V���ٶ�
//distance:������
//a_start:��ʼ���ٶ�
//a_stop:ֹͣ���ٶ�
void robot_straight_Y(float Y_I,float V,float distance,u32 a_start,u32 a_stop)
{
	u32 i;
	for(i=0;i<=(V/a_start);i++)
	{
		if(fabs(Y_I-robot_zqd.Y)<distance)
			break;
		if(Y_I-robot_zqd.Y>0)
		{
			robot_zqd.pwm[0] = -(float)i*a_start;
			robot_zqd.pwm[2] = (float)i*a_start;
			robot_zqd.pwm[1] = 0;
		}
		else if(Y_I-robot_zqd.Y<0)
		{
			robot_zqd.pwm[0] = (float)i*a_start;
			robot_zqd.pwm[2] = -(float)i*a_start;
			robot_zqd.pwm[1] = 0;
		}
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		delay_ms(1000);
	}
	LCD_Show_pwm();
	while(fabs(Y_I-robot_zqd.Y)>distance);
	for(;i>0;i--)
	{
		if(fabs(Y_I-robot_zqd.Y)<distance)
			break;
		if(Y_I-robot_zqd.Y>0)
		{
			robot_zqd.pwm[0] = -(float)i*a_stop;
			robot_zqd.pwm[2] = (float)i*a_stop;
			robot_zqd.pwm[1] = 0;
		}
		else if(Y_I-robot_zqd.Y<0)
		{
			robot_zqd.pwm[0] = (float)i*a_stop;
			robot_zqd.pwm[2] = -(float)i*a_stop;
			robot_zqd.pwm[1] = 0;
		}
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		delay_ms(100);
	}
	set_motor_vx_vy_w(0,0,0);
	control1_W(robot_zqd.pwm[0]);
	control2_W(robot_zqd.pwm[1]);
	control3_W(robot_zqd.pwm[2]);
	
}

//����ֱ�� ǰ��������ΪĿ���,����������Ϊ�м��
void robot_certain_point(float X_I,float Y_I,float Theta_I,float pointX, float pointY,float pointTheta) //
{
	float D_Theta,D_X,D_Y,Vw=0,sx=0,sy=0;
	float point_Theta,point_X,point_Y,point_Vw=0,point_sx=0,point_sy=0;
	
	point_Theta=pointTheta-robot_zqd.theta;
	point_X=pointX-robot_zqd.X;
	point_Y=pointY-robot_zqd.Y;
	
	D_Theta = Theta_I/180*PI - robot_zqd.theta;
	D_X = X_I - robot_zqd.X;
	D_Y = Y_I - robot_zqd.Y;

		while(fabs(point_Y) > 0.05f || fabs(point_X) > 0.05f)
	{
		if(point_Y > 0.05f)
		{
			
			if(point_Y >= 1.5f)
			{
				point_sy = 8;
				if(robot_zqd.Vy < 0.3f)
					point_sy = 2;
				else if(robot_zqd.Vy < 0.5f)
					point_sy = 4;
				else if(robot_zqd.Vy < 0.9f)
					point_sy = 6;
			}
			else
			{
				point_sy = 8;
			}
			if(point_Y < 1.5f){    
				point_sy = 6;
			}
			if(point_Y < 0.8f){    
				point_sy = 4;
			}
			if(point_Y < 0.3f){    
				point_sy = 2;
			}
			if(point_Y < 0.2f)		point_sy = 0.25;
			
		}
		else if(point_Y < -0.05f)
		{
			if(point_Y < -1.5f)
			{
				point_sy = -8;
				if(robot_zqd.Vy > -0.3f)
					point_sy = -2;
				else if(robot_zqd.Vy > -0.5f)
					point_sy = -4;
				else if(robot_zqd.Vy > -0.9f)
					point_sy = -6;
			}
			else
			{
				point_sy = -8;
			}
			if(point_Y > -1.5f){    
				point_sy = -6;
			}
			if(point_Y > -0.8f){    
				point_sy = -4;
			}
			if(point_Y > -0.3f){    
				point_sy = -2;
			}
			if(point_Y > -0.2f)	   point_sy = -0.25;
		}
		else 
			point_sy = 0;
		
		
		if(point_X > 0.05f)
		{
			if(point_X > 1.5f)
			{
				point_sx = 8;
				if(robot_zqd.Vx < 0.1f)
					point_sx = 0.5;
				else if(robot_zqd.Vx < 0.2f)
					point_sx = 1;
				else if(robot_zqd.Vx < 0.4f)
					point_sx = 2;
				else if(robot_zqd.Vx < 0.58f)
					point_sx = 4;
				else if(robot_zqd.Vx < 7.5f)
					point_sx = 6;
			}
			else if(point_X < 1.5f)
			{
				if(point_X > 0.2f)
				{
					point_sx = 2;
				}
				else if(point_X > 0.15f)
				{
					point_sx = 1;
				}
				else if(point_X > 0.1f)
				{
					point_sx = 0.5f;
				}
				else
					point_sx = 0.25f;
			}
		}
		else if(point_X < -0.05f)
		{
			if(point_X < -1.5f)
			{
				point_sx = -8;
				if(robot_zqd.Vx > -0.1f)
					point_sx = -0.5;
				else if(robot_zqd.Vx > -0.2f)
					point_sx = -1;
				else if(robot_zqd.Vx > -0.4f)
					point_sx = -2;
				else if(robot_zqd.Vx > -0.58f)
					point_sx = -4;
				else if(robot_zqd.Vx > -7.5f)
					point_sx = -6;
			}
			else if(point_X > -1.5f)
			{
				if(point_X < -0.2f)
				{
					point_sx = -2;
				}
				else if(point_X < -0.15f)
				{
					point_sx = -1;
				}
				else if(point_X < -0.1f)
				{
					point_sx = -0.5f;
				}
				else
					point_sx = -0.25f;
			}
		}
		else 
			point_sx = 0;
		
		
		if(point_Theta>0&&(point_Theta<PI))  
		{
			point_Vw=point_Theta*500;
		}
		else if(point_Theta>0&&(point_Theta>=PI)) 
		{
			point_Theta = 2*PI-point_Theta;
			point_Vw=-point_Theta*500;
		}
		else if(point_Theta<0&&(point_Theta>=-PI)) 
		{
			point_Theta = -point_Theta;
			point_Vw=-point_Theta*500;
		}
		else if(point_Theta<0&&(point_Theta<-PI)) 
		{
			point_Theta = 2*PI+point_Theta;
			point_Vw=point_Theta*500;
		}
		else 
			point_Vw=point_Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(point_Theta > 0.523599f)
		{
			if(point_Vw>0)
				point_Vw = 300;
			else
				point_Vw = -300;
		}
		
		//С��30�����5��	PWM40
		if(point_Theta < 0.523599f)
		{
			if(point_Vw>0)
				point_Vw = 150;
			else
				point_Vw = -150;
		}
		//С��5��	pwm8
		if(point_Theta < 0.0872665f)
		{
			if(point_Vw>0)
				point_Vw = 20;
			else
				point_Vw = -20;
		}
		
		
		
		
		set_motor_vx_vy_w(point_sx*12,point_sy*100,point_Vw);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		point_Theta = pointTheta/180*PI - robot_zqd.theta;
		point_X = pointX - robot_zqd.X;
		point_Y = pointY - robot_zqd.Y;
	}
		/*
		while(fabs(point_Y) > 0.05f || fabs(point_X) > 0.05f)
		{
			//���Ŀ�����y����x
		 if(fabs(point_Y)>fabs(point_X))
		 {
			 //y�᷽���ٶȲ�����Ϊ0
			if(point_Y > 0.05f)
			{
				if(point_Y < 0.2f){		
					if(point_sy<=0.25f)
					point_sy = 0.25f;
				}
				else if(point_Y < 0.3f){ 
					if(point_sy<=2)				
					point_sy = 2;
				}
				else if(point_Y < 0.8f){
					if(point_sy<=4)				
					point_sy = 4;
				}
				else if(point_Y < 1.5f){    
					if(point_sy<=6)
					point_sy = 6;
				}
				
				

				
				if(point_Y >= 1.5f)				
				{
					point_sy = 8;
					if(robot_zqd.Vy < 0.3f)
					{
						point_sy = 2;
					}
					else if(robot_zqd.Vy < 0.5f)
					{
						point_sy = 4;
					}
					else if(robot_zqd.Vy < 0.9f)
					{
						point_sy = 6;
					}

				}
				
			}
			else if(point_Y < -0.05f)
			{
				if(point_Y > -0.2f){	
					if(point_sy>=-0.25f)
						point_sy = -0.25f;
				}
				else if(point_Y > -0.3f){  
					if(point_sy>=-2)
						point_sy = -2;
				}
				else if(point_Y > -0.8f){ 
					if(point_sy>=-4)
						point_sy = -4;
				}
				else if(point_Y > -1.5f){ 
					if(point_sy>=-6)				
						point_sy = -6;
				}
				
				if(point_Y < -1.5f)
				{
					point_sy = -8;
					if(robot_zqd.Vy > -0.3f)
					{
						point_sy = -2;
					}
					else if(robot_zqd.Vy > -0.5f)
					{
						point_sy = -4;
					}
					else if(robot_zqd.Vy > -0.9f)
					{
						point_sy = -6;
					}
				}

				
				
				

			}else 
				point_sy = 0;
		
			
			if(point_X > 0.05f)
			{
				if(point_X > 1.5f)
				{
					point_sx = 8;
					if(robot_zqd.Vx < 0.1f)
						point_sx = 0.5;
					else if(robot_zqd.Vx < 0.2f)
						point_sx = 1;
					else if(robot_zqd.Vx < 0.4f)
						point_sx = 2;
					else if(robot_zqd.Vx < 0.58f)
						point_sx = 4;
					else if(robot_zqd.Vx < 7.5f)
						point_sx = 6;
				}
				else if(point_X < 1.5f)
				{
					if(point_X > 0.2f)
					{
						point_sx = 2;
					}
					else if(point_X > 0.15f)
					{
						point_sx = 1;
					}
					else if(point_X > 0.1f)
					{
						point_sx = 0.5f;
					}
					else
						point_sx = 0.25f;
				}
			}
			else if(point_X < -0.05f)
			{
				if(point_X < -1.5f)
				{
					point_sx = -8;
					if(robot_zqd.Vx > -0.1f)
						point_sx = -0.5;
					else if(robot_zqd.Vx > -0.2f)
						point_sx = -1;
					else if(robot_zqd.Vx > -0.4f)
						point_sx = -2;
					else if(robot_zqd.Vx > -0.58f)
						point_sx = -4;
					else if(robot_zqd.Vx > -7.5f)
						point_sx = -6;
				}
				else if(point_X > -1.5f)
				{
					if(point_X < -0.2f)
					{
						point_sx = -2;
					}
					else if(point_X < -0.15f)
					{
						point_sx = -1;
					}
					else if(point_X < -0.1f)
					{
						point_sx = -0.5f;
					}
					else
						point_sx = -0.25f;
				}
			}
			else 
				point_sx = 0;
		}
		 
		//���Ŀ�����x����y
		if(fabs(point_X)>fabs(point_Y))
		{
			if(point_Y > 0.05f)
			{
				if(point_Y >= 1.5f)
				{
					point_sy = 8;
					if(robot_zqd.Vy < 0.3f)
						point_sy = 2;
					else if(robot_zqd.Vy < 0.5f)
						point_sy = 4;
					else if(robot_zqd.Vy < 0.9f)
						point_sy = 6;
				}
				else
				{
					point_sy = 8;
				}
				if(point_Y < 1.5f){    
					point_sy = 6;
				}
				if(point_Y < 0.8f){    
					point_sy = 4;
				}
				if(point_Y < 0.3f){    
					point_sy = 2;
				}
				if(point_Y < 0.2f)		point_sy = 0.25;
				
			}
			else if(point_Y < -0.05f)
			{
				if(point_Y < -1.5f)
				{
					point_sy = -8;
					if(robot_zqd.Vy > -0.3f)
						point_sy = -2;
					else if(robot_zqd.Vy > -0.5f)
						point_sy = -4;
					else if(robot_zqd.Vy > -0.9f)
						point_sy = -6;
				}
				else
				{
					point_sy = -8;
				}
				if(point_Y > -1.5f){    
					point_sy = -6;
				}
				if(point_Y > -0.8f){    
					point_sy = -4;
				}
				if(point_Y > -0.3f){    
					point_sy = -2;
				}
				if(point_Y > -0.2f)	    point_sy = -0.25;
			}
			else 
				point_sy = 0;
			
			//x�����ٶȲ�����Ϊ0
			if(point_X > 0.05f)
			{
				if(point_X < 0.1f)
				{
					if(point_sx <= 0.25f)
						point_sx = 0.25f;
				}
				else if(point_X < 0.15f)
				{
					if(point_sx <= 0.5f)
						point_sx = 0.5f;
				}
				else if(point_X < 0.2f)
				{
					if(point_sx <= 1)
						point_sx = 1;
				}
				else if(point_X < 1.5f)
				{
					if(point_sx <= 2)
						point_sx = 2;
				}
				if(point_X >= 1.5f)
				{
					point_sx = 8;
					if(robot_zqd.Vx < 0.1f)
						point_sx = 0.5;
					else if(robot_zqd.Vx < 0.2f)
						point_sx = 1;
					else if(robot_zqd.Vx < 0.4f)
						point_sx = 2;
					else if(robot_zqd.Vx < 0.58f)
						point_sx = 4;
					else if(robot_zqd.Vx < 7.5f)
						point_sx = 6;
				}
			}
			else if(point_X < -0.05f)
			{
				if(point_X > -0.1f)
				{
					if(point_sx >= -0.25f)
						point_sx = -0.25f;
				}
				else if(point_X > -0.15f)
				{
					if(point_sx >= -0.5f)
						point_sx = -0.5f;
				}
				else if(point_X > -0.2f)
				{
					if(point_sx >= -1)
						point_sx = -1;
				}
				else if(point_X > -1.5f)
				{
					if(point_sx >= -2)
						point_sx = -2;
				}
				if(point_X < -1.5f)
				{
					point_sx = -8;
					if(robot_zqd.Vx > -0.1f)
						point_sx = -0.5;
					else if(robot_zqd.Vx > -0.2f)
						point_sx = -1;
					else if(robot_zqd.Vx > -0.4f)
						point_sx = -2;
					else if(robot_zqd.Vx > -0.58f)
						point_sx = -4;
					else if(robot_zqd.Vx > -7.5f)
						point_sx = -6;
				}
			}
			else 
				point_sx = 0;
		}
		
		
		if(point_Theta>0&&(point_Theta<PI))  
		{
			point_Vw=point_Theta*500;
		}
		else if(point_Theta>0&&(point_Theta>=PI)) 
		{
			point_Theta = 2*PI-point_Theta;
			point_Vw=-point_Theta*500;
		}
		else if(point_Theta<0&&(point_Theta>=-PI)) 
		{
			point_Theta = -point_Theta;
			point_Vw=-point_Theta*500;
		}
		else if(point_Theta<0&&(point_Theta<-PI)) 
		{
			point_Theta = 2*PI+point_Theta;
			point_Vw=point_Theta*500;
		}
		else 
			point_Vw=point_Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(point_Theta > 0.523599f)
		{
			if(point_Vw>0)
				point_Vw = 300;
			else
				point_Vw = -300;
		}
		
		//С��30�����5��	PWM40
		if(point_Theta < 0.523599f)
		{
			if(point_Vw>0)
				point_Vw = 150;
			else
				point_Vw = -150;
		}
		//С��5��	pwm8
		if(point_Theta < 0.0872665f)
		{
			if(point_Vw>0)
				point_Vw = 20;
			else
				point_Vw = -20;
		}
		
		
		set_motor_vx_vy_w(point_sx*12,point_sy*100,point_Vw);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		point_Theta = pointTheta/180*PI - robot_zqd.theta;
		point_X = pointX - robot_zqd.X;
		point_Y = pointY - robot_zqd.Y;
	}
	
	
	*/
	while(fabs(D_Y) > 0.05f || fabs(D_X) > 0.05f)
	{
		if(D_Y > 0.05f)
		{
			
			if(D_Y >= 1.5f)
			{
				sy = 8;
				if(robot_zqd.Vy < 0.3f)
					sy = 2;
				else if(robot_zqd.Vy < 0.5f)
					sy = 4;
				else if(robot_zqd.Vy < 0.9f)
					sy = 6;
			}
			else
			{
				sy = 8;
			}
			if(D_Y < 1.5f){    
				sy = 6;
			}
			if(D_Y < 0.8f){    
				sy = 4;
			}
			if(D_Y < 0.3f){    
				sy = 2;
			}
			if(D_Y < 0.2f)		sy = 0.25;
			
		}
		else if(D_Y < -0.05f)
		{
			if(D_Y < -1.5f)
			{
				sy = -8;
				if(robot_zqd.Vy > -0.3f)
					sy = -2;
				else if(robot_zqd.Vy > -0.5f)
					sy = -4;
				else if(robot_zqd.Vy > -0.9f)
					sy = -6;
			}
			else
			{
				sy = -8;
			}
			if(D_Y > -1.5f){    
				sy = -6;
			}
			if(D_Y > -0.8f){    
				sy = -4;
			}
			if(D_Y > -0.3f){    
				sy = -2;
			}
			if(D_Y > -0.2f)	    sy = -0.25;
		}
		else 
			sy = 0;
		
		
		if(D_X > 0.05f)
		{
			if(D_X > 1.5f)
			{
				sx = 8;
				if(robot_zqd.Vx < 0.1f)
					sx = 0.5;
				else if(robot_zqd.Vx < 0.2f)
					sx = 1;
				else if(robot_zqd.Vx < 0.4f)
					sx = 2;
				else if(robot_zqd.Vx < 0.58f)
					sx = 4;
				else if(robot_zqd.Vx < 7.5f)
					sx = 6;
			}
			else if(D_X < 1.5f)
			{
				if(D_X > 0.2f)
				{
					sx = 2;
				}
				else if(D_X > 0.15f)
				{
					sx = 1;
				}
				else if(D_X > 0.1f)
				{
					sx = 0.5f;
				}
				else
					sx = 0.25f;
			}
		}
		else if(D_X < -0.05f)
		{
			if(D_X < -1.5f)
			{
				sx = -8;
				if(robot_zqd.Vx > -0.1f)
					sx = -0.5;
				else if(robot_zqd.Vx > -0.2f)
					sx = -1;
				else if(robot_zqd.Vx > -0.4f)
					sx = -2;
				else if(robot_zqd.Vx > -0.58f)
					sx = -4;
				else if(robot_zqd.Vx > -7.5f)
					sx = -6;
			}
			else if(D_X > -1.5f)
			{
				if(D_X < -0.2f)
				{
					sx = -2;
				}
				else if(D_X < -0.15f)
				{
					sx = -1;
				}
				else if(D_X < -0.1f)
				{
					sx = -0.5f;
				}
				else
					sx = -0.25f;
			}
		}
		else 
			sx = 0;
		
		
		if(D_Theta>0&&(D_Theta<PI))  
		{
			Vw=D_Theta*500;
		}
		else if(D_Theta>0&&(D_Theta>=PI)) 
		{
			D_Theta = 2*PI-D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta>=-PI)) 
		{
			D_Theta = -D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta<-PI)) 
		{
			D_Theta = 2*PI+D_Theta;
			Vw=D_Theta*500;
		}
		else 
			Vw=Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(D_Theta > 0.523599f)
		{
			if(Vw>0)
				Vw = 300;
			else
				Vw = -300;
		}
		
		//С��30�����5��	PWM40
		if(D_Theta < 0.523599f)
		{
			if(Vw>0)
				Vw = 150;
			else
				Vw = -150;
		}
		//С��5��	pwm8
		if(D_Theta < 0.0872665f)
		{
			if(Vw>0)
				Vw = 20;
			else
				Vw = -20;
			
		}
		
		
		
		
		set_motor_vx_vy_w(sx*12,sy*100,Vw);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		D_Theta = Theta_I/180*PI - robot_zqd.theta;
		D_X = X_I - robot_zqd.X;
		D_Y = Y_I - robot_zqd.Y;
	}
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	delay_ms(1000);
	robot_turnOrigin_stage(Theta_I);
	
	
	
	
	
	
}

//ֱ������,���ݱ���
//X_I:Ŀ�������X
//Y_I:Ŀ�������Y
//Theta_I:Ŀ������ĽǶ�
void robot_straight_stage(float X_I,float Y_I,float Theta_I)
{
	float D_Theta,D_X,D_Y,Vw=0,sx,sy=0;
	D_Theta = Theta_I/180*PI - robot_zqd.theta;
	D_X = X_I - robot_zqd.X;
	D_Y = Y_I - robot_zqd.Y;
	
	while(fabs(D_Y) > 0.05f || fabs(D_X) > 0.05f)
	{
		if(D_Y > 0.05f)
		{
			
			if(D_Y >= 1.5f)
			{
				sy = 8;
				if(robot_zqd.Vy < 0.3f)
					sy = 2;
				else if(robot_zqd.Vy < 0.5f)
					sy = 4;
				else if(robot_zqd.Vy < 0.9f)
					sy = 6;
			}
			else
			{
				sy = 8;
			}
			if(D_Y < 1.5f){    
				sy = 6;
			}
			if(D_Y < 0.8f){    
				sy = 4;
			}
			if(D_Y < 0.3f){    
				sy = 2;
			}
			if(D_Y < 0.2f)		sy = 0.25;
			
		}
		else if(D_Y < -0.05f)
		{
			if(D_Y < -1.5f)
			{
				sy = -8;
				if(robot_zqd.Vy > -0.3f)
					sy = -2;
				else if(robot_zqd.Vy > -0.5f)
					sy = -4;
				else if(robot_zqd.Vy > -0.9f)
					sy = -6;
			}
			else
			{
				sy = -8;
			}
			if(D_Y > -1.5f){    
				sy = -6;
			}
			if(D_Y > -0.8f){    
				sy = -4;
			}
			if(D_Y > -0.3f){    
				sy = -2;
			}
			if(D_Y > -0.2f)	    sy = -0.25;
		}
		else 
			sy = 0;
		
		
		if(D_X > 0.05f)
		{
			if(D_X > 1.5f)
			{
				sx = 8;
				if(robot_zqd.Vx < 0.1f)
					sx = 0.5;
				else if(robot_zqd.Vx < 0.2f)
					sx = 1;
				else if(robot_zqd.Vx < 0.4f)
					sx = 2;
				else if(robot_zqd.Vx < 0.58f)
					sx = 4;
				else if(robot_zqd.Vx < 7.5f)
					sx = 6;
			}
			else if(D_X < 1.5f)
			{
				if(D_X > 0.2f)
				{
					sx = 2;
				}
				else if(D_X > 0.15f)
				{
					sx = 1;
				}
				else if(D_X > 0.1f)
				{
					sx = 0.5f;
				}
				else
					sx = 0.25f;
			}
		}
		else if(D_X < -0.05f)
		{
			if(D_X < -1.5f)
			{
				sx = -8;
				if(robot_zqd.Vx > -0.1f)
					sx = -0.5;
				else if(robot_zqd.Vx > -0.2f)
					sx = -1;
				else if(robot_zqd.Vx > -0.4f)
					sx = -2;
				else if(robot_zqd.Vx > -0.58f)
					sx = -4;
				else if(robot_zqd.Vx > -7.5f)
					sx = -6;
			}
			else if(D_X > -1.5f)
			{
				if(D_X < -0.2f)
				{
					sx = -2;
				}
				else if(D_X < -0.15f)
				{
					sx = -1;
				}
				else if(D_X < -0.1f)
				{
					sx = -0.5f;
				}
				else
					sx = -0.25f;
			}
		}
		else 
			sx = 0;
		
		
		if(D_Theta>0&&(D_Theta<PI))  
		{
			Vw=D_Theta*500;
		}
		else if(D_Theta>0&&(D_Theta>=PI)) 
		{
			D_Theta = 2*PI-D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta>=-PI)) 
		{
			D_Theta = -D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta<-PI)) 
		{
			D_Theta = 2*PI+D_Theta;
			Vw=D_Theta*500;
		}
		else 
			Vw=Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(D_Theta > 0.523599f)
		{
			if(Vw>0)
				Vw = 300;
			else
				Vw = -300;
		}
		
		//С��30�����5��	PWM40
		if(D_Theta < 0.523599f)
		{
			if(Vw>0)
				Vw = 150;
			else
				Vw = -150;
		}
		//С��5��	pwm8
		if(D_Theta < 0.0872665f)
		{
			if(Vw>0)
				Vw = 20;
			else
				Vw = -20;
		}
		
		
		
		
		set_motor_vx_vy_w(sx*12,sy*100,Vw);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		D_Theta = Theta_I/180*PI - robot_zqd.theta;
		D_X = X_I - robot_zqd.X;
		D_Y = Y_I - robot_zqd.Y;
	}
	control1_W(0);
	control2_W(0);
	control3_W(0);
	delay_ms(1000);
	robot_turnOrigin_stage(Theta_I);
}


//����ֱ��
//X_I:Ŀ�������X
//Y_I:Ŀ�������Y
//Theta_I:Ŀ������ĽǶ�
//ʹ��ʱע��������˶�������theta�Ǿ���һ��
void robot_straight_ObsAvoidance(float X_I,float Y_I,float Theta_I)
{
	float D_Theta,D_X,D_Y,Vw,sx,sy=0;
	float d_x,d_y,dis;
	float deg;
	D_Theta = Theta_I/180*PI - robot_zqd.theta;
	D_X = X_I - robot_zqd.X;
	D_Y = Y_I - robot_zqd.Y;
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	while(fabs(D_Y) > 0.05f || fabs(D_X) > 0.05f)
	{
		if(D_Y > 0.05f)
		{
			if(D_Y >= 1.5f)
			{
				sy = 8;
				if(robot_zqd.Vy < 0.3f)
					sy = 2;
				else if(robot_zqd.Vy < 0.5f)
					sy = 4;
				else if(robot_zqd.Vy < 0.9f)
					sy = 6;
			}
			else
			{
				sy = 8;
			}
			if(D_Y < 1.5f){    
				sy = 6;
			}
			if(D_Y < 0.8f){    
				sy = 4;
			}
			if(D_Y < 0.3f){    
				sy = 2;
			}
			if(D_Y < 0.2f)		sy = 0.25;
			
		}
		else if(D_Y < -0.05f)
		{
			if(D_Y < -1.5f)
			{
				sy = -8;
				if(robot_zqd.Vy > -0.3f)
					sy = -2;
				else if(robot_zqd.Vy > -0.5f)
					sy = -4;
				else if(robot_zqd.Vy > -0.9f)
					sy = -6;
			}
			else
			{
				sy = -8;
			}
			if(D_Y > -1.5f){    
				sy = -6;
			}
			if(D_Y > -0.8f){    
				sy = -4;
			}
			if(D_Y > -0.3f){    
				sy = -2;
			}
			if(D_Y > -0.2f)	    sy = -0.25;
		}
		else 
			sy = 0;
		
		
		if(D_X > 0.05f)
		{
			if(D_X > 1.5f)
			{
				sx = 8;
				if(robot_zqd.Vx < 0.1f)
					sx = 0.5;
				else if(robot_zqd.Vx < 0.2f)
					sx = 1;
				else if(robot_zqd.Vx < 0.4f)
					sx = 2;
				else if(robot_zqd.Vx < 0.58f)
					sx = 4;
				else if(robot_zqd.Vx < 7.5f)
					sx = 6;
			}
			else if(D_X < 1.5f)
			{
				if(D_X > 0.2f)
				{
					sx = 2;
				}
				else
					sx = 0.25f;
			}
		}
		else if(D_X < -0.05f)
		{
			if(D_X < -1.5f)
			{
				sx = -8;
				if(robot_zqd.Vx > -0.1f)
					sx = -0.5;
				else if(robot_zqd.Vx > -0.2f)
					sx = -1;
				else if(robot_zqd.Vx > -0.4f)
					sx = -2;
				else if(robot_zqd.Vx > -0.58f)
					sx = -4;
				else if(robot_zqd.Vx > -7.5f)
					sx = -6;
			}
			else if(D_X > -1.5f)
			{
				if(D_X < -0.2f)
				{
					sx = -2;
				}
				else
					sx = -0.25f;
			}
		}
		else 
			sx = 0;
		
		
		if(D_Theta>0&&(D_Theta<PI))  
		{
			Vw=D_Theta*500;
		}
		else if(D_Theta>0&&(D_Theta>=PI)) 
		{
			D_Theta = 2*PI-D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta>=-PI)) 
		{
			D_Theta = -D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta<-PI)) 
		{
			D_Theta = 2*PI+D_Theta;
			Vw=D_Theta*500;
		}
		else 
			Vw=Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(D_Theta > 0.523599f)
		{
			if(Vw>0)
				Vw = 300;
			else
				Vw = -300;
		}
		
		//С��30�����5��	PWM40
		if(D_Theta < 0.523599f)
		{
			if(Vw>0)
				Vw = 150;
			else
				Vw = -150;
		}
		//С��5��	pwm8
		if(D_Theta < 0.0872665f)
		{
			if(Vw>0)
				Vw = 20;
			else
				Vw = -20;
		}
		//���±���
		if(uart_getLaser())
		{
			if(uart3_data[1] < fabs(D_X)*1000 || uart3_data[1] < fabs(D_Y)*1000) // �˴����ַ��ؾ���С��Ŀ�����->�����ϰ�
			{
			
				deg = (uart3_data[0] - (float)MID_LASER) * PI /180; //�Ƕȱ任�ɻ�����
				if(deg < 0 )
					deg = -deg;
				if(uart3_data[1] < 2500 && uart3_data[1] * sin(deg) <400) //ԭ��4000  //׼������Ѱ�Һ��ʽǶ�
				{
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[0]);
					control3_W(robot_zqd.pwm[0]);
					
					dis = uart3_data[1] + 1000; //�״ﷵ�ؾ���+1.5��
					d_x = robot_zqd.X;    //��¼�µ�ǰ������ϵX
					d_y = robot_zqd.Y;
					D_X = d_x - robot_zqd.X;
					D_Y = d_y - robot_zqd.Y;
					while(1)
					{
						while(receive3 != 1);
		
						if(!uart_getLaser())
						{	
							set_motor_vx_vy_w_R(0,0,0);
							control1_W(robot_zqd.pwm[0]);
							control2_W(robot_zqd.pwm[1]);
							control3_W(robot_zqd.pwm[2]);
							//continue;
						}
						
						
					/*	
						if(fabs(robot_zqd.Vx) > 0.5f || robot_zqd.Vy > 0.5f)
						{
							set_motor_vx_vy_w_R(0,0,0);
							control1_W(robot_zqd.pwm[0]);
							control2_W(robot_zqd.pwm[1]);
							control3_W(robot_zqd.pwm[2]);
							delay_ms(30000);
							continue;
						}
					*/
						
						
						if(D_Y*1000 < -dis || D_Y*1000 > dis || D_X*1000 < -dis || D_X*1000 > dis) //���־������������Ѿ��ɹ�����
						{
							set_motor_vx_vy_w_R(0,0,0);
							control1_W(robot_zqd.pwm[0]);
							control2_W(robot_zqd.pwm[1]);
							control3_W(robot_zqd.pwm[2]);
							break;
						}
						
						deg = (uart3_data[0] - (float)MID_LASER) * PI /180;
						if(uart3_data[1] < 2500 && deg < 0 && uart3_data[1] * sin(deg) > -400)//����׼����������
						{
							set_motor_vx_vy_w_R(8,25,Vw); //ԭ��80 25 Vw
						}
						else if(uart3_data[1] < 2500 && deg > 0 && uart3_data[1] * sin(deg) <400)
						{
							set_motor_vx_vy_w_R(-8,25,Vw);//ԭ��-80 25 Vw
						}
						else
						{
							set_motor_vx_vy_w_R(0,25,Vw); //ֱ��
						}
						
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
						
						
						//�������
						D_Theta = Theta_I/180*PI - robot_zqd.theta;
						D_X = d_x - robot_zqd.X;
						D_Y = d_y - robot_zqd.Y;
						
						if(D_Theta>0&&(D_Theta<PI))  
						{
							Vw=D_Theta*500;
						}
						else if(D_Theta>0&&(D_Theta>=PI)) 
						{
							D_Theta = 2*PI-D_Theta;
							Vw=-D_Theta*500;
						}
						else if(D_Theta<0&&(D_Theta>=-PI)) 
						{
							D_Theta = -D_Theta;
							Vw=-D_Theta*500;
						}
						else if(D_Theta<0&&(D_Theta<-PI)) 
						{
							D_Theta = 2*PI+D_Theta;
							Vw=D_Theta*500;
						}
						else 
							Vw=Vw;
						
						//С��60�����30������
						//ʵ��PWMΪ201
						if(D_Theta > 0.523599f)
						{
							if(Vw>0)
								Vw = 300;
							else
								Vw = -300;
						}
						
						//С��30�����5��	PWM40
						if(D_Theta < 0.523599f)
						{
							if(Vw>0)
								Vw = 150;
							else
								Vw = -150;
						}
						//С��5��	pwm8
						if(D_Theta < 0.0872665f)
						{
							if(Vw>0)
								Vw = 20;
							else
								Vw = -20;
						}
					}
					sx = 0;
					sy = 0;
					Vw = 0;
				}
			}
		}
		
		set_motor_vx_vy_w(sx*12,sy*100,Vw);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
		D_Theta = Theta_I/180*PI - robot_zqd.theta;
		D_X = X_I - robot_zqd.X;
		D_Y = Y_I - robot_zqd.Y;
	}
	control1_W(0);
	control2_W(0);
	control3_W(0);
	delay_ms(1000);
	robot_turnOrigin_stage(Theta_I);
}



//ֱ������,����������
//X_I:Ŀ�������X
//Y_I:Ŀ�������Y
//V��ǰ�����ٶ�
//distance:���ֹͣ�ľ���,��λm
void robot_straight_I(float X_I,float Y_I,float Theta,float V,float W,float distance)
{
	float D_X,D_Y,D_Theta,T_Theta,V_Theta=0,Vx,Vy,Vw=0,length=0;
	T_Theta=Theta;
	Theta=Theta/180*PI;  //�������ڻ�����ϵ����һ��  ��̬��Thetaֵ  ������ϵx��������X_I�����������ϵx��������X_R�ļн�
	do{
		D_X=X_I-robot_zqd.X;
		D_Y=Y_I-robot_zqd.Y;
		D_Theta=Theta-robot_zqd.theta;	
		if(D_X==0){
			if(D_Y>0) 
				V_Theta=90.0/180.0*PI;
			else if(D_Y<0) 
				V_Theta=270.0/180.0*PI;
		}
		else if(D_Y==0){
			if(D_X>0) 
				V_Theta=0.0;
			else if(D_X<0) 
				V_Theta=PI;
		}
		else{
			if(D_Y>0&&D_X>0) 
				V_Theta=atan(D_Y/D_X);
			else if(D_Y>0&&D_X<0) 
				V_Theta=PI-atan(D_Y/-D_X);
			else if(D_Y<0&&D_X<0) 
				V_Theta=atan(D_Y/D_X)+PI;
			else if(D_Y<0&&D_X>0) 
				V_Theta=2*PI-atan(-D_Y/D_X);
		}
		length=sqrt(D_Y*D_Y+D_X*D_X);
		
		Vx=V*cos(V_Theta);
		Vy=V*sin(V_Theta);
		if(D_Theta>0&&fabs(D_Theta)<=PI)  
			Vw=W;
		else if(D_Theta>0&&fabs(D_Theta)>PI) 
			Vw=-W;
		else if(D_Theta<0&&fabs(D_Theta)<=PI) 
			Vw=-W;
		else if(D_Theta<0&&fabs(D_Theta)>PI) 
			Vw=W;
		else 
			Vw=0;
		
		set_motor_vx_vy_w(Vx,Vy,Vw);		

		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);	
		control3_W(robot_zqd.pwm[2]);
		length=sqrt(D_Y*D_Y+D_X*D_X);
	}while(length>distance);
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	while(robot_zqd.Vx!=0||robot_zqd.Vy!=0||robot_zqd.W!=0);
	delay_ms(10);
	robot_turnOrigin(400,T_Theta,10);
}

//�����˶�
//W�������ٶ� Theta Ŀ����̬�� dis��̬��ƫ��
void robot_turnOrigin(float W,float Theta,float dis)
{ 
	float D_Theta,Vw=0;        //W����0 ��ʱ��
	Theta=Theta/180.0f*PI;
	dis=dis/180.0f*PI;
	D_Theta=Theta-robot_zqd.theta;
	if(D_Theta>0&&fabs(D_Theta)<PI)  
		Vw=W;
	else if(D_Theta>0&&fabs(D_Theta)>=PI) 
		Vw=-W;
	else if(D_Theta<0&&fabs(D_Theta)<=PI) 
		Vw=-W;
	else if(D_Theta<0&&fabs(D_Theta)>PI) 
		Vw=W;
	else 
		Vw=Vw;
	
	if(fabs(D_Theta) > PI)
		D_Theta = fabs(D_Theta) - PI;
	
	while(fabs(D_Theta)>dis){
		set_motor_vx_vy_w(0,0,Vw);		
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);		
		
		D_Theta=Theta-robot_zqd.theta;	
		if(fabs(D_Theta) > PI)
			D_Theta = fabs(D_Theta) - PI;
	}
	control1_W(0);
	control2_W(0);		
	control3_W(0);
	robot_zqd.theta = Theta;
}

//�����˶����������Ƕȣ��Զ�����
void robot_turnOrigin_stage(float theta)
{
	float D_Theta,Vw=0;        //W����0 ��ʱ��
	theta=theta/180.0f*PI;
	D_Theta=theta-robot_zqd.theta;
	
	//����30�����Կ���
	if(D_Theta>0&&(D_Theta<PI))  
	{
		Vw=D_Theta*500;
	}
	else if(D_Theta>0&&(D_Theta>=PI)) 
	{
		D_Theta = 2*PI-D_Theta;
		Vw=-D_Theta*500;
	}
	else if(D_Theta<0&&(D_Theta>=-PI)) 
	{
		D_Theta = -D_Theta;
		Vw=-D_Theta*500;
	}
	else if(D_Theta<0&&(D_Theta<-PI)) 
	{
		D_Theta = 2*PI+D_Theta;
		Vw=D_Theta*500;
	}
	else 
		Vw=Vw;
	
	//С��60�����30������
	//ʵ��PWMΪ100
	if(D_Theta < 2.0944f)
	{
		if(Vw>0)
			Vw = 1000;
		else
			Vw = -1000;
	}
	
	//С��30�����5��
	if(D_Theta < 0.523599f)
	{
		if(Vw>0)
			Vw = 200;
		else
			Vw = -200;
	}
	//С��5��
	if(D_Theta < 0.0872665f)
	{
		if(Vw>0)
			Vw = 40;
		else
			Vw = -40;
	}
	
	
	while(D_Theta>0.015f)
	{
		set_motor_vx_vy_w(0,0,Vw);		
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);		
		
		D_Theta=theta-robot_zqd.theta;
		//����30�����Կ���
		if(D_Theta>0&&(D_Theta<PI))  
		{
			Vw=D_Theta*500;
		}
		else if(D_Theta>0&&(D_Theta>=PI)) 
		{
			D_Theta = 2*PI-D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta>=-PI)) 
		{
			D_Theta = -D_Theta;
			Vw=-D_Theta*500;
		}
		else if(D_Theta<0&&(D_Theta<-PI)) 
		{
			D_Theta = 2*PI+D_Theta;
			Vw=D_Theta*500;
		}
		else 
			Vw=Vw;
		
		//С��60�����30������
		//ʵ��PWMΪ201
		if(D_Theta < 2.0944f)
		{
			if(Vw>0)
				Vw = 1000;
			else
				Vw = -1000;
		}
		
		//С��30�����5��	PWM40
		if(D_Theta < 0.523599f)
		{
			if(Vw>0)
				Vw = 200;
			else
				Vw = -200;
		}
		//С��5��	pwm8
		if(D_Theta < 0.0872665f)
		{
			if(Vw>0)
				Vw = 40;
			else
				Vw = -40;
		}
	}
	control1_W(0);
	control2_W(0);		
	control3_W(0);
	set_motor_vx_vy_w(0,0,0);
	while(robot_zqd.W);
}

void getBasketball()
{
	USART_SendData(USART1, '1'); 
}


void getVolleyball()
{
	USART_SendData(USART1, '3'); 
}

u8 uart_getData(void)
{
	
	if(USART_RX_STA&0x8000)
	{					   
		//�õ��˴ν��յ������ݳ���
		//len=USART_RX_STA&0x3fff;

		/*
		//ԭ�����Ӿ�����
		//���������ص�λ�� 0-640
		if(USART_RX_BUF[0]!=' ')
			uart_data[0]=(USART_RX_BUF[0]-'0')*100;
		else 
			uart_data[0]=0;
		USART_RX_BUF[0] = ' ';
		if(USART_RX_BUF[1]!=' ')
			uart_data[0]+=(USART_RX_BUF[1]-'0')*10;
		USART_RX_BUF[1] = ' ';
		if(USART_RX_BUF[2]!=' ')
			uart_data[0]+=(USART_RX_BUF[2]-'0');
		USART_RX_BUF[2] = ' ';
		
		//�����Ϣ
		if(USART_RX_BUF[3]!=' ')
			uart_data[1]=(USART_RX_BUF[3]-'0')*1000;
		else 
			uart_data[1]=0;
		USART_RX_BUF[3] = ' ';
		if(USART_RX_BUF[4]!=' ')
			uart_data[1]+=(USART_RX_BUF[4]-'0')*100;
		USART_RX_BUF[4] = ' ';
		if(USART_RX_BUF[5]!=' ')
			uart_data[1]+=(USART_RX_BUF[5]-'0')*10;
		USART_RX_BUF[5] = ' ';
		if(USART_RX_BUF[6]!=' ')
			uart_data[1]+=(USART_RX_BUF[6]-'0');
		USART_RX_BUF[6] = ' ';
		
		//�뾶��Ϣ
		if(USART_RX_BUF[7]!=' ')
			uart_data[2]=(USART_RX_BUF[7]-'0')*100;
		else 
			uart_data[2]=0;
		USART_RX_BUF[7] = ' ';
		if(USART_RX_BUF[8]!=' ')
			uart_data[2]+=(USART_RX_BUF[8]-'0')*10;
		USART_RX_BUF[8] = ' ';
		if(USART_RX_BUF[9]!=' ')
			uart_data[2]+=(USART_RX_BUF[9]-'0');
		USART_RX_BUF[9] = ' ';
		*/
		
		//����λ����Ϣ
		if(USART_RX_BUF[0]!=' ')
			uart_data[0]=(USART_RX_BUF[0]-'0')*100;
		else 
			uart_data[0]=0;
		USART_RX_BUF[0] = ' ';
		if(USART_RX_BUF[1]!=' ')
			uart_data[0]+=(USART_RX_BUF[1]-'0')*10;
		USART_RX_BUF[1] = ' ';
		if(USART_RX_BUF[2]!=' ')
			uart_data[0]+=(USART_RX_BUF[2]-'0');
		USART_RX_BUF[2] = ' ';
		
		//�����Ϣ
		if(USART_RX_BUF[3]!=' ')
			uart_data[1]=(USART_RX_BUF[3]-'0')*1000;
		else 
			uart_data[1]=0;
		USART_RX_BUF[3] = ' ';
		if(USART_RX_BUF[4]!=' ')
			uart_data[1]+=(USART_RX_BUF[4]-'0')*100;
		USART_RX_BUF[4] = ' ';
		if(USART_RX_BUF[5]!=' ')
			uart_data[1]+=(USART_RX_BUF[5]-'0')*10;
		USART_RX_BUF[5] = ' ';
		if(USART_RX_BUF[6]!=' ')
			uart_data[1]+=(USART_RX_BUF[6]-'0');
		USART_RX_BUF[6] = ' ';

		LCD_ShowString(30+200,420,200,16,16,"View :pix");	
		LCD_ShowNum(30+200+48+8+45,420,uart_data[0],4,16);		
		LCD_ShowString(30+200,440,200,16,16,"View :length");	
		LCD_ShowNum(30+200+48+8+45,440,uart_data[1],4,16);	
		
		USART_RX_STA=0;
		receive=0;
	}
	
	if(uart_data[0]<10 || uart_data[0] >630)
		return 0;
	if(uart_data[1]<500)
		return 0;
	if(uart_data[2]==0)
		return 0;
	
	
	
	return 1;
}


//���⴦������
u8 uart_getLaser(void)
{
	
	if(USART3_RX_STA&0x8000)
	{					   
		//�õ��˴ν��յ������ݳ���
		//len=USART_RX_STA&0x3fff;

		
		//�������
		if(USART3_RX_BUF[0]!=' ')
			uart3_data[1]=(USART3_RX_BUF[0]-'0')*1000;
		else 
			uart3_data[1]=0;
		USART3_RX_BUF[0] = ' ';
		if(USART3_RX_BUF[1]!=' ')
			uart3_data[1]+=(USART3_RX_BUF[1]-'0')*100;
		USART3_RX_BUF[1] = ' ';
		if(USART3_RX_BUF[2]!=' ')
			uart3_data[1]+=(USART3_RX_BUF[2]-'0')*10;
		USART3_RX_BUF[2] = ' ';
		if(USART3_RX_BUF[3]!=' ')
			uart3_data[1] +=(USART3_RX_BUF[3]-'0');
		USART3_RX_BUF[3] = ' ';
		
		//����Ƕ�
		if(USART3_RX_BUF[4]!=' ')
			uart3_data[0]=(USART3_RX_BUF[4]-'0')*100;
		else 
			uart3_data[0]=0;
		USART3_RX_BUF[4] = ' ';
		if(USART3_RX_BUF[5]!=' ')
			uart3_data[0]+=(USART3_RX_BUF[5]-'0')*10;
		USART3_RX_BUF[5] = ' ';
		if(USART3_RX_BUF[6]!=' ')
			uart3_data[0]+=(USART3_RX_BUF[6]-'0');
		USART3_RX_BUF[6] = ' ';
		
		

		LCD_ShowString(30+200,460,200,16,16,"Radar:rad");	
		LCD_ShowNum(30+200+48+8+45,460,uart3_data[0],4,16);		
		LCD_ShowString(30+200,480,200,16,16,"Radar:length");	
		LCD_ShowNum(30+200+48+8+45,480,uart3_data[1],4,16);	
		
		USART3_RX_STA=0;
		receive3=0;
	}
	
	if(uart3_data[0]<240 || uart3_data[0] >300) //ԭ��&&
		return 0;
	if(uart3_data[1]>4000)
		return 0;
	
	
	return 1;
}


//�Ӿ�����
//����4������
void find_ball(u8 ball)
{
	float w=200;
	u8 time = 1;
	float theta = robot_zqd.theta,D_theta = 0;
	switch(ball)
	{
		case 1:
			getBasketball();
			delay_ms(10000);
			getBasketball();
			delay_ms(10000);
			getBasketball();
			break;
		case 3:
			getVolleyball();
			delay_ms(10000);
			getVolleyball();
			delay_ms(10000);
			getVolleyball();
			break;
	}
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	while(receive != 1);
	USART_RX_STA=0;
	receive=0;
	do{
		while(receive != 1);
	
		if(!uart_getData())
		{	
			if(time == 0)
			{
			}
			else if(time++ <5)
			{
				set_motor_vx_vy_w_R(0,0,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				continue;
			}
			else if(time !=0)
				time = 0;
		}
		else
			time = 1;
		
		if(time == 0)
		{
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -200;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 200;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LCD_Show_pwm();
		}
		else if(uart_data[1] > 4000)
		{
			set_motor_vx_vy_w(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0]< MID_VIEW-30) && uart_data[1]>1300)
		{
			set_motor_vx_vy_w_R(-5,10,0);  //ԭ�� -50 10 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0] > MID_VIEW+30) && uart_data[1] > 1300)
		{
			set_motor_vx_vy_w_R(5,10,0);
			control1_W(robot_zqd.pwm[0]);  //ԭ��50 10 0
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1] > 1300)
		{
			set_motor_vx_vy_w_R(0,20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0]< MID_VIEW-20) && uart_data[1] > 700)
		{
			set_motor_vx_vy_w_R(-4,0,0);  //ԭ��-40 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0] > MID_VIEW+20) && (uart_data[1] >700))
		{
			set_motor_vx_vy_w_R(4,0,0); //ԭ��4 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1]>700)
		{
			set_motor_vx_vy_w_R(0,12,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0]< MID_VIEW-30)
		{
			set_motor_vx_vy_w_R(-3,0,0); //ԭ��-30 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] > MID_VIEW+30)
		{
			set_motor_vx_vy_w_R(3,0,0); //ԭ��30 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] <= MID_VIEW+30 && uart_data[0] > MID_VIEW+10)
		{
			set_motor_vx_vy_w_R(1.5,0,0); //ԭ��15 0 0 
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] >= MID_VIEW-30 && uart_data[0] < MID_VIEW -10)
		{
			set_motor_vx_vy_w_R(-1.5,0,0); //ԭ��-15 0 0 
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else 
		{
				control1_W(0);
				control2_W(0);
				control3_W(0);
				jixiebi_down();
				set_motor_vx_vy_w_R(0,7,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				break;
			
		}
	}while(1);
	get_hongwai();
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	control1_W(0);
	control2_W(0);
	control3_W(0);
	jixiebi_up();
	LCD_Show_pwm();
	
}

//�Ӿ����״�������
//����4m����
void find_ball_sanfen(u8 ball)
{
	
	float w=200;
	u8 time = 1;
	float theta = robot_zqd.theta,D_theta = 0;
	switch(ball)
	{
		case 1:
			getBasketball();
			delay_ms(10000);
			getBasketball();
			delay_ms(10000);
			getBasketball();
			break;
		case 3:
			getVolleyball();
		
			delay_ms(10000);
		/*
			getVolleyball();
			delay_ms(10000);
			getVolleyball();
		*/
			break;


	}
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//����Ӿ���������
	while(receive != 1);
	USART_RX_STA=0;
	receive=0;
	
	//��մ��ڽ������ݻ���   
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;

	
	do{
		while(receive != 1);
		while(receive3 != 1);
		
		if(!uart_getData())
		{	
			if(time == 0)
			{
			}
			else if(time++ <5)
			{
				set_motor_vx_vy_w_R(0 ,0,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				continue;
			}
			else if(time !=0)
				time = 0;
		}
		else
			time = 1;
		
		uart_getLaser();
		
		if(time == 0)
		{
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -200;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 200;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LCD_Show_pwm();
		}
		else if(uart_data[1]>4000)
		{
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0]< MID_VIEW-30) && uart_data[1]>1000)
		{
			set_motor_vx_vy_w_R(-8,16,0); //ԭ��-5 10 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart_data[0] > MID_VIEW+30) && uart_data[1] > 1000)
		{
			set_motor_vx_vy_w_R(8,16,0);//ԭ�� 5 10 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1] > 1000)
		{
			set_motor_vx_vy_w_R(0,30,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else
			break;
	}while(1);
	

			

			if(uart_data[1]>800)
				{
				LCD_ShowString(30+200,500,200,16,16,"View!");
				//�״����ݲ�ͬʱ�������Ӿ�����Ѱ��
					while(1)
					{
					while(receive != 1);
					uart_getData();
					uart_getLaser();
					if(uart_data[1]<800)
						break;

					if(!uart_getData())
					{	
						set_motor_vx_vy_w_R(0,0,0);
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					//	continue;
					}
					if(uart_data[0]< MID_VIEW-20 )
					{
						set_motor_vx_vy_w_R(-6,0,0); //ԭ��-4 0 0
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else if(uart_data[0] > MID_VIEW+20)
					{
						set_motor_vx_vy_w_R(6,0,0);//ԭ��4 0 0
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					
					else if(uart_data[1]>700)
					{
						set_motor_vx_vy_w_R(0,20,0);
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					
					else if(uart_data[0]< MID_VIEW-30)
					{
						set_motor_vx_vy_w_R(-6,0,0); //ԭ��-3 0 0
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else if(uart_data[0] > MID_VIEW+30)
					{
						set_motor_vx_vy_w_R(6,0,0);//ԭ�� 3 0 0
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else if(uart_data[0] <= MID_VIEW+30 && uart_data[0] > MID_VIEW+10)
					{
						set_motor_vx_vy_w_R(3,0,0); //ԭ�� 1.5 0 0 
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else if(uart_data[0] >= MID_VIEW-30 && uart_data[0] < MID_VIEW -10)
					{
						set_motor_vx_vy_w_R(-3,0,0);//ԭ��-1.5 0 0
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else 
					{
							control1_W(0);
							control2_W(0);
							control3_W(0);
							jixiebi_down();
							set_motor_vx_vy_w_R(0,7,0);
							control1_W(robot_zqd.pwm[0]);
							control2_W(robot_zqd.pwm[1]);
							control3_W(robot_zqd.pwm[2]);
							break;
					}
				}
			}
			
		

	if(uart_data[1]<=800)
			{	
				LCD_ShowString(30+200,500,200,16,16,"Radar!");
					//�״�������ͬ�������״�����Ѱ��
				while(1)
				{
					while(receive3 != 1);
					uart_getLaser();
					uart_getData();

					if(!uart_getLaser())
					{	
					set_motor_vx_vy_w_R(0,0,0);
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
					
					else if((uart3_data[0]< MID_LASER-10) && uart3_data[1] >700)
					{
					set_motor_vx_vy_w_R(-10,0,0);//ԭ��-10 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
					else if((uart3_data[0] > MID_LASER+10) && uart3_data[1] >700)
					{
					set_motor_vx_vy_w_R(10,0,0); //ԭ��10 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
					else if(uart3_data[1]>700)
					{
					set_motor_vx_vy_w_R(0,20,0); //ԭ�� 0 14 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
					else if(uart3_data[0]< MID_LASER-5)
					{
					set_motor_vx_vy_w_R(-10,0,0);//ԭ��-10 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
					else if(uart3_data[0] > MID_LASER+5)
					{
					set_motor_vx_vy_w_R(10,0,0);//ԭ��10 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					}
				/*	else if(uart3_data[0] <= MID_LASER+5 && uart3_data[0] > MID_LASER+2)
					{
						set_motor_vx_vy_w_R(1,0,0);
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					else if(uart3_data[0] >= MID_LASER-5 && uart3_data[0] < MID_LASER -2)
					{
						set_motor_vx_vy_w_R(-1,0,0);
						control1_W(robot_zqd.pwm[0]);
						control2_W(robot_zqd.pwm[1]);
						control3_W(robot_zqd.pwm[2]);
					}
					*/
					else 
					{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					jixiebi_down();
					set_motor_vx_vy_w_R(0,7,0);//ԭ��0 7 0 
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					if(uart3_data[1] < 350)
						break;
					}	
				
				}
			}

			
	
			
	get_hongwai();
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	control1_W(0);
	control2_W(0);
	control3_W(0);
	jixiebi_up();
	//control1_W(500);
	//control3_W(500);
	//delay_ms(30000);
	
	
	LCD_Show_pwm();
	LCD_ShowString(30+200,460,200,16,16,"       ");
}

//���ü�������
//����������ɫ
//����3m����
void find_ball_laser(void)
{
	float w=300;
	float theta = robot_zqd.theta,D_theta = 0;	
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		//	continue;
		}
		LED0 = !LED0;

		if(uart3_data[1] < 10)
			continue;
		if(uart3_data[1] > 3000)
		{			
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -300;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 300;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-15)
		{
			set_motor_vx_vy_w_R(0,0,200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+15)
		{
			set_motor_vx_vy_w_R(0,0,-200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1] > 1000)
		{
			set_motor_vx_vy_w_R(0,30,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0]< MID_LASER-10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0] > MID_LASER+10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,-150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1]>700)
		{
			set_motor_vx_vy_w_R(0,20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-5)
		{
			set_motor_vx_vy_w_R(-8,0,0); //ԭ��-80 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+5)
		{
			set_motor_vx_vy_w_R(8,0,0);//ԭ��80 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else 
		{
			control1_W(0);
			control2_W(0);
			control3_W(0);
			jixiebi_down();
			set_motor_vx_vy_w_R(0,7,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			if(uart3_data[1] < 350)
				break;
		}
	}while(1);
	get_hongwai();	
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	control1_W(0);
	control2_W(0);
	control3_W(0);
	jixiebi_up();
	LCD_Show_pwm();
	
}

//��Ȧ������������
//�����Ұ��û������Զ���ת
//find_ball_laser()�Ľ�
//����3m����
void find_ball_zhongquan(void)
{
	
	float w=300;
	float theta = robot_zqd.theta,D_theta = 0;

	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 3000)
		{			
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -300;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 300;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-15)
		{
			set_motor_vx_vy_w_R(0,0,200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+15)
		{
			set_motor_vx_vy_w_R(0,0,-200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1] > 1000 )
		{
			set_motor_vx_vy_w_R(0,17,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0]< MID_LASER-10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0] > MID_LASER+10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,-150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1]>700)
		{
			set_motor_vx_vy_w_R(0,14,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-5)
		{
			set_motor_vx_vy_w_R(-8,0,0);//ԭ��-80 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+5)
		{
			set_motor_vx_vy_w_R(8,0,0);//ԭ��80 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] <= MID_LASER+5 && uart3_data[0] > MID_LASER+2)
		{
			set_motor_vx_vy_w_R(3,0,0);//ԭ��30 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] >= MID_LASER-5 && uart3_data[0] < MID_LASER -2)
		{
			set_motor_vx_vy_w_R(-3,0,0);//ԭ��-30 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else 
		{
			control1_W(0);
			control2_W(0);
			control3_W(0);
			jixiebi_down();
			set_motor_vx_vy_w_R(0,7,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			if(uart3_data[1] < 350)
				break;
		}
	}while(1);
	get_hongwai();
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	control1_W(0);
	control2_W(0);
	control3_W(0);
	jixiebi_up();
	LCD_Show_pwm();

	
}


void panduan_weizhi(void)
{
	//int i=0;
	zhongquan_case=10;

	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1500)
		{			
			zhongquan_case=0;
			set_motor_vx_vy_w(0,400,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			delay_ms(40000);
			break;
		}
		else 
		{
			if(changdi==1)
				robot_straight_stage(robot_zqd.X+2,robot_zqd.Y+0.7f,40);
			else if(changdi==2)
				robot_straight_stage(robot_zqd.X-2,robot_zqd.Y+0.7f,320);
			break;

		}
	}while(1);
}

void panduan_weizhi2(void)
{
	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1700)
		{			
			zhongquan_case=1;
			if(changdi==1)
				robot_straight_stage(robot_zqd.X-1.2f,robot_zqd.Y+1.2f,45);//�ҳ� 
			else if(changdi==2)
				robot_straight_stage(robot_zqd.X+0.5f,robot_zqd.Y+1.2f,315);//��
			break;
		}
		else 
		{
			zhongquan_case=2;
			if(changdi==1)
			{
				robot_certain_point(1.2f,6.6f,90,2.4f,6.85f,90);
			}
			else if(changdi==2)
			{
				robot_certain_point(-1.2f,6.6f,270,-2.4f,6.85f,270);
			}
			break;
		}
	}while(1);
}

void panduan_weizhifan(void)
{
	//int i=0;
	zhongquan_case=10;
	

	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1500 || uart3_data[0]<MID_LASER-25 || uart3_data[0] > MID_LASER+25)
		{			
			if(uart3_data[1]>2800)
			{
			zhongquan_case=2;
			if(changdi==1)
				set_motor_vx_vy_w(-40,0,0);//�ҳ�
			else if(changdi==2)
				set_motor_vx_vy_w(40,0,0);//��
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			delay_ms(40000);
			break;
			}
			
			else{	
			zhongquan_case=2;
			if(changdi==1)
				set_motor_vx_vy_w(-40,0,0);//�ҳ�
			else if(changdi==2)
				set_motor_vx_vy_w(40,0,0);//��
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			delay_ms(20000);
			break;
			}
		}
		else 
		{
			if(changdi==1)
				robot_straight_stage(robot_zqd.X-0.7f,robot_zqd.Y-1.8f,45); //�ҳ�
			else if(changdi==2)
				robot_straight_stage(robot_zqd.X+1.2f,robot_zqd.Y-2.2f,315);//��
			break;

		}
	}while(1);
}
void panduan_weizhifan2(void)
{
		//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1700)
		{			
			zhongquan_case=1;
			/*
			if(changdi==1)				
				robot_straight_stage(robot_zqd.X-0.5f,robot_zqd.Y+0.5f,45);//�ҳ�
			else if(changdi==2)
				robot_straight_stage(robot_zqd.X+0.5f,robot_zqd.Y+0.5f,315);//��
			*/
			break;
		}
		else 
		{
			zhongquan_case=0;
			robot_straight_stage(0,4.2f,0);
			break;
		}
	}while(1);		
}
//��Ȧ������������
//�����Ұ��û������Զ���ת
//find_ball_laser()�Ľ�
//����3m����
u8 find_ball_dixian(void)
{
	
	float w=300;
	float theta = robot_zqd.theta,D_theta = 0;
	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 3000)
		{			
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -300;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 300;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-15)
		{
			set_motor_vx_vy_w_R(0,0,200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+15)
		{
			set_motor_vx_vy_w_R(0,0,-200);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1] > 1000 )
		{
			set_motor_vx_vy_w_R(0,17,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0]< MID_LASER-10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0] > MID_LASER+10) && uart3_data[1] >700)
		{
			set_motor_vx_vy_w_R(0,0,-150);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1]>700)
		{
			set_motor_vx_vy_w_R(0,14,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0]< MID_LASER-5)
		{
			set_motor_vx_vy_w_R(-8,0,0);//ԭ��-80 0 0 
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] > MID_LASER+5)
		{
			set_motor_vx_vy_w_R(8,0,0);//ԭ��80 0 0 
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] <= MID_LASER+5 && uart3_data[0] > MID_LASER+2)
		{
			set_motor_vx_vy_w_R(3,0,0);//ԭ�� 30 0 0 
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[0] >= MID_LASER-5 && uart3_data[0] < MID_LASER -2)
		{
			set_motor_vx_vy_w_R(-3,0,0);//ԭ��-30 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(robot_zqd.Y < 0.9f)
		{
			robot_zqd.pwm[0] = 0;
			robot_zqd.pwm[1] = 0;
			robot_zqd.pwm[2] = 0;
			control1_W(0);
			control2_W(0);
			control3_W(0);
			return 1;
		}
		else 
		{
			control1_W(0);
			control2_W(0);
			control3_W(0);
			jixiebi_down();
			set_motor_vx_vy_w_R(0,7,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			if(uart3_data[1] < 350)
				break;
		}
	}while(1);
	get_hongwai();
	//get_hongwai_dixian(0.9f);
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	control1_W(0);
	control2_W(0);
	control3_W(0);
	jixiebi_up();
	LCD_Show_pwm();
	return 0;

	
}


void find_lankuang(void)
{ 
	u16 lankuang_mid=266;
	if(lankuang_state==0)
		lankuang_state=1;
	else if(lankuang_state==1)
		lankuang_state=2;

	/*
	//���״������𣬱���
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0 ,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			continue;
		}
		LED0 = !LED0;
		
		if(uart_data[1] > (DIS_LASER+500))
		{
			set_motor_vx_vy_w_R(0,40,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1] <(DIS_LASER-500))
		{
			set_motor_vx_vy_w_R(0,-40,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] > MID_LASER+10 )
		{
			set_motor_vx_vy_w_R(-100,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] < MID_LASER - 10)
		{
			set_motor_vx_vy_w_R(100,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		if(uart_data[1]>(DIS_LASER+100))
		{
			set_motor_vx_vy_w_R(0,20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1]<(DIS_LASER-100))
		{
			set_motor_vx_vy_w_R(0,-20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0]> MID_LASER + 2)
		{
			set_motor_vx_vy_w_R(50,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] < MID_LASER - 2 )
		{
			set_motor_vx_vy_w_R(-50,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		
		else
		{
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			break;
		}
	}while(1);
	*/
	/*
	float w=200;
	u8 time = 1;
	float theta = robot_zqd.theta,D_theta = 0;
	*/
	
	//get_dingweizhu();
	//delay_ms(10000);
	//get_dingweizhu();
	//delay_ms(10000);
	//get_dingweizhu();
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//����Ӿ���������
	//while(receive != 1);
	//USART_RX_STA=0;
	//receive=0;
	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	/*
	do{
	//	while(receive != 1);
		while(receive3 != 1);
	
		if(!uart_getData())
		{	
			if(time == 0)
			{
			}
			else if(time++ <5)
			{
				set_motor_vx_vy_w_R(0 ,0,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				continue;
			}
			else if(time !=0)
				time = 0;
		}
		else
			time = 1;
		uart_getLaser();
		
		if(time == 0)
		{
			D_theta = robot_zqd.theta - theta;
			if((D_theta > PI/6.0f && D_theta < PI) || (D_theta < -PI && D_theta > -PI*11.0f/6.0f))
			{
				w = -200;
			}
			if((D_theta < -PI/6.0f && D_theta > -PI) || (D_theta > PI && D_theta < PI*11.0f/6.0f))
			{
				w = 200;
			}
			set_motor_vx_vy_w(0,0,w);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1] > DIS_VIEW + 25)
		{
			set_motor_vx_vy_w_R(0,20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[1] < DIS_VIEW -25)
		{
			set_motor_vx_vy_w_R(0,-20,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0]< MID_VIEW-20)
		{
			set_motor_vx_vy_w_R(-5,0,0);//ԭ��-50 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart_data[0] > MID_VIEW+20)
		{
			set_motor_vx_vy_w_R(5,0,0);//ԭ��50 0 0
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else
		{
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			break;
		}
	}while(1);
		*/

	
	//��ȷ��׼
	if(uart3_data[1]<3500)
	{
		
		if(uart3_data[0] > lankuang_mid-9 && uart3_data[0] < lankuang_mid+9)
		{

			LCD_ShowString(30+200,460,200,16,16,"Radar!");
			//�״�������ͬ�������״�����Ѱ��
			while(1)
		 {
			if(time_out_flag==1)
				break;
			else
			{			
			
				while(receive3 != 1);
				if(!uart_getLaser())
				{	
					set_motor_vx_vy_w_R(0,0,0);
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
					//continue;
				}
				if((uart3_data[0]< lankuang_mid-10))
				{
					set_motor_vx_vy_w_R(-10,0,0);//ԭ��-100 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if((uart3_data[0] > lankuang_mid+10))
				{
					set_motor_vx_vy_w_R(10,0,0);//ԭ��100 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[1]>DIS_LASER + 100)
				{
					set_motor_vx_vy_w_R(0,20,0);
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[1] < DIS_LASER - 100)
				{
					set_motor_vx_vy_w_R(0,-20,0);
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[0]< lankuang_mid-5)
				{
					set_motor_vx_vy_w_R(-5,0,0);//ԭ��-50 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[0] > lankuang_mid+5)
				{
					set_motor_vx_vy_w_R(5,0,0);//ԭ��50 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[0] <= lankuang_mid+5 && uart3_data[0] > lankuang_mid+2)
				{
					set_motor_vx_vy_w_R(3,0,0);//ԭ��30 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else if(uart3_data[0] >= lankuang_mid-5 && uart3_data[0] < lankuang_mid -2)
				{
					set_motor_vx_vy_w_R(-3,0,0);//ԭ��-30 0 0
					control1_W(robot_zqd.pwm[0]);
					control2_W(robot_zqd.pwm[1]);
					control3_W(robot_zqd.pwm[2]);
				}
				else 
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					break;
				}
			 }
			}
		}
	}
			
			
	/*
	else
	{
		LCD_ShowString(30+200,460,200,16,16,"View!");
		//�״����ݲ�ͬʱ�������Ӿ�����Ѱ��
		while(1)
		{
			if(time_out_flag==1)
				break;
			else
		 {
			while(receive != 1);
			if(!uart_getData())
			{	
				set_motor_vx_vy_w_R(0,0,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				continue;
			}
			if(uart_data[0]< MID_VIEW-20)
			{
				set_motor_vx_vy_w_R(-5,0,0);//ԭ��-50 0 0
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[0] > MID_VIEW+20)
			{
				set_motor_vx_vy_w_R(5,0,0);//ԭ��50 0 0
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[1]>DIS_VIEW+10)
			{
				set_motor_vx_vy_w_R(0,15,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[1]<DIS_VIEW -10)
			{
				set_motor_vx_vy_w_R(0,-15,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[0]< MID_VIEW-30)
			{
				set_motor_vx_vy_w_R(-4.5,0,0);//ԭ��-45 0 0
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[0] > MID_VIEW+30)
			{
				set_motor_vx_vy_w_R(4.5,0,0);//ԭ��45 0 0 
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[0] <= MID_VIEW+30 && uart_data[0] > MID_VIEW+10)
			{
				set_motor_vx_vy_w_R(3,0,0);//ԭ��30 0 0
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else if(uart_data[0] >= MID_VIEW - 30 && uart_data[0] < MID_VIEW -10)
			{
				set_motor_vx_vy_w_R(-3,0,0);//ԭ��-30 0 0
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
			}
			else 
			{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					break;
			}
		 }
	 }
	
	}
	*/
		
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	//��ȷ��׼
	while(1)
	{
		if(time_out_flag==1)
			break;
		else
		{
		while(receive3 != 1);
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		//	continue;
		}
		if((uart3_data[0]< lankuang_mid-1))
		{
			set_motor_vx_vy_w_R(0,0,100);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if((uart3_data[0] > lankuang_mid+1))
		{
			set_motor_vx_vy_w_R(0,0,-100);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		//���뾫�ȵ�����50mm
		else if(uart3_data[1]>DIS_LASER + 10)
		{
			set_motor_vx_vy_w_R(0,7,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else if(uart3_data[1] < DIS_LASER - 10)
		{
			set_motor_vx_vy_w_R(0,-7,0);
			
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
		}
		else 
		{
			control1_W(0);
			control2_W(0);
			control3_W(0);
			break;
		}
	}
		
	}
		
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	
	
	LCD_Show_pwm();
	LCD_ShowString(30+200,460,200,16,16,"       ");
	
}

void go_back(void)
{ 
	u8 i=0;
	float dis;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//����Ӿ���������
	//while(receive != 1);
	//USART_RX_STA=0;
	//receive=0;
	
	//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;

	
	//��ȷ��׼
	if(uart3_data[1]<3500)
	{

		if(uart3_data[0] > MID_LASER-9 && uart3_data[0] < MID_LASER+9)
		{

			LCD_ShowString(30+200,460,200,16,16,"Radar!");
			//�״�������ͬ�������״�����Ѱ��
			while(1)
		 {
			{			
				
				while(receive3 != 1);
				i++;
				if(i>3)
					break;
				uart_getLaser();				
				control1_W(0);
				control2_W(0);
				control3_W(0);				
			 }
			}
		 dis=(float)uart3_data[1];
		 dis=dis/1000;
		 robot_straight_stage(robot_zqd.X,robot_zqd.Y-dis-0.5f,180);
			
		}
	}
		
	robot_zqd.pwm[0] = 0;
	robot_zqd.pwm[1] = 0;
	robot_zqd.pwm[2] = 0;
	
	
	LCD_Show_pwm();
	LCD_ShowString(30+200,460,200,16,16,"       ");
	
}

void remote_control(void)
{	
	int16_t vx=0,vy=0,w=0;
	u8 key;
	vx = 0;
	vy = 0;
	w = 0;
	while(1)
	{
		key = Remote_Scan();
		if(key == KEY_UP)
		{
			vy += 3;
		}
		else if(key == KEY_DOWN)
		{
			vy -= 3;
		}
		else if(key == KEY_LEFT)
		{
			vx -= 20;
		}
		else if(key == KEY_RIGHT)
		{
			vx += 20;
		}
		else if(key == KEY_VOL_A)
		{
			w += 40;
		}
		else if(key == KEY_VOL_D)
		{
			w -= 40;
		}
		else if(key == KEY_PLAY)
		{
			control_init();
		}
		else if(key == KEY_POWER)
		{
			control1_W(0);
			control2_W(0);
			control3_W(0);
			set_motor_vx_vy_w_R(0,0,0);
			break;
		}
		set_motor_vx_vy_w_R(vx,vy,w);
		control1_W(robot_zqd.pwm[0]);
		control2_W(robot_zqd.pwm[1]);
		control3_W(robot_zqd.pwm[2]);
	}
}


//�����翪�س�ʼ��
void charge_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��

  //GPIOG_6��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_6);//GPIOG6
}


//�����翪��״̬����
void charge(u8 state)
{
	if(state)
		GPIO_SetBits(GPIOG,GPIO_Pin_6);
	else
		GPIO_ResetBits(GPIOG,GPIO_Pin_6);
}

//��Ȧֱ�е�
void zhongquanpoint(u8 zhongquan)
{
	if(zhongquan==0)
	{
		if(changdi==1)
			robot_straight_stage(0,4.2,290);//right side
		else if(changdi==2)
			robot_straight_stage(0,4.2,0);//left side
	}
	
	if(zhongquan==1)
	{
		if(changdi==1)
		{
		robot_straight_stage(robot_zqd.X+1,robot_zqd.Y-1,290);
		//robot_straight_stage(2.2f,4.6721f,225);//right side
		}
		else if(changdi==2)
		{
		robot_straight_stage(robot_zqd.X-1,robot_zqd.Y-1,90);
		//robot_straight_stage(-2.2f,4.6721f,315);//left side
		}
	}
	
	if(zhongquan==2)
	{
		//robot_certain_point(1.8f,7,90,2,4.2f,0);//right side
		if(changdi==1)
		{
			robot_straight_stage(robot_zqd.X,robot_zqd.Y+0.7f,270);
			robot_straight_stage(1.8f,6.85f,290);
		}
		else if(changdi==2)
		{
			robot_straight_stage(-robot_zqd.X,robot_zqd.Y+0.7f,90);
			robot_straight_stage(-1.65f,6.85f,90);
		}
		//robot_certain_point(-2.3f,7,270,2,4.2f,0);//left side
		//robot_straight_stage(-1.8,7,270);
	}
}

void zhongquanpointfan(u8 zhongquan)
{
	if(zhongquan==0)
	{
		if(changdi==1)
		{
			robot_certain_point(4,4.2f,270,0,4.2f,0);
			//robot_straight_stage(0,4.2f,0);
			//robot_straight_stage(4,robot_zqd.Y,270);
		}
		else if(changdi==2)
		{
			//robot_certain_point(-4,4.2f,90,0,4.2f,0);
			robot_straight_stage(0,-4.2,0);//left side
			//robot_straight_stage(-4,robot_zqd.Y,90);
		}
	
	}
	
	if(zhongquan==1)
	{
		if(changdi==1)
		{
		//robot_certain_point(4.5f,5.6721f,270,2.2f,4.6721f,315);
		robot_straight_stage(2.2f,4.6721f,315);
		//robot_straight_stage(4.5f,robot_zqd.Y+1,270);
		}
		else if(changdi==2)
		{
		//robot_certain_point(-4.5f,5.6721f,90,-2.2f,4.6721f,45);
		robot_straight_stage(-2.2f,4.6721f,45);
		//robot_straight_stage(-4.5f,robot_zqd.Y+1,90);			
		}
	}
	
	if(zhongquan==2)
	{
		if(changdi==1)
			robot_straight_stage(1.8f,6.7f,90);
		else if(changdi==2)
			robot_straight_stage(-1.8f,7,270);
	}
}

// ����Ŀ��㣬��Ȧ��������
void sanfenpoint(u8 sanfen,u8 zhongquan)
{
	if(sanfen==0)
		{
			if(zhongquan==0)
			{
				robot_certain_point(4.0f,7,270,0,4.2f,270);//right side
				//robot_certain_point(-4.0f,7,90,0,4.2f,90);//left side
			}
			
		if(zhongquan==1)
			{					
				robot_certain_point(4.0f,7,270,2.2f,4.6721f,270);//right side
				//robot_certain_point(-4.0f,7,90,-2.2f,4.6721f,90);//left side
			}
		
		if(zhongquan==2)
			{
				robot_certain_point(4.0f,7,270,2.3f,7,280);//right side
				//robot_certain_point(-4.0f,7,90,2.0f,7,90);//left side
			}
		else
				robot_straight_stage(4.0f,7,270);//right side 
			//robot_straight_stage(-4.0f,7,90);//left side 

		}
	
	if(sanfen==1)
	{
		if(zhongquan==0)
			{
				if(changdi==1)
					robot_certain_point(5.21f,2.3f,315,0,4.2f,315);//right side
				else if(changdi==2)
					robot_certain_point(-4.4f,2.2f,45,0,4.2f,45);//left side
			}
			
		if(zhongquan==1)
			{
				if(changdi==1)
					robot_certain_point(5.21f,2.3f,315,1.6f,4.1f,225);//right side
				else if(changdi==2)
					robot_certain_point(-4.4f,2.2f,45,-1.5f,4,135);//left side
			}
		
		if(zhongquan==2)
			{	
				if(changdi==1)
				{
					robot_straight_stage(2.5f,7,315);
					robot_certain_point(5.21f,2.3f,315,2.5f,3.5f,315);//right side
				}
				if(changdi==2)
				{
					robot_straight_stage(-2.5f,7,45);
					robot_certain_point(-4.5f,2,45,-2.5f,7,45);//left side
				}
			}

	}
	
	if(sanfen==2)
	{
		if(zhongquan==0)
			{
				robot_certain_point(7.25f,1.4f,0,0,3.5f,0);//right side
				//robot_certain_point(-7.25f,1.4f,0,0,3.5f,0);//left side
			}
			
		if(zhongquan==1)
			{
				robot_certain_point(7.25f,1.4f,0,2.2f,4.6721f,0);//right side
				//robot_certain_point(-7.25f,1.4f,0,-2.2f,4.6721f,0);//left side
			}
		
		if(zhongquan==2)
			{
				robot_certain_point(7.25f,1.4f,0,2.5f,7,0);//right side 
				//robot_certain_point(-7.25f,1.4f,0,-2.5f,7,0);//left side 
			}
	}
}

void panduan_weizhisanfen()
{
	sanfen_case=10;
		//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1500)
		{			
			sanfen_case=0;
			robot_straight_stage(robot_zqd.X+2,robot_zqd.Y,270);
			break;
		}
		else 
		{
			sanfen_case=1;
			robot_certain_point(7,2,90,3.5,2,270);
		}
	}while(1);
	
}


void panduan_weizhisanfenfan()
{
	sanfen_case=10;
		//��մ��ڽ������ݻ���
	receive3 = 0;
	USART3_RX_STA = 0;
	
	control1_W(0);
	control2_W(0);
	control3_W(0);
	LCD_Show_pwm();
	
	//��ֹ��Ч����
	while(receive3 != 1);
	receive3 = 0;
	USART3_RX_STA = 0;
	
	do{
		while(receive3 != 1);
		
		if(!uart_getLaser())
		{	
			set_motor_vx_vy_w_R(0,0,0);
			control1_W(robot_zqd.pwm[0]);
			control2_W(robot_zqd.pwm[1]);
			control3_W(robot_zqd.pwm[2]);
			LED1 = !LED1;
			//continue;
		}
		LED0 = !LED0;
		
		if(uart3_data[1] < 10)
			continue;
		
		if(uart3_data[1] > 1500)
		{			
			sanfen_case=1;
			robot_straight_stage(robot_zqd.X,robot_zqd.Y+2,270);
			break;
		}
		else 
		{
			sanfen_case=0;
			robot_certain_point(4,6.5f,270,3.5,2,270);
		}
	}while(1);
	
}
