
/*
 * Driver for keys on GPIO lines capable of generating interrupts.
 *
 * Copyright 2005 Phil Blundell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/workqueue.h>

#include <linux/wakelock.h>
#include <linux/suspend.h>

#include <linux/iio/iio.h>
#include <linux/iio/machine.h>
#include <linux/iio/driver.h>
#include <linux/iio/consumer.h>

#include <asm/gpio.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>

#include "remotectl.h"

#if 0
#define remotectl_dbg(bdata, format, arg...)		\
	dev_printk(KERN_INFO , &bdata->input->dev , format , ## arg)
#else
#define remotectl_dbg(bdata, format, arg...)	
#endif

extern suspend_state_t get_suspend_state(void);

int switchkey_count=0;
int keynum_count=1;
int goon_count=1;
int switchkey_flag=0;
int NeedSwitch_flag=0;
int endkeynull_flag=0;
int goon_flag=0;
int pre_keycode=0;
int save_switch_usercode=0;

struct rkxx_remote_switchkey_table{
    int keyCode;
    int switchCode_1;
    int switchCode_2;
    int switchCode_3;
    int switchCode_4;
    int switchCode_5;		
    int switchCode_6;
    int switchCode_7;
    int switchCode_8;
};

struct rkxx_remotectl_switchkey_button {
    int row_usercode;
    int row_nbuttons;
    struct rkxx_remote_switchkey_table *switchkey_table;
};

struct rkxx_remotectl_suspend_data{
    int suspend_flag;
    int cnt;
    long scanTime[50];
};

struct rkxx_remote_key_table{
    int scanCode;
	int keyCode;		
};

struct rkxx_remotectl_button {	
    int usercode;
    int nbuttons;
    struct rkxx_remote_key_table *key_table;
};

struct rkxx_remotectl_drvdata {
    int state;
	int nbuttons;
	int result;
    unsigned long pre_time;
    unsigned long cur_time;
    long int pre_sec;
    long int cur_sec;
    long period;
    int scanData;
    int count;
    int keybdNum;
    int keycode;
    int press;
    int pre_press;
    int gpio;
    int wifi_ledclt;
    int wakeup;
    int rep;

    struct input_dev *input;
    struct timer_list timer;
    struct timer_list timer_for_switch;
    struct tasklet_struct remote_tasklet;
    struct wake_lock remotectl_wake_lock;
    struct rkxx_remotectl_suspend_data remotectl_suspend_data;
};



//特殊功能键值定义
    //193      //photo
    //194      //video
    //195      //music
    //196      //IE
    //197      //
    //198
    //199
    //200
    
    //183      //rorate_left
    //184      //rorate_right
    //185      //zoom out
    //186      //zoom in
static struct rkxx_remote_switchkey_table remote_switchkey_table_202[] = {
	{KEY_1, 51,39,181,0x00,51,39,181,0x00},//",./@"//---//"51,83,98,181"//	//",;@"//---//"51,39,181"//
	{KEY_2, KEY_A,KEY_B,KEY_C,0x00,KEY_AA,KEY_BB,KEY_CC,0x00},
	{KEY_3, KEY_D,KEY_E,KEY_F,0x00,KEY_DD,KEY_EE,KEY_FF,0x00},
	{KEY_4, KEY_G,KEY_H,KEY_I,0x00,KEY_GG,KEY_HH,KEY_II,0x00},	
	{KEY_5, KEY_J,KEY_K,KEY_L,0x00,KEY_JJ,KEY_KK,KEY_LL,0x00},
	{KEY_6, KEY_M,KEY_N,KEY_O,0x00,KEY_MM,KEY_NN,KEY_OO,0x00},
	{KEY_7, KEY_P,KEY_Q,KEY_R,KEY_S,KEY_PP,KEY_QQ,KEY_RR,KEY_SS},
	{KEY_8, KEY_T,KEY_U,KEY_V,0x00,KEY_TT,KEY_UU,KEY_VV,0x00},
	{KEY_9, KEY_W,KEY_X,KEY_Y,KEY_Z,KEY_WW,KEY_XX,KEY_YY,KEY_ZZ},
};
static struct rkxx_remote_key_table remote_key_table_40bf[] = {
	  //brower
	 {0xea, KEY_F17},
	 //tv
	 {0xda, KEY_F18},
	  //file
	 {0xca, KEY_F20},     
	 //set
	 {0x4a, KEY_F19},
	 {0x9a, KEY_HOME},     //home
	 //back
	 {0xd8, KEY_BACK}, 
	 
	{0x78, KEY_REPLY},
	{0x60, KEY_UP},
	{0xf8, KEY_DOWN},
	 {0xc2, KEY_MUTE},
	{0xba, KEY_LEFT},
	{0x3a, KEY_RIGHT},  ////////

	{0x22, KEY_VOLUMEUP},
	{0x68, KEY_VOLUMEDOWN},

	{0x58, KEY_POWER},     //power off
	     
	{0x1a, KEY_MENU},
	//here start

	 {0x80, KEY_1},          
	{0x40, KEY_2},       
	{0xc0, KEY_3},     
	{0x20, KEY_4},     
	{0xa0, KEY_5},
	{0xc8, KEY_6}, 
	{0xe0, KEY_7},
	{0x10, KEY_8},
	{0x90, KEY_9},
	{0x0, KEY_0}, 
	{0x30, KEY_DELETE}, 

	//media ctrl
	{0xaa, 402},//play pause
	{0x2a, 400},//prev
	{0xe8, 401},//next

	//mouse switch
	{0xd0,388},

	};
static struct rkxx_remote_key_table remote_key_table_202[] = {
      //brower
     {0xca, KEY_F17},
     //vedio
     {0xda, KEY_F21},
      //games
     {0xea, KEY_F20},     
     //allapk
     {0x2a, KEY_F19},
     
     {0x58, KEY_HOME},     //home
     //back
     {0xA2, KEY_BACK}, 
     
    {0xB0, KEY_REPLY},//ok = DPAD CENTER
  
   	{0x42, KEY_F24},	//EPG---recent app
   
    {0xD0, KEY_UP},
    {0x70, KEY_DOWN},
     {0xC2, KEY_MUTE},
    {0x08, KEY_LEFT},
    {0x88, KEY_RIGHT},  ////////
    
    {0x18, KEY_VOLUMEUP},
    {0xe8, KEY_VOLUMEDOWN},

    {0xB2, KEY_POWER},     //power off
         //mute
    {0xb8, KEY_MENU},
    //here start
  
     {0x80, KEY_1},          
    {0x40, KEY_2},       
    {0xc0, KEY_3},     
    {0x20, KEY_4},     
    {0xa0, KEY_5},
    {0x60, KEY_6}, 
    {0xe0, KEY_7},
    {0x10, KEY_8},
    {0x90, KEY_9},
    {0x00, KEY_0}, 
    {0x30, KEY_DELETE}, 
    {0xf4, KEY_SPACE},

//media ctrl
    {0x02,   402},      //play pause
    {0xF8,   400},      //pre
    {0x78,   401},	  //next
    {0x82,   66},     //stop
    
    {0x22,   888}, 


//mouse switch
    {0xe2,388},

};


/*
static struct rkxx_remote_key_table remote_key_table_df[] = {
    {0xf8, KEY_REPLY},
    {0xc0, KEY_BACK}, 
    {0xf0, KEY_UP},
    {0xd8, KEY_DOWN},
    {0xd0, KEY_LEFT},
    {0xe8,KEY_RIGHT},  ////////
    {0x90, KEY_VOLUMEDOWN},
    {0x60, KEY_VOLUMEUP},
    {0x80, KEY_HOME},     //home
    {0xe0, 183},          //rorate left
    {0x10, 184},          //rorate right
    {0x20, 185},          //zoom out
    {0xa0, 186},          //zoom in
    {0x70, KEY_MUTE},       //mute
    {0x50, KEY_POWER},     //power off
    {0x40, KEY_SEARCH},     //search
};
*/
static struct rkxx_remote_key_table remote_key_table_sunchip_ff[] = {
	
