#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "HCTL2020.h"
#include "timer.h"
#include "lcd.h"
#include "control.h"
#include "pwm.h"
#include "led.h"
//#include "key.h"
#include "beep.h"
#include "remote.h"
#include "exti.h"
	
u8 zhongquan_case;
u8 changdi;
u8 chengxu;
u8 sanfen_case;



int main(void)
{
	u8 key = 0;					//����ֵ
	//u8 chengxu = 0;				//����ѡ��
	u8 flag=0;
	u8 qiu = 0;				//����
	int16_t time = 0;			//��ʱ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����ϵͳ�ж����ȼ�����2   2λ��ռ���� 2λ��Ӧ����
	zhongquan_case=10;
	changdi=1;					//���ҳ�
	chengxu=0;
	delay_init(168);  			//��ʼ����ʱ����
	uart_init(9600);	 		//���ڳ�ʼ��Ϊ9600
	initall_PWM();				//��ʼ��PWM������
 	LED_Init();			    	 //LED�˿ڳ�ʼ��
	LCD_Init();					//LCD��ʼ��
	LCD_Show_Title();			//Һ������ʾ���ݳ�ʼ��
	//KEY_Init();					//������ʼ��
	hctl2020_init();			//��ʼ��������
	Remote_Init();				//����ң�س�ʼ��
	charge_init();				//�����翪�س�ʼ��
	shot_init();				//�����ʼ��
	xianwei_init();				//��λ���س�ʼ��
	BEEP_Init();
	hongwai_init();				//���⿪�س�ʼ��
	//EXTIX_Init();


	//TIM_SetCompare2(TIM5,280);	//��HCTL2020�ṩʱ���źţ�PA0
	TIM_SetCompare2(TIM5,7);
	control_init();				//�����˳�ʼ��
	TIM2_Int_Init(100-1,8400-1);//��ʱ��ȡ��������ʱ��0.01f
	EXTIX_Init();	
	

	while(1)
	{
		key = Remote_Scan();
		flag = 0;
		key = 0;
		chengxu = 0;
		//ѡ�����
		while(1)
		{
			LCD_ShowString(30+200,400,200,16,16,"chengxu:");
			key = Remote_Scan();
			//key = KEY_Scan(0);
			switch(key)
			{
				case 0:		//û�а�������
					
					break;
				case KEY_RIGHT:		//��
					LCD_ShowString(30+200,400,200,16,16,"qiu:    ");
					flag = 1;
					break;
				case KEY_DOWN:		//��
					LCD_ShowString(30+200,400,200,16,16,"chengxu-");
					if(chengxu != 0)
						chengxu--;
					break;
				case KEY_LEFT:		//��
					LCD_ShowString(30+200,400,200,16,16,"clear   ");
					chengxu = 0;
					break;
				case KEY_UP:		//��
					LCD_ShowString(30+200,400,200,16,16,"chengxu+");
					chengxu++;
					break;
			}
			
			LCD_ShowNum(30+200+48+8+10,320,chengxu,4,16);
			
			if(flag)
				break;
		}
		
		flag = 0;
		key = 0;
		
		//ѡ����
		while(1)
		{
			key = Remote_Scan();
			//key = KEY_Scan(0);
			switch(key)
			{
				case 0:		//û�а�������
					
					break;
				case KEY_RIGHT:		//��
					LCD_ShowString(30+200,400,200,16,16,"changdi   ");
					flag = 1;
					break;
				case KEY_DOWN:		//��
					LCD_ShowString(30+200,400,200,16,16,"qiu-");
					if(qiu != 0)
						qiu--;
					break;
				case KEY_LEFT:		//��
					LCD_ShowString(30+200,400,200,16,16,"clear   ");
					qiu = 0;
					break;
				case KEY_UP:		//��
					LCD_ShowString(30+200,400,200,16,16,"qiu+");
					qiu++;
					break;

			}
			
			LCD_ShowNum(30+200+48+8+10,340,qiu,4,16);
			
			if(flag)
				break;
		}
		
		flag = 0;
		key = 0;
				while(1)
		{
			key = Remote_Scan();
			switch(key)
			{
				case 0:		//û�а�������
					break;
				case KEY_RIGHT:		//��
					LCD_ShowString(30+200,400,200,16,16,"start   ");
					flag = 1;
					break;
				case KEY_DOWN:		//��
					LCD_ShowString(30+200,400,200,16,16,"changdi-");
					if(changdi != 0)
						changdi--;
					break;
				case KEY_LEFT:		//��
					LCD_ShowString(30+200,400,200,16,16,"clear   ");
					changdi=0;
					break;
				case KEY_UP:		//��
					LCD_ShowString(30+200,400,200,16,16,"changdi+");
					changdi++;
					break;
				case KEY_POWER:
					flag = 1;
					chengxu = 99;
					break;
			}
			
			LCD_ShowNum(30+200+48+8+10,360,changdi,4,16);
			
			if(flag)
				break;
		}
												
		//ÿ�γ���ʼǰ��ʼ��λ����Ϣ
		control_init();						//�����˳�ʼ��
		
		
		switch(chengxu)
		{
			case 0:	    	//���Գ���
				switch(qiu){
					case 0:
						robot_turnOrigin_stage(180);
						//˳ʱ��180��
						break;
					case 1:
						//��е���½�
						jixiebi_down();
						LED0 = !LED0;
						break;
					case 2:
						//��е������
						jixiebi_up();
						LED0 = !LED0;
						break;
					case 3:
						//�������
						get_hongwai();
						LED0 = !LED0;
						break;
					case 4:
						//�������
						while(1)
						{
							time = 0;
							key = Remote_Scan();
							switch(key)
							{
								case KEY_1:
									charge(1);
									LED0 = 0;
									break;
								case KEY_2:
									charge(0);
									LED0 = 1;
									break;
								case KEY_3:
									charge(0);
									delay_ms(30000);
									GPIO_SetBits(GPIOG,GPIO_Pin_7);
									LED1 = 0;
									delay_ms(10000);
									delay_ms(10000);
									delay_ms(10000);
									GPIO_ResetBits(GPIOG,GPIO_Pin_7);
									LED1 = 1;
									break;
								case KEY_4:
									TIM_SetCompare1(TIM9,1950);		//PE6
									TIM_SetCompare2(TIM9,3990);		//PE5
									break;
								case KEY_5:
									TIM_SetCompare1(TIM9,3990);		//PE6
									TIM_SetCompare2(TIM9,3000);		//PE5
									break;
								case KEY_6:
									TIM_SetCompare1(TIM9,3990);		//PE6
									TIM_SetCompare2(TIM9,3990);		//PE5
								case KEY_7:
									time = 1;
									break;
							}
							if(time)
								break;
						}
						break;
					case 5:
						//�Ӿ�����
						//find_ball(qiu);
						find_ball(3);
					
						break;
					case 6:
						//�������
						find_ball_laser();
						break;
					case 7:
						//ֱ������
						robot_straight_stage(0,1,0);
						//robot_straight_stage(1.0f,2.5f,0);
						//robot_straight_stage(2.5f,3.5f,0);
						break;
					case 8:
						//���������
						find_lankuang();					
						break;
					case 9:
					for(time = 0 ;time <23;time++)
					{
						control1_W(0);
						control2_W(0);
						control3_W(0);
						delay_ms(30000);
					}
					BEEP=1;
						//TIM7_Int_Init(10000,8400*5-1); // ѹ��Ͷ�� ������ 120s��ʱ
						//remote_control();
						break;
				}		
				
				break;
			case 1:
				//�����һ�غ�
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//             ��     
				//��ʱ10s
				changdi=1;
				for(time = 0 ;time <23;time++)
					{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
					}
				control_init();
				charge(1);
				if(changdi==1)
					robot_straight_stage(0,5.35f,290);					//�ҳ�
				else if(changdi==2)
					robot_straight_stage(0,5.35f,70);					//��
				if(down_shot_up())
					break;
				charge(1);
				robot_straight_stage(0,4.5f,0);
				find_ball_laser();
				if(robot_zqd.X>0)
				{
					if(changdi==1)
						robot_turnOrigin_stage(280);									//�ҳ�
					else if(changdi==2)
						robot_turnOrigin_stage(90);										//��
				}
				else
				{
					if(changdi==1)					
						robot_turnOrigin_stage(290);							//�ҳ�
					else if(changdi==2)
						robot_turnOrigin_stage(70);									//��
				}					
				if(down_shot_up())
					break;				
				if(changdi==1)
					robot_straight_stage(-0.1f,-0.2f,0);									//�ҳ�
				else if(changdi==2)
					robot_straight_stage(0,0,0);		 			//��
				break;
			case 2:
				//����ڶ��غ�
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//             ��     
			changdi=1;	
			for(time = 0 ;time <23;time++)
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
				}
				charge(1);
				robot_straight_stage(0,3.5f,0);
				panduan_weizhi();
				if(zhongquan_case!=0)
					panduan_weizhi2();
				find_ball_sanfen(qiu); // 1������ 3������
				if(changdi==1)
					robot_turnOrigin_stage(280);										//�ҳ� 
				else if(changdi==2)
					robot_turnOrigin_stage(80);											//��
				if(down_shot_up())
					break;
				charge(1);
				if(zhongquan_case==1)
				{	
					if(changdi==1)
						robot_turnOrigin_stage(225);			//�ҳ�
					else if(changdi==2)
						robot_turnOrigin_stage(135);      //�� 
				}
				sanfenpoint(1,zhongquan_case);
				find_ball_sanfen(qiu);
				if(robot_zqd.X>6.4f)
					robot_straight_stage(robot_zqd.X,robot_zqd.Y-1.2f,0);//�˺�һ�� 
				else
					robot_straight_stage(robot_zqd.X,robot_zqd.Y-1.2f,356);
					
				if(down_shot_up())
					break;
				robot_zqd.theta_offset = -0.05f;
				robot_straight_stage(0,robot_zqd.Y,180);
				go_back();
				//robot_turnOrigin_stage(0);
				//robot_straight_stage(0.3f,-0.35f,0);
				break;
			case 3:
				//��������غ� 
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//                  ��     
			changdi=1;	
			for(time = 0 ;time <23;time++)
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
				}
				charge(1);
				set_motor_vx_vy_w(0,400,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				delay_ms(40000);				
				if(changdi==1)
					robot_straight_stage(5.21f,1.5f,315);									//�ҳ�
				else if(changdi==2)
					robot_straight_stage(5.1f,1.5f,45);								//��					
				find_ball_sanfen(qiu);
				robot_straight_stage(robot_zqd.X,robot_zqd.Y-1,0);
				if(down_shot_up())
					break;
				charge(1);
				robot_straight_stage(robot_zqd.X,robot_zqd.Y+1.6f,0);
				if(changdi==1)
				{	
						robot_straight_stage(11.3f,robot_zqd.Y,180);										//�ҳ�
						robot_straight_stage(robot_zqd.X,2.5f,180);
						//robot_straight_stage(11.3f,robot_zqd.Y-0.6f,180);
				}
				
					
				else if(changdi==2)
					robot_straight_stage(-11.3f,2.7f,180);									//�� 				                   
				find_ball_dixian();			//�״�����							
				if(changdi==1)
				{
					robot_straight_stage(robot_zqd.X,robot_zqd.Y-0.2f,180); //�ҳ�
					robot_straight_stage(robot_zqd.X-4.5f,robot_zqd.Y,0);
				}
				else if(changdi==2)
				robot_straight_stage(robot_zqd.X,robot_zqd.Y-0.5f,335); //��			
				if(down_shot_up())
					break;
				robot_straight_stage(0,robot_zqd.Y,180);
				go_back();
				//robot_turnOrigin_stage(0);
				//robot_straight_stage(0.3f,-0.30f,0);
				break;
			case 4:
				//Ͷ����һ�غ�
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//             ��     
				changdi=1;
				TIM7_Int_Init(10000,8400*5-1); // ѹ��Ͷ�� 120s��ʱ
				for(time = 0 ;time <23;time++)
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
				}
				charge(1);
				if(changdi==1)				
					robot_straight_stage(8.6f,7.25f,290);										//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-7.8f,9,90);										//��					
				find_lankuang();
				if(down_shot_up())
					break;
				charge(1);
				if(changdi==1)
					robot_straight_stage(2.5f,7,90);         //�ҳ�
				else if(changdi==2)
					robot_straight_stage(-2.5f,7,290); 		//��
				panduan_weizhifan();
				if(zhongquan_case!=2)
					panduan_weizhifan2();
				if(zhongquan_case==2)
				{
					if(changdi==1)
						robot_straight_stage(robot_zqd.X-0.8f,robot_zqd.Y,105);//�ҳ�
					else if(changdi==2)
						robot_straight_stage(robot_zqd.X+0.8f,robot_zqd.Y,225);//��
				}
				find_ball_sanfen(qiu);
				zhongquanpointfan(zhongquan_case);
				if(changdi==1)
					robot_straight_stage(8.6f,7.25f,290);									//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-7.8f,9,90);									//��
				find_lankuang(); 
				if(down_shot_up())
					break;
				lankuang_state=3;
				//while(1);
				robot_straight_stage(0,2,0);
				//robot_straight_stage(0.1,0,0);										//��
				robot_straight_stage(-0.1,0,0);									//�ҳ�
				break;
			case 5:
				//Ͷ���ڶ��غ�
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//             ��     
				
				changdi=1;
				TIM7_Int_Init(10000,8400*5-1); // ѹ��Ͷ�� ������ 120s��ʱ
				for(time = 0 ;time <23;time++)
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
				}				
				charge(1);
				if(changdi==1)
					robot_straight_stage(5.21f,1.5f,315);         //�ҳ�
				else if(changdi==2)
					robot_straight_stage(-4.5f,2.2f,45);			//��
				find_ball_sanfen(qiu);					//�Ӿ�����
				if(changdi==1)
					robot_straight_stage(robot_zqd.X+0.5f,robot_zqd.Y+0.2f,0);
				if(changdi==2)
					robot_straight_stage(robot_zqd.X-0.5f,robot_zqd.Y+0.2f,0);
				if(changdi==1)
					robot_straight_stage(8.25f,7.5f,285);											//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-7,7.5f,90);										//��
				find_lankuang();
				if(down_shot_up())
					break;
				charge(1);
				robot_straight_stage(2.5f,6.7f,90);
				panduan_weizhifan();
				if(zhongquan_case!=2)
					panduan_weizhifan2();
				if(zhongquan_case==2)
				{
					if(changdi==1)
						robot_straight_stage(robot_zqd.X-0.8f,robot_zqd.Y,105);//�ҳ�
					else if(changdi==2)
						robot_straight_stage(robot_zqd.X+0.8f,robot_zqd.Y,225);//��
				}
				find_ball_sanfen(qiu);
				zhongquanpointfan(zhongquan_case);
				if(changdi==1)
				{
					if(zhongquan_case==1||zhongquan_case==0)
						robot_certain_point(8.6f,7.25f,290,4,6,90);
					if(zhongquan_case==2)
						robot_straight_stage(8.6f,7.25f,290);									//�ҳ�
				}
				else if(changdi==2)
					robot_straight_stage(-7.8f,9,90);									//��
				find_lankuang(); 
				if(down_shot_up())
					break;
				lankuang_state=3;
				//while(1);
				robot_straight_stage(robot_zqd.X,2,0);
				robot_straight_stage(0,2,0);
				robot_straight_stage(0,0,0);
				break;
			case 6:
				//Ͷ�������غ�
				// --------------- --------------
				//|     |         |       |      |
				//|  -            |         -    |
				//|-              |            - |
				// --------------- --------------
				//                  ��     
				changdi=1;
				TIM7_Int_Init(10000,8400*5-1); // ѹ��Ͷ�� ������ 120s��ʱ
				for(time = 0 ;time <23;time++)
				{
					control1_W(0);
					control2_W(0);
					control3_W(0);
					delay_ms(30000);
				}
				charge(1);
				set_motor_vx_vy_w(0,400,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				delay_ms(20000);
				if(changdi==1)
					robot_straight_stage(5.3f,1.5f,315);         //�ҳ�
				else if(changdi==2)
					robot_straight_stage(-4.5f,2.2f,45);			//��
				find_ball_sanfen(qiu);					//�Ӿ�����
				if(changdi==1)
					robot_straight_stage(robot_zqd.X+0.5f,robot_zqd.Y+0.2f,0);
				if(changdi==2)
					robot_straight_stage(robot_zqd.X-0.5f,robot_zqd.Y+0.2f,0);
				if(changdi==1)
					robot_straight_stage(8.25f,7.5f,285);											//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-7,7.5f,90);										//��
				find_lankuang();
				if(down_shot_up())
					break;
				charge(1);
				if(changdi==1)				
					robot_straight_stage(11.3f,2.6f,180);										//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-11.3f,3.2f,180);									//��
				find_ball_dixian();			//�״�����
				if(changdi==1)
					robot_straight_stage(8.25f,7.5f,285);												//�ҳ�
				else if(changdi==2)
					robot_straight_stage(-7,7.5f,90);											//��
				find_lankuang();
				if(down_shot_up())
					break;
				lankuang_state=3;
				//while(1);
				robot_straight_stage(0,3,0);
				delay_ms(30000);
				robot_straight_stage(0,-0.2,0);
				break;
			case 7:
				//�Ӿ��״����
				find_ball_sanfen(1);
				break;
			case 8:
					robot_straight_stage(2.6f,7,90);
					panduan_weizhifan();
					find_ball_sanfen(qiu);
			
			case 9:
				//�����غϲ��Գ���
				charge(1);
				set_motor_vx_vy_w(0,400,0);
				control1_W(robot_zqd.pwm[0]);
				control2_W(robot_zqd.pwm[1]);
				control3_W(robot_zqd.pwm[2]);
				delay_ms(20000);
				//robot_straight_stage(-3.9,2,45);										//��
				robot_straight_stage(3.9,2,315);										//�ҳ�
				delay_ms(30000);
				//find_ball_zhongquan( );			//�״�����
				//find_ball(qiu);				//�Ӿ�����
				delay_ms(30000);
				robot_straight_stage(robot_zqd.X,robot_zqd.Y-1,0);
				delay_ms(30000);
				//if(down_shot_up())
				//	break;
				delay_ms(30000);
				charge(1);
				//robot_straight_stage(-11,3,180);									//��
				robot_straight_stage(11,3,180);										//�ҳ�
				delay_ms(30000);
				//find_ball_dixian();			//�״�����
				//if(qiu == 1)					//�Ӿ�����
				//	qiu = 2;
				//else
				//	qiu = 1;
				//find_ball(qiu);
				delay_ms(30000);
				//robot_turnOrigin_stage(330);											//��
				robot_turnOrigin_stage(30);												//�ҳ�
				delay_ms(30000);
				//if(down_shot_up())
				//	break;
				delay_ms(30000);
				robot_zqd.theta_offset = -0.05f;
				robot_straight_stage(0,robot_zqd.Y,0);
				delay_ms(30000);
				robot_straight_stage(0,0,0);
				break;
		}

		
		
	}
	
}