    {0xc2, KEY_POWER},
    {0xd2, KEY_MUTE}, 
    {0x52, KEY_MENU},
    {0xe0, KEY_BACK},
    {0x62, KEY_UP},          
    {0x22, KEY_DOWN},         
    {0x82, KEY_LEFT},
    {0x92, KEY_RIGHT},  
    {0xc0, KEY_VOLUMEDOWN},
    {0x2, KEY_VOLUMEUP},
    {0x42, KEY_HOME},     
    {0xa2, KEY_REPLY},          
    {0x40, KEY_1},          
    {0x60, KEY_2},       
    {0x50, KEY_3},     
    {0x80, KEY_4},     
    {0xa0, KEY_5},
    {0x90, KEY_6}, 
    {0x0, KEY_7},
    {0x20, KEY_8},
    {0x10, KEY_9},
    {0xb0, KEY_0},  
    {0xe2, 388}, //SERACH---Serach^M
    {0x30, KEY_F17},   //RATIO---Browser^M
//    {0x1a, KEY_F18},     //SETUP---Setting^M
//    {0x70, KEY_F19}, //SYSTEM---All app^M
//    {0xf2, KEY_F20},         //AUDIO---Music^M
//    {0xd2, KEY_F21},     //TV/RADIO---Video^M
//    {0x90, KEY_CHANNELUP},     //play_pause^M
//    {0x10, KEY_F22},     //Market^M
//    {0xd0, KEY_F11},     //PREVIOUS^M
//    {0x12, KEY_F12},     //NEXT^M
    {0x12, KEY_F9},    //MEDIA_PREVIOUS^M
    {0xd0, KEY_F10},   //MEDIA_NEXT ^M
//    {0xb2, KEY_F15},^M
//    {0x92, KEY_F16},^M
//    {0xa2, KEY_F8},  //MEDIA_STOP^M
//    {0x82, KEY_F23},         //INFO^M
//    {0x22, KEY_F24}, //EPG---recent app^M
//    {0x30, KEY_F1},  //TXT^M
//    {0x18, KEY_F2},     //SUB.T^M
//    {0xba, KEY_F3},     //PAGE_UP^M
//    {0xf8, KEY_F4},  //PAGE_DOWN^M
//    {0xa0, KEY_F5},  //dot .^M
    {0x70, 111},        //delet^M
};


static struct rkxx_remote_key_table remote_key_table_ruikemei[] = {                 
                                                                               
                                                                               
    {0xba, KEY_POWER}, //电源键                                                
    {0xa2, KEY_UP},                                                            
    {0xaa, KEY_DOWN},                                                          
    {0xe0, KEY_LEFT},                                                          
    {0x80, KEY_RIGHT},                                                         
    {0x82, KEY_REPLY},                                                         
    {0x0, KEY_BACK},                                                           
    {0xa0, KEY_MENU},                                                          
    {0x62, KEY_HOME},                                                          
    {0xc8, KEY_VOLUMEDOWN},                                                    
    {0xe8, KEY_VOLUMEUP},                                                      
    {0xd8,   402},      //play pause                                           
    {0x28,   400},      //pre                                                  
    {0xa8,   401},	  //next                                                   
    {0x5a,   KEY_MUTE}                                                              
	//power off                                                                  
}; 	


suspend_state_t get_suspend_state(void)
{
	return 0;
}
static struct rkxx_remotectl_switchkey_button remotectl_switchkey_button[] = 
{
    {  
       .row_usercode = 0x202,
       .row_nbuttons =  9, 
       .switchkey_table = &remote_switchkey_table_202[0],
    },
    /*
    {  
       .row_usercode = 0x40bf,
       .row_nbuttons =  9, 
       .switchkey_table = &remote_switchkey_table_40bf[0],
    },*/
};


static struct rkxx_remotectl_button remotectl_button[] = 
{
    {  
       .usercode = 0xff,
       .nbuttons =  27, 
       .key_table = &remote_key_table_sunchip_ff[0],
    },
    {  
       .usercode = 0x202, 
       .nbuttons =  35, 
       .key_table = &remote_key_table_202[0],
    },
    {  
       .usercode = 0x8f7, 
       .nbuttons =  15, 
       .key_table = &remote_key_table_ruikemei[0],
    },    
    {  
       .usercode = 0x40bf, 
       .nbuttons =  31, 
       .key_table = &remote_key_table_40bf[0],
    },
};


static int remotectl_keybdNum_lookup(struct rkxx_remotectl_drvdata *ddata)
{	
    int i;	

    for (i = 0; i < sizeof(remotectl_button)/sizeof(struct rkxx_remotectl_button); i++){		
        if (remotectl_button[i].usercode == (ddata->scanData&0xFFFF)){			
            ddata->keybdNum = i;
            if(remotectl_button[i].usercode == 0x202||remotectl_button[i].usercode == 0x40bf||remotectl_button[i].usercode == 0x1fe){
            	NeedSwitch_flag=1;
            	save_switch_usercode=remotectl_button[i].usercode;
            }else
            	save_switch_usercode=0x00;
            return 1;
        }
    }
    return 0;
}


static int remotectl_keycode_lookup(struct rkxx_remotectl_drvdata *ddata)
{	
    int i;	
    unsigned char keyData = ((ddata->scanData >> 8) & 0xff);

    for (i = 0; i < remotectl_button[ddata->keybdNum].nbuttons; i++){
        if (remotectl_button[ddata->keybdNum].key_table[i].scanCode == keyData){			
            ddata->keycode = remotectl_button[ddata->keybdNum].key_table[i].keyCode;
            return 1;
        }
    }
    return 0;
}


static void remotectl_get_pwr_scanData(struct rkxx_remotectl_drvdata *ddata,int *pwr_data,int loop)
{	
    int i;
    int temp_scanCode;
    int temp_pwr_data;
    
    for (i = 0; i < remotectl_button[loop].nbuttons; i++){
        if (remotectl_button[loop].key_table[i].keyCode == KEY_POWER){			
            temp_scanCode = remotectl_button[loop].key_table[i].scanCode;
            temp_pwr_data = (temp_scanCode<<8)|((~temp_scanCode)&0xFF);
            //printk("pwr data =0x%x\n",temp_pwr_data);
        }
    }
    *pwr_data = temp_pwr_data;
}

static int KeyCodeChange(int keycode)
{
	int Newkeycode;
	int i,j;	
    //printk("KeyCodeChange keycode=%d,sizeof_button=%d\n",keycode,sizeof(remotectl_switchkey_button)/sizeof(struct rkxx_remotectl_switchkey_button));
       
    for (i = 0; i < sizeof(remotectl_switchkey_button)/sizeof(struct rkxx_remotectl_switchkey_button); i++){		
        if (remotectl_switchkey_button[i].row_usercode == save_switch_usercode){			
            j = i;
            break;
        }
    }

    for (i = 0; i < remotectl_switchkey_button[j].row_nbuttons; i++){    	
        if (remotectl_switchkey_button[j].switchkey_table[i].keyCode == keycode){
        	if(keynum_count==1){
        		if(switchkey_flag==1)			
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_1;
            		else if(switchkey_flag==2)
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_5;
        	}else if(keynum_count==2){			
            		if(switchkey_flag==1)			
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_2;
            		else if(switchkey_flag==2)
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_6;            		
        	}else if(keynum_count==3){			
            		if(switchkey_flag==1)			
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_3;
            		else if(switchkey_flag==2)
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_7;
        	}if(keynum_count==4){			
            		if(switchkey_flag==1)			
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_4;
            		else if(switchkey_flag==2)
            			Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_8;
            		if(Newkeycode==0x00){
            			keynum_count=1;
            			endkeynull_flag=1;
            			if(switchkey_flag==1)			
            				Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_1;
            			else if(switchkey_flag==2)
            				Newkeycode = remotectl_switchkey_button[j].switchkey_table[i].switchCode_5;
            		}else
            			endkeynull_flag=0;
        	}
            return Newkeycode;
        }
    }
    return 0;
	
	
}

static void remotectl_do_something(unsigned long  data)
{
    struct rkxx_remotectl_drvdata *ddata = (struct rkxx_remotectl_drvdata *)data;

    switch (ddata->state)
    {
        case RMC_IDLE:
        {
            ;
        }
        break;
        
        case RMC_PRELOAD:
        {
            mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(130));
            //printk("RMC_PRELOAD,period=%d\n",ddata->period);
            if ((TIME_PRE_MIN < ddata->period) && (ddata->period < TIME_PRE_MAX)){
                
                ddata->scanData = 0;
                ddata->count = 0;
                ddata->state = RMC_USERCODE;
            }else{
                ddata->state = RMC_PRELOAD;
            }
            ddata->pre_time = ddata->cur_time;
            //mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(130));
        }
        break;
        
        case RMC_USERCODE:
        {
            ddata->scanData <<= 1;
            ddata->count ++;
            if ((TIME_BIT1_MIN < ddata->period) && (ddata->period < TIME_BIT1_MAX)){
                ddata->scanData |= 0x01;
            }
		
            if (ddata->count == 0x10){//16 bit user code
          //    printk("u=0x%x\n",((ddata->scanData)&0xFFFF));
                if (remotectl_keybdNum_lookup(ddata)){
                    ddata->state = RMC_GETDATA;
                    ddata->scanData = 0;
                    ddata->count = 0;
                }else{                //user code error
                    ddata->state = RMC_PRELOAD;
                }
            }
        }
        break;
        
        case RMC_GETDATA:
        {
            ddata->count ++;
            ddata->scanData <<= 1;

          
            if ((TIME_BIT1_MIN < ddata->period) && (ddata->period < TIME_BIT1_MAX)){
                ddata->scanData |= 0x01;
            }           
            if (ddata->count == 0x10){
                //printk("RMC_GETDATA=%x\n",(ddata->scanData&0xFFFF));

                if ((ddata->scanData&0x0ff) == ((~ddata->scanData >> 8)&0x0ff)){
                    if (remotectl_keycode_lookup(ddata)){
                        ddata->press = 1;
                        /*
                         if (get_suspend_state()==0){
                                input_event(ddata->input, EV_KEY, ddata->keycode, 1);
                                input_sync(ddata->input);
                            }else if ((get_suspend_state())&&(ddata->keycode==KEY_POWER)){
                                input_event(ddata->input, EV_KEY, KEY_WAKEUP, 1);
                                input_sync(ddata->input);
                            }*/
                            //printk("0\n");
                        if(NeedSwitch_flag){
	            	 	if(ddata->keycode==888){
	            	 		switchkey_flag++;
	            	 		if(switchkey_flag>2) switchkey_flag=0;
	            	 	}           	 	
	            	 }	            	 
	            	 if((switchkey_flag>0)&&(ddata->keycode >= 0x02&&ddata->keycode <= 0x0a)){
	            	 	if(ddata->keycode!=pre_keycode){
	            	 		pre_keycode = ddata->keycode;
	            	 		goon_count=1;
	            	 		keynum_count=1;
	            	 	}	    	 		
	    	 	 	ddata->keycode = KeyCodeChange(ddata->keycode);
	    	 	 	if(ddata->keycode==KEY_RESERVED){
	            	 		keynum_count=1;
		            	        goon_count=1;
		            	        pre_keycode=0;
		            	        goon_flag=0;
		            	        break;
	            	 	}	    	 	 	
	    	 	 }else{	    	 	 
		            	 keynum_count=1;
		            	 goon_count=1;
		            	 pre_keycode=0;
		            	 goon_flag=0;
		         }
		         
		         if((switchkey_flag>0)&&(ddata->keycode==0x0b)&&(save_switch_usercode==0x202))ddata->keycode=KEY_SPACE;
	    	 	 //printk( "ddata->keycode=0x%x, switchkey_flag=%d\n",ddata->keycode,switchkey_flag);
	    	 	                             
                         if((switchkey_flag>0)&&(goon_flag)&&(keynum_count>1||goon_count==4||endkeynull_flag)){
	         	 goon_count++;
		         
		         if(goon_count>4)goon_count=1;
		         input_event(ddata->input, EV_KEY, KEY_DELETE, 1);
			 input_sync(ddata->input);

			 input_event(ddata->input, EV_KEY, KEY_DELETE, 0);
			 input_sync(ddata->input);
			 }
			 
			 input_event(ddata->input, EV_KEY, ddata->keycode, 1);
			 input_sync(ddata->input);
                        //input_event(ddata->input, EV_KEY, ddata->keycode, ddata->press);
		                //input_sync(ddata->input);
                        ddata->state = RMC_SEQUENCE;
                    }else{
                        ddata->state = RMC_PRELOAD;
                    }
                }else{
                    ddata->state = RMC_PRELOAD;
                }
            }
        }
        break;
             
        case RMC_SEQUENCE:{

            //printk( "S=%d\n",ddata->period);
  
            if ((TIME_RPT_MIN < ddata->period) && (ddata->period < TIME_RPT_MAX)){
            		 mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(110));
                 //printk("1\n");;
            }else if ((TIME_SEQ1_MIN < ddata->period) && (ddata->period < TIME_SEQ1_MAX)){
	 							  mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(110));
	 							  //printk("2\n");
            }else if ((TIME_SEQ2_MIN < ddata->period) && (ddata->period < TIME_SEQ2_MAX)){
            		  mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(110));
            		  //printk("3\n");;   
            }else{  	 
            	 
        	 input_event(ddata->input, EV_KEY, ddata->keycode, 0);
	         input_sync(ddata->input);
        	 ddata->state = RMC_PRELOAD;
        	 ddata->press = 0;
        	 if((switchkey_flag>0)&&(pre_keycode!=KEY_RESERVED)){
        	 	mod_timer(&ddata->timer_for_switch,jiffies + msecs_to_jiffies(800));
        	 }
        	 //printk("4\n");
            }
        }
        break;
       
        default:
            break;
    } 
	return;
}


#ifdef CONFIG_PM
void remotectl_wakeup(unsigned long _data)
{
    struct rkxx_remotectl_drvdata *ddata =  (struct rkxx_remotectl_drvdata*)_data;
    long *time;
    int i;
	int power_scanData;
		 
    time = ddata->remotectl_suspend_data.scanTime;

    if (get_suspend_state()){
        ddata->remotectl_suspend_data.suspend_flag = 0;
        ddata->count = 0;
        ddata->state = RMC_USERCODE;
        ddata->scanData = 0;
        
        for (i=0;i<ddata->remotectl_suspend_data.cnt;i++){
        		if (ddata->count>=32)
        			break;

           if ((TIME_BIT1_MIN < time[i]) && (time[i] < TIME_BIT1_MAX)){
                ddata->scanData |= 0x01;
                ddata->scanData <<= 1;
                ddata->count ++;;
            }else if ((TIME_BIT0_MIN < time[i]) && (time[i] < TIME_BIT0_MAX)){
            	  ddata->scanData <<= 1;
            	  ddata->count ++;;
            }/*else{
            	   if (ddata->count>16){
            	   	  break;
            	   }else{
            	   	
            	   	printk(KERN_ERR "ddata->count=0x%x**********************\n",ddata->count);
            	   	ddata->count = 0;
            	   	ddata->scanData = 0;
            	   }		
            }*/
        }
        //printk(KERN_ERR"data=0x%x\n",ddata->scanData);
        if (ddata->scanData)					//(ddata->scanData>16)			
				{
					  ddata->scanData=(ddata->scanData>>1)&0xFFFF;				
					  printk(KERN_ERR"data=0x%x\n",ddata->scanData);
					  
					  for (i=0;i<sizeof(remotectl_button)/sizeof(struct rkxx_remotectl_button);i++){
					  	remotectl_get_pwr_scanData(ddata,&power_scanData,i);
					  	if ((ddata->scanData == power_scanData)||((ddata->scanData&0x0fff) == (power_scanData&0x0fff))||((ddata->scanData&0x00ff) == (power_scanData&0x00ff)))					//modified by zwm	2013.06.19
					    {
					    	input_event(ddata->input, EV_KEY, KEY_WAKEUP, 1);
            		input_sync(ddata->input);
            		input_event(ddata->input, EV_KEY, KEY_WAKEUP, 0);
            		input_sync(ddata->input);
            		break;
					    }
					  }
				}
    }
    memset(ddata->remotectl_suspend_data.scanTime,0,50*sizeof(long));
    ddata->remotectl_suspend_data.cnt= 0; 
    ddata->state = RMC_PRELOAD;
    
}

#endif


static void remotectl_timer(unsigned long _data)
{
    struct rkxx_remotectl_drvdata *ddata =  (struct rkxx_remotectl_drvdata*)_data;
    
    //printk("to\n");
    
    if(ddata->press != ddata->pre_press) {
        ddata->pre_press = ddata->press = 0;
        
				input_event(ddata->input, EV_KEY, ddata->keycode, 0);
        input_sync(ddata->input);
        if((switchkey_flag>0)&&(pre_keycode!=KEY_RESERVED)){
	 	mod_timer(&ddata->timer_for_switch,jiffies + msecs_to_jiffies(800));
	}
        //printk("5\n");
        //if (get_suspend_state()==0){
            //input_event(ddata->input, EV_KEY, ddata->keycode, 1);
            //input_sync(ddata->input);
            //input_event(ddata->input, EV_KEY, ddata->keycode, 0);
		    //input_sync(ddata->input);
        //}else if ((get_suspend_state())&&(ddata->keycode==KEY_POWER)){
            //input_event(ddata->input, EV_KEY, KEY_WAKEUP, 1);
            //input_sync(ddata->input);
            //input_event(ddata->input, EV_KEY, KEY_WAKEUP, 0);
            //input_sync(ddata->input);
        //}
    }
#ifdef CONFIG_PM
    remotectl_wakeup(_data);
#endif
    ddata->state = RMC_PRELOAD;
}

static void switch_timer(unsigned long _data){
	struct rkxx_remotectl_drvdata *ddata =  (struct rkxx_remotectl_drvdata*)_data;	
	
	goon_flag=0;
	goon_count=1;
	keynum_count=1;
	pre_keycode=0;	
}

static irqreturn_t remotectl_isr(int irq, void *dev_id)
{
    struct rkxx_remotectl_drvdata *ddata =  (struct rkxx_remotectl_drvdata*)dev_id;
    struct timeval  ts;


    ddata->pre_time = ddata->cur_time;
    ddata->pre_sec = ddata->cur_sec;
    do_gettimeofday(&ts);
    ddata->cur_time = ts.tv_usec;
    ddata->cur_sec = ts.tv_sec;
    
		if (likely(ddata->cur_sec == ddata->pre_sec)){
			ddata->period =  ddata->cur_time - ddata->pre_time;
	  }else{
				ddata->period =  1000000 - ddata->pre_time + ddata->cur_time;
		}

	if((switchkey_flag>0)&&(pre_keycode!=KEY_RESERVED)){
	if(ddata->period>=80000&&ddata->period<=500000){		
	 	keynum_count++;
	 	goon_flag=1;
	 	if(keynum_count>4){
	 		keynum_count=1;
	 	}
	}
	}
    tasklet_hi_schedule(&ddata->remote_tasklet); 
    //if ((ddata->state==RMC_PRELOAD)||(ddata->state==RMC_SEQUENCE))
    //mod_timer(&ddata->timer,jiffies + msecs_to_jiffies(130));
#ifdef CONFIG_PM
   if (ddata->state==RMC_PRELOAD)
       wake_lock_timeout(&ddata->remotectl_wake_lock, HZ);
   if ((get_suspend_state())&&(ddata->remotectl_suspend_data.cnt<50))		//zwm
       ddata->remotectl_suspend_data.scanTime[ddata->remotectl_suspend_data.cnt++] = ddata->period;
#endif

    return IRQ_HANDLED;
}

static ssize_t anroidTokernel_write(struct device *dev,
  struct device_attribute *attr,const char *buf, size_t count)
{

	int rc;
	unsigned long t_cmd;

	if (strict_strtoul(buf, 0, &t_cmd))
                return -EINVAL;
	//printk("anroidTokernel :anroidTokernel_write entry t_cmd=%d\n",t_cmd);
	
	if(t_cmd==100){
		;
	}	
	return count;
}

static ssize_t anroidTokernel_read(struct device *dev,
  struct device_attribute *attr, char *buf)
{
 return 0;
}

static DEVICE_ATTR(anroidTokernel, 0777, anroidTokernel_read, anroidTokernel_write);
static int remotectl_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
	  struct device_node *node = pdev->dev.of_node;
    struct rkxx_remotectl_drvdata *ddata;
    struct input_dev *input;
    unsigned int gpio;
    int i, j;
    int irq;
    int error = 0;

    printk("++++++++remotectl driver V1.0 for linux 3.10\n");

    ddata = kzalloc(sizeof(struct rkxx_remotectl_drvdata),GFP_KERNEL);
    memset(ddata,0,sizeof(struct rkxx_remotectl_drvdata));

    ddata->state = RMC_PRELOAD;
    input = input_allocate_device();
    
    if (!ddata || !input) {
        error = -ENOMEM;
        goto fail0;
    }

    platform_set_drvdata(pdev, ddata);

    input->name = "rkxx-remotectl";//pdev->name;
    input->phys = "gpio-keys/input0";
    input->dev.parent = &pdev->dev;

    input->id.bustype = BUS_HOST;
    input->id.vendor = 0x0001;
    input->id.product = 0x0001;
    input->id.version = 0x0100;

	/* Enable auto repeat feature of Linux input subsystem */
	//if (ddata->rep)
		//__set_bit(EV_REP, input->evbit);
    
	//ddata->nbuttons = pdata->nbuttons;
	ddata->input = input;
  wake_lock_init(&ddata->remotectl_wake_lock, WAKE_LOCK_SUSPEND, "rk29_remote");
  
  gpio = of_get_named_gpio(node, "module-gpios", 0);
		if (!gpio_is_valid(gpio)) {
			printk("invalid gpio specified\n");
			error = -2;
			goto fail;
	}
		
  error = devm_gpio_request(dev, gpio, "remotectl");
	if (error < 0) {
		printk("gpio-keys: failed to request GPIO %d,"
		" error %d\n", gpio, error);
		goto fail0;
	}
	error = gpio_direction_input(gpio);
	if (error < 0) {
		pr_err("gpio-keys: failed to configure input"
			" direction for GPIO %d, error %d\n",
		gpio, error);
		gpio_free(gpio);
		goto fail0;
	}
    irq = gpio_to_irq(gpio);
	if (irq < 0) {
		error = irq;
		pr_err("gpio-keys: Unable to get irq number for GPIO %d, error %d\n",	gpio, error);
		gpio_free(gpio);
		goto fail1;
	}

    ddata->gpio = gpio;
    ddata->wakeup = 1;

	error = request_irq(irq, remotectl_isr,	IRQF_TRIGGER_FALLING , "remotectl", ddata);
	
	if (error) {
		pr_err("gpio-remotectl: Unable to claim irq %d; error %d\n", irq, error);
		gpio_free(gpio);
		goto fail1;
	}
	
     gpio = of_get_named_gpio(node, "module-gpios", 1);
	if (!gpio_is_valid(gpio)) {
		printk("invalid gpio specified\n");
		error = -2;
		goto fail;
	}
		
     error = devm_gpio_request(dev, gpio, "wifi_ledclt");
	if (error < 0) {
		printk("gpio-keys: failed to request GPIO %d,"
		" error %d\n", gpio, error);
		goto fail0;
	}
     error = gpio_direction_output(gpio,1);
	if (error < 0) {
		pr_err("gpio-keys: failed to configure output"
			" direction for GPIO %d, error %d\n",
		gpio, error);
		gpio_free(gpio);
		goto fail0;
	}
    ddata->wifi_ledclt = gpio;
	
    setup_timer(&ddata->timer,remotectl_timer, (unsigned long)ddata);
    setup_timer(&ddata->timer_for_switch,switch_timer, (unsigned long)ddata);
    
    tasklet_init(&ddata->remote_tasklet, remotectl_do_something, (unsigned long)ddata);
    
    device_create_file(&pdev->dev,&dev_attr_anroidTokernel);
    
    for (j=0;j<sizeof(remotectl_button)/sizeof(struct rkxx_remotectl_button);j++){ 
    	printk("remotectl probe j=0x%x\n",j);
		for (i = 0; i < remotectl_button[j].nbuttons; i++) {
			unsigned int type = EV_KEY;
	        
			input_set_capability(input, type, remotectl_button[j].key_table[i].keyCode);
		}
  }
  
  	for (j = 0; j < sizeof(remotectl_switchkey_button)/sizeof(struct rkxx_remotectl_switchkey_button); j++){
  		for (i = 0; i < remotectl_switchkey_button[j].row_nbuttons; i++) {
			unsigned int type = EV_KEY;
	        
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_1);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_2);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_3);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_4);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_5);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_6);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_7);
			input_set_capability(input, type, remotectl_switchkey_button[j].switchkey_table[i].switchCode_8);
		}
  	}
  	
	error = input_register_device(input);
	if (error) {
		pr_err("gpio-keys: Unable to register input device, error: %d\n", error);
		goto fail2;
	}
    
	input_set_capability(input, EV_KEY, KEY_WAKEUP);

	device_init_wakeup(&pdev->dev, 1);

	return 0;

fail2:
    pr_err("gpio-remotectl input_allocate_device fail\n");
	input_free_device(input);
	kfree(ddata);
fail1:
    pr_err("gpio-remotectl gpio irq request fail\n");
    free_irq(gpio_to_irq(gpio), ddata);
    del_timer_sync(&ddata->timer);
    tasklet_kill(&ddata->remote_tasklet); 
    gpio_free(gpio);
fail0: 
    pr_err("gpio-remotectl input_register_device fail\n");
    platform_set_drvdata(pdev, NULL);
fail:
	
	return error;
}

static int remotectl_remove(struct platform_device *pdev)
{
	struct rkxx_remotectl_drvdata *ddata = platform_get_drvdata(pdev);
	struct input_dev *input = ddata->input;
    int irq;

	device_init_wakeup(&pdev->dev, 0);
    irq = gpio_to_irq(ddata->gpio);
    free_irq(irq, ddata);
    tasklet_kill(&ddata->remote_tasklet); 
    gpio_free(ddata->gpio);

	input_unregister_device(input);

	return 0;
}


#ifdef CONFIG_PM
static int remotectl_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
    struct rkxx_remotectl_drvdata *ddata = platform_get_drvdata(pdev);
    
    //ddata->remotectl_suspend_data.suspend_flag = 1;
    ddata->remotectl_suspend_data.cnt = 0;

	if (device_may_wakeup(&pdev->dev)) {
		if (ddata->wakeup) {
			int irq = gpio_to_irq(ddata->gpio);
			enable_irq_wake(irq);
		}
	}
    
	return 0;
}

static int remotectl_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
    struct rkxx_remotectl_drvdata *ddata = platform_get_drvdata(pdev);

    if (device_may_wakeup(&pdev->dev)) {
        if (ddata->wakeup) {
            int irq = gpio_to_irq(ddata->gpio);
            disable_irq_wake(irq);
        }
    }

	return 0;
}

static const struct dev_pm_ops remotectl_pm_ops = {
	.suspend	= remotectl_suspend,
	.resume		= remotectl_resume,
};
#endif

#ifdef CONFIG_OF
static const struct of_device_id of_rk_remotectl_match[] = {
	{ .compatible = "rockchip,remotectl" },
	{ /* Sentinel */ }
};
#endif

static struct platform_driver remotectl_device_driver = {
	.probe		= remotectl_probe,
	.remove		= remotectl_remove,
	.driver		= {
		.name	= "rkxx-remotectl",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
	    .pm	= &remotectl_pm_ops,
#endif
#ifdef CONFIG_OF
		.of_match_table	= of_rk_remotectl_match,
#endif
	},

};

static int  remotectl_init(void)
{
    printk(KERN_INFO "++++++++remotectl_init\n");
    return platform_driver_register(&remotectl_device_driver);
}


static void  remotectl_exit(void)
{
	platform_driver_unregister(&remotectl_device_driver);
    printk(KERN_INFO "++++++++remotectl_init\n");
}

module_init(remotectl_init);
module_exit(remotectl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rockchip");
MODULE_DESCRIPTION("Keyboard driver for CPU GPIOs");
MODULE_ALIAS("platform:gpio-keys1");


