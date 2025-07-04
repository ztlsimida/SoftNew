#include "typesdef.h"
#include "errno.h"
#include "osal/irq.h"
#include "devid.h"
#include "dev/scale/hgscale.h"
#include "osal/string.h"


struct hgscale1_hw
{
    __IO uint32 SCALECON;   		//0x00
    __IO uint32 SWINCON;         	//0x04
    __IO uint32 TWINCON;         	//0x08
    __IO uint32 SWIDTH_STEP;       //0x0C
    __IO uint32 SHEIGH_STEP;       //0x10
    __IO uint32 INBUF_LINE_NUM;    //0x14
    __IO uint32 INYDMA_STADR;      //0x18
    __IO uint32 INUDMA_STADR;      //0x1C
    __IO uint32 INVDMA_STADR;      //0x20
    __IO uint32 INBUFCON;          //0x24
    __IO uint32 SHEIGH_CNT;        //0x28	
    __IO uint32 SCALESTA;          //0x2C
};

struct hgscale2_hw
{
    __IO uint32 SCALECON;   		//0x00
    __IO uint32 SWINCON;         	//0x04
    __IO uint32 SSTART;         	//0x08
    __IO uint32 TWINCON;         	//0x0C
    __IO uint32 SWIDTH_STEP;       //0x10
    __IO uint32 SHEIGH_STEP;       //0x14
    __IO uint32 SRAMBUF_STADR;     //0x18
    __IO uint32 OUTYDMA_STADR;     //0x1C
    __IO uint32 OUTUDMA_STADR;     //0x20
	__IO uint32 OUTVDMA_STADR;     //0x24
	__IO uint32 SHEIGH_CNT;     	//0x28
    __IO uint32 SCALESTA;          //0x2C
};

struct hgscale3_hw
{
    __IO uint32 SCALECON;   		//0x00
    __IO uint32 SWINCON;         	//0x04
    __IO uint32 SSTART;         	//0x08
    __IO uint32 TWINCON;         	//0x0C
    __IO uint32 SWIDTH_STEP;       //0x10
    __IO uint32 SHEIGH_STEP;       //0x14
    __IO uint32 INBUF_LINE_NUM;    //0x18
    __IO uint32 INYDMA_STADR;      //0x1C
    __IO uint32 INUDMA_STADR;      //0x20
    __IO uint32 INVDMA_STADR;      //0x24
    __IO uint32 OUTYDMA_STADR;     //0x28
    __IO uint32 OUTUDMA_STADR;     //0x2C
    __IO uint32 OUTVDMA_STADR;     //0x30
    __IO uint32 INBUFCON;          //0x34
    __IO uint32 SHEIGH_CNT;        //0x38	
    __IO uint32 SCALESTA;          //0x3C
};

scale_irq_hdl scaleirq1_vector_table[SCALE_IRQ_NUM];
volatile uint32  scaleirq1_dev_table[SCALE_IRQ_NUM];

scale_irq_hdl scaleirq2_vector_table[SCALE_IRQ_NUM];
volatile uint32 scaleirq2_dev_table[SCALE_IRQ_NUM];

scale_irq_hdl scaleirq3_vector_table[SCALE_IRQ_NUM];
volatile uint32  scaleirq3_dev_table[SCALE_IRQ_NUM];

static int32 hgscale1_ioctl(struct scale_device *p_scale, enum scale_ioctl_cmd ioctl_cmd, uint32 param1, uint32 param2){
    int32  ret_val = RET_OK;	
	uint32 s_w,s_h,d_w,d_h;
	//uint32 *size_array;
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	switch(ioctl_cmd){
		case SCALE_IOCTL_CMD_SET_IN_OUT_SIZE:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;
			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWINCON = (s_w-1)|((s_h-1)<<16);
			hw->TWINCON = (d_w-1)|((d_h-1)<<16);
		break;

		case SCALE_IOCTL_CMD_SET_STEP:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;
			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWIDTH_STEP = 256*s_w/d_w;
			hw->SHEIGH_STEP = 256*s_h/d_h;
		break;
		
		case SCALE_IOCTL_CMD_SET_LINE_BUF_NUM:
			hw->INBUF_LINE_NUM = param1;
		break;
	
		case SCALE_IOCTL_CMD_SET_IN_Y_ADDR:
			hw->INYDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_IN_U_ADDR:
			hw->INUDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_IN_V_ADDR:
			hw->INVDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_DATA_FROM:
			if(param1)
				hw->SCALECON &=~BIT(2);				//vpp
			else
				hw->SCALECON |= BIT(2);				//soft
		break;

		case SCALE_IOCTL_CMD_GET_INBUF_NUM:
			ret_val = (hw->INBUFCON>>16)&0x3ff;
		break;

		case SCALE_IOCTL_CMD_SET_INBUF_NUM:
			hw->INBUFCON = ((param1)<<16);
		break;

		case SCALE_IOCTL_CMD_SET_NEW_FRAME:
			hw->INBUFCON |= BIT(15);
		break;

		case SCALE_IOCTL_CMD_SET_END_FRAME:
			hw->INBUFCON |= BIT(31);
		break;
		
		case SCALE_IOCTL_CMD_GET_HEIGH_CNT:
			ret_val = hw->SHEIGH_CNT;
		break;
		
		default:
			os_printf("NO SCALE1 IOCTL:%d\r\n",ioctl_cmd);
            ret_val = -ENOTSUPP;
        break;
	}


	return ret_val;
}


static int32 hgscale2_ioctl(struct scale_device *p_scale, enum scale_ioctl_cmd ioctl_cmd, uint32 param1, uint32 param2){
    int32  ret_val = RET_OK;
	uint32 s_w,s_h,d_w,d_h;
	uint32 s_x,s_y;
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	switch(ioctl_cmd){
		case SCALE_IOCTL_CMD_SET_IN_OUT_SIZE:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;

			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWINCON = (s_w-1)|((s_h-1)<<16);
			hw->TWINCON = (d_w-1)|((d_h-1)<<16);
		break;
		
		case SCALE_IOCTL_CMD_SET_STEP:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;	
			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWIDTH_STEP = 256*s_w/d_w;
			hw->SHEIGH_STEP = 256*s_h/d_h;
		break;

		

		case SCALE_IOCTL_CMD_SET_START_ADDR:
			s_x = param1 & 0xffff;
			s_y = (param1>>16) & 0xffff;
			hw->SSTART = (s_x) |(s_y<<16);
		break;
		
		case SCALE_IOCTL_CMD_SET_SRAMBUF_WLEN:
			hw->SCALECON &= ~(BIT(4)|BIT(5));
			if(param1 == 64){
				hw->SCALECON |= BIT(4);
			}else if(param1 == 256){
				hw->SCALECON |= BIT(5);
			}else if(param1 == 512){
				hw->SCALECON |= (BIT(4)|BIT(5));
			}
		break;

		case SCALE_IOCTL_CMD_SET_OUT_Y_ADDR:
			hw->OUTYDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_OUT_U_ADDR:
			hw->OUTUDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_OUT_V_ADDR:
			hw->OUTVDMA_STADR = param1;
		break;
		
		case SCALE_IOCTL_CMD_SET_SRAM_ADDR:
			hw->SRAMBUF_STADR = param1;
		break;
	
		case SCALE_IOCTL_CMD_GET_HEIGH_CNT:
			ret_val = hw->SHEIGH_CNT;
		break;

		default:
			os_printf("NO SCALE2 IOCTL:%d\r\n",ioctl_cmd);
            ret_val = -ENOTSUPP;
        break;
	}


	return ret_val;
}

static int32 hgscale3_ioctl(struct scale_device *p_scale, enum scale_ioctl_cmd ioctl_cmd, uint32 param1, uint32 param2){
    int32  ret_val = RET_OK;	
	uint32 s_w,s_h,d_w,d_h;
	uint32 s_x,s_y;
//	uint32 *size_array;
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	switch(ioctl_cmd){
		case SCALE_IOCTL_CMD_SET_IN_OUT_SIZE:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;

			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWINCON = (s_w-1)|((s_h-1)<<16);
			hw->TWINCON = (d_w-1)|((d_h-1)<<16);

		break;

		case SCALE_IOCTL_CMD_SET_STEP:
			s_w = param1 & 0xffff;
			s_h = (param1>>16) & 0xffff;	
			d_w = param2 & 0xffff;
			d_h = (param2>>16) & 0xffff;
			
			hw->SWIDTH_STEP = 256*s_w/d_w;
			hw->SHEIGH_STEP = 256*s_h/d_h;
		break;

		case SCALE_IOCTL_CMD_SET_START_ADDR:
			s_x = param1 & 0xffff;
			s_y = (param1>>16) & 0xffff;
			hw->SSTART = (s_x) |(s_y<<16);
		break;


		case SCALE_IOCTL_CMD_SET_LINE_BUF_NUM:
			hw->INBUF_LINE_NUM = param1;
		break;

		case SCALE_IOCTL_CMD_SET_IN_Y_ADDR:
			hw->INYDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_IN_U_ADDR:
			hw->INUDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_IN_V_ADDR:
			hw->INVDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_OUT_Y_ADDR:
			hw->OUTYDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_OUT_U_ADDR:
			hw->OUTUDMA_STADR = param1;
		break;

		case SCALE_IOCTL_CMD_SET_OUT_V_ADDR:
			hw->OUTVDMA_STADR = param1;
		break;
		
		case SCALE_IOCTL_CMD_DMA_TO_MEMORY:
			if(param1){
				hw->SCALECON |= BIT(1);
			}else{
				hw->SCALECON &= ~BIT(1);
			}
		break;

		case SCALE_IOCTL_CMD_SET_DATA_FROM:
			if(param1)
				hw->SCALECON &=~BIT(2);				//vpp
			else
				hw->SCALECON |= BIT(2);				//soft
		break;

		case SCALE_IOCTL_CMD_GET_INBUF_NUM:
			ret_val = (hw->INBUFCON>>16)&0x3ff;
		break;

		case SCALE_IOCTL_CMD_SET_INBUF_NUM:
			//os_printf("set inbuf_num:%d %d\r\n",param1,param2);
			hw->INBUFCON = ((param1)<<16)|(param2);
		break;

		case SCALE_IOCTL_CMD_SET_NEW_FRAME:
			hw->INBUFCON |= BIT(15);
		break;

		case SCALE_IOCTL_CMD_SET_END_FRAME:
			hw->INBUFCON |= BIT(31);
		break;
		
		case SCALE_IOCTL_CMD_GET_HEIGH_CNT:
			ret_val = hw->SHEIGH_CNT;
		break;


		default:
			os_printf("NO SCALE3 IOCTL:%d\r\n",ioctl_cmd);
            ret_val = -ENOTSUPP;
        break;
	}

	return ret_val;
}


void irq_scale1_enable(struct scale_device *p_scale,uint8 mode,uint8 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	if(mode){
		hw->SCALECON |= BIT(irq+16);
	}else{
		hw->SCALECON &= ~BIT(irq+16);
	}
}

void irq_scale2_enable(struct scale_device *p_scale,uint8 mode,uint8 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	if(mode){
		hw->SCALECON |= BIT(irq+16);
	}else{
		hw->SCALECON &= ~BIT(irq+16);
	}
}

void irq_scale3_enable(struct scale_device *p_scale,uint8 mode,uint8 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	if(mode){
		hw->SCALECON |= BIT(irq+16);
	}else{
		hw->SCALECON &= ~BIT(irq+16);
	}
}

void SCALE1_IRQHandler_action(void *p_scale)
{
	uint32 sta = 0;
	uint8 loop;
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	sta = hw->SCALESTA;
	for(loop = 0;loop < SCALE_IRQ_NUM;loop++){
		if(sta&BIT(loop)){
			hw->SCALESTA = BIT(loop);
			if(scaleirq1_vector_table[loop] != NULL)
				scaleirq1_vector_table[loop] (loop,scaleirq1_dev_table[loop],0);
		}
	}
}


int32 scaleirq1_register(struct scale_device *p_scale,uint32 irq, scale_irq_hdl isr, uint32 dev_id){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 	
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	request_irq(scale_hw->irq_num, SCALE1_IRQHandler_action, p_scale);
	
	irq_scale1_enable(p_scale, 1, irq);
	scaleirq1_vector_table[irq] = isr;
	scaleirq1_dev_table[irq] = dev_id;
	hw->SCALESTA |= BIT(irq);
	os_printf("scaleirq1_register:%d %x  %x\r\n",irq,(uint32)scaleirq1_vector_table[irq],(uint32)isr);
	return 0;
}


int32 scaleirq1_unregister(struct scale_device *p_scale,uint32 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	irq_scale1_enable(p_scale, 0, irq);
	scaleirq1_vector_table[irq] = NULL;
	scaleirq1_dev_table[irq] = 0;
	hw->SCALESTA |= BIT(irq);
	return 0;
}


void SCALE2_IRQHandler_action(void *p_scale)
{
	uint32 sta = 0;
	uint8 loop;
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	sta = hw->SCALESTA;
	for(loop = 0;loop < SCALE_IRQ_NUM;loop++){
		if(sta&BIT(loop)){
			hw->SCALESTA = BIT(loop);
			if(scaleirq2_vector_table[loop] != NULL)
				scaleirq2_vector_table[loop] (loop,scaleirq2_dev_table[loop],0);
		}
	}
}


int32 scaleirq2_register(struct scale_device *p_scale,uint32 irq, scale_irq_hdl isr, uint32 dev_id){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 	
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	request_irq(scale_hw->irq_num, SCALE2_IRQHandler_action, p_scale);
	
	irq_scale2_enable(p_scale, 1, irq);
	scaleirq2_vector_table[irq] = isr;
	scaleirq2_dev_table[irq] = dev_id;
	hw->SCALESTA |= BIT(irq);
	os_printf("scaleirq2_register:%d %x  %x\r\n",irq,(uint32)scaleirq2_vector_table[irq],(uint32)isr);
	return 0;
}


int32 scaleirq2_unregister(struct scale_device *p_scale,uint32 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	irq_scale2_enable(p_scale, 0, irq);
	scaleirq2_vector_table[irq] = NULL;
	scaleirq2_dev_table[irq] = 0;
	hw->SCALESTA |= BIT(irq);
	return 0;
}


void SCALE3_IRQHandler_action(void *p_scale)
{
	uint32 sta = 0;
	uint8 loop;
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	sta = hw->SCALESTA;
	for(loop = 0;loop < SCALE_IRQ_NUM;loop++){
		if(sta&BIT(loop)){
			hw->SCALESTA = BIT(loop);
			if(scaleirq3_vector_table[loop] != NULL)
				scaleirq3_vector_table[loop] (loop,scaleirq3_dev_table[loop],0);
		}
	}
}


int32 scaleirq3_register(struct scale_device *p_scale,uint32 irq, scale_irq_hdl isr, uint32 dev_id){
	struct hgscale *scale_hw = (struct hgscale*)p_scale; 	
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	request_irq(scale_hw->irq_num, SCALE3_IRQHandler_action, p_scale);
	
	irq_scale3_enable(p_scale, 1, irq);
	scaleirq3_vector_table[irq] = isr;
	scaleirq3_dev_table[irq] = dev_id;
	hw->SCALESTA |= BIT(irq);
	os_printf("scaleirq3_register:%d %x  %x\r\n",irq,(uint32)scaleirq3_vector_table[irq],(uint32)isr);
	return 0;
}


int32 scaleirq3_unregister(struct scale_device *p_scale,uint32 irq){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	irq_scale3_enable(p_scale, 0, irq);
	scaleirq3_vector_table[irq] = NULL;
	scaleirq3_dev_table[irq] = 0;
	hw->SCALESTA |= BIT(irq);
	return 0;
}


static int32 hgscale1_open(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	scale_hw->opened = 1;
	scale_hw->dsleep = 0;
	irq_enable(scale_hw->irq_num);
	hw->SCALECON |= BIT(0);	 //enable
	return 0;
}

static int32 hgscale1_close(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale1_hw *hw  = (struct hgscale1_hw *)scale_hw->hw;
	scale_hw->opened = 0;
	scale_hw->dsleep = 0;
	hw->SCALECON &= ~BIT(0);  //disable
	return 0;
}

static int32 hgscale2_open(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	scale_hw->opened = 1;
	scale_hw->dsleep = 0;
	irq_enable(scale_hw->irq_num);
	hw->SCALECON |= BIT(0);	 //enable
	return 0;
}

static int32 hgscale2_close(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale2_hw *hw  = (struct hgscale2_hw *)scale_hw->hw;
	scale_hw->opened = 0;
	scale_hw->dsleep = 0;
	hw->SCALECON &= ~BIT(0);  //disable
	return 0;
}

static int32 hgscale3_open(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	scale_hw->opened = 1;
	scale_hw->dsleep = 0;
	irq_enable(scale_hw->irq_num);
	hw->SCALECON |= BIT(0);	 //enable
	return 0;
}

static int32 hgscale3_close(struct scale_device *p_scale){
	struct hgscale *scale_hw = (struct hgscale*)p_scale;	
	struct hgscale3_hw *hw  = (struct hgscale3_hw *)scale_hw->hw;
	scale_hw->opened = 0;
	scale_hw->dsleep = 0;
	hw->SCALECON &= ~BIT(0);  //disable
	return 0;
}

int32 hgscale1_suspend(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale1_hw *hw;
	struct hgscale1_hw *hw_cfg;
	//ȷ���Ѿ����򿪲������߹�,ֱ�ӷ���
	if(!scale_hw->opened || scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 1;
	scale_hw->cfg_backup = (uint32 *)os_malloc(sizeof(struct hgscale1_hw));
	hw_cfg = (struct hgscale1_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale1_hw*)scale_hw->hw;
	hw_cfg->SCALECON 		= hw->SCALECON;
	hw_cfg->SWINCON 		= hw->SWINCON;
	hw_cfg->TWINCON 		= hw->TWINCON;
	hw_cfg->SWIDTH_STEP 	= hw->SWIDTH_STEP;
	hw_cfg->SHEIGH_STEP 	= hw->SHEIGH_STEP;
	hw_cfg->INBUF_LINE_NUM 	= hw->INBUF_LINE_NUM;
	hw_cfg->INYDMA_STADR 	= hw->INYDMA_STADR;
	hw_cfg->INUDMA_STADR 	= hw->INUDMA_STADR;
	hw_cfg->INVDMA_STADR 	= hw->INVDMA_STADR;
	hw_cfg->INBUFCON 		= hw->INBUFCON;
	hw_cfg->SHEIGH_CNT 		= hw->SHEIGH_CNT;
	hw_cfg->SCALESTA 		= hw->SCALESTA;	
	irq_disable(scale_hw->irq_num);
	return 0;
}

int32 hgscale1_resume(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale1_hw *hw;
	struct hgscale1_hw *hw_cfg;
	//����Ѿ����򿪲���û�����߹�,ֱ�ӷ���
	if(!scale_hw->opened || !scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 0;	
	hw_cfg = (struct hgscale1_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale1_hw*)scale_hw->hw;
	hw->SCALECON 		= hw_cfg->SCALECON;
	hw->SWINCON 		= hw_cfg->SWINCON;
	hw->TWINCON 		= hw_cfg->TWINCON;
	hw->SWIDTH_STEP 	= hw_cfg->SWIDTH_STEP;
	hw->SHEIGH_STEP 	= hw_cfg->SHEIGH_STEP;
	hw->INBUF_LINE_NUM 	= hw_cfg->INBUF_LINE_NUM;
	hw->INYDMA_STADR 	= hw_cfg->INYDMA_STADR;
	hw->INUDMA_STADR 	= hw_cfg->INUDMA_STADR;
	hw->INVDMA_STADR 	= hw_cfg->INVDMA_STADR;
	hw->INBUFCON 		= hw_cfg->INBUFCON;
	hw->SHEIGH_CNT 		= hw_cfg->SHEIGH_CNT;
	hw->SCALESTA 		= hw_cfg->SCALESTA;	
	irq_enable(scale_hw->irq_num);
	os_free(scale_hw->cfg_backup);
	return 0;
}

int32 hgscale2_suspend(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale2_hw *hw;
	struct hgscale2_hw *hw_cfg;
	//ȷ���Ѿ����򿪲������߹�,ֱ�ӷ���
	if(!scale_hw->opened || scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 1;
	scale_hw->cfg_backup = (uint32 *)os_malloc(sizeof(struct hgscale2_hw));
	hw_cfg = (struct hgscale2_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale2_hw*)scale_hw->hw;
	hw_cfg->SCALECON 		= hw->SCALECON;
	hw_cfg->SWINCON 		= hw->SWINCON;
	hw_cfg->SSTART 			= hw->SSTART;
	hw_cfg->TWINCON 		= hw->TWINCON;
	hw_cfg->SWIDTH_STEP 	= hw->SWIDTH_STEP;
	hw_cfg->SHEIGH_STEP 	= hw->SHEIGH_STEP;
	hw_cfg->SRAMBUF_STADR 	= hw->SRAMBUF_STADR;
	hw_cfg->OUTYDMA_STADR 	= hw->OUTYDMA_STADR;
	hw_cfg->OUTUDMA_STADR 	= hw->OUTUDMA_STADR;
	hw_cfg->OUTVDMA_STADR 	= hw->OUTVDMA_STADR;
	hw_cfg->SHEIGH_CNT 		= hw->SHEIGH_CNT;
	hw_cfg->SCALESTA 		= hw->SCALESTA;	
	irq_disable(scale_hw->irq_num);
	return 0;
}

int32 hgscale2_resume(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale2_hw *hw;
	struct hgscale2_hw *hw_cfg;
	//����Ѿ����򿪲���û�����߹�,ֱ�ӷ���
	if(!scale_hw->opened || !scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 0;	
	hw_cfg = (struct hgscale2_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale2_hw*)scale_hw->hw;
	hw->SCALECON 		= hw_cfg->SCALECON;
	hw->SWINCON 		= hw_cfg->SWINCON;
	hw->SSTART 			= hw_cfg->SSTART;
	hw->TWINCON 		= hw_cfg->TWINCON;
	hw->SWIDTH_STEP 	= hw_cfg->SWIDTH_STEP;
	hw->SHEIGH_STEP 	= hw_cfg->SHEIGH_STEP;
	hw->SRAMBUF_STADR 	= hw_cfg->SRAMBUF_STADR;
	hw->OUTYDMA_STADR 	= hw_cfg->OUTYDMA_STADR;
	hw->OUTUDMA_STADR 	= hw_cfg->OUTUDMA_STADR;
	hw->OUTVDMA_STADR 	= hw_cfg->OUTVDMA_STADR;
	hw->SHEIGH_CNT 		= hw_cfg->SHEIGH_CNT;
	hw->SCALESTA 		= hw_cfg->SCALESTA;	
	irq_enable(scale_hw->irq_num);
	os_free(scale_hw->cfg_backup);
	return 0;
}


int32 hgscale3_suspend(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale3_hw *hw;
	struct hgscale3_hw *hw_cfg;
	//ȷ���Ѿ����򿪲������߹�,ֱ�ӷ���
	if(!scale_hw->opened || scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 1;
	scale_hw->cfg_backup = (uint32 *)os_malloc(sizeof(struct hgscale3_hw));
	hw_cfg = (struct hgscale3_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale3_hw*)scale_hw->hw;
	hw_cfg->SCALECON 		= hw->SCALECON;
	hw_cfg->SWINCON 		= hw->SWINCON;
	hw_cfg->SSTART 			= hw->SSTART;
	hw_cfg->TWINCON 		= hw->TWINCON;
	hw_cfg->SWIDTH_STEP 	= hw->SWIDTH_STEP;
	hw_cfg->SHEIGH_STEP 	= hw->SHEIGH_STEP;
	hw_cfg->INBUF_LINE_NUM 	= hw->INBUF_LINE_NUM;

	hw_cfg->INYDMA_STADR 	= hw->INYDMA_STADR;
	hw_cfg->INUDMA_STADR 	= hw->INUDMA_STADR;
	hw_cfg->INVDMA_STADR 	= hw->INVDMA_STADR;

	hw_cfg->OUTYDMA_STADR 	= hw->OUTYDMA_STADR;
	hw_cfg->OUTUDMA_STADR 	= hw->OUTUDMA_STADR;
	hw_cfg->OUTVDMA_STADR 	= hw->OUTVDMA_STADR;
	hw_cfg->SHEIGH_CNT 		= hw->SHEIGH_CNT;
	hw_cfg->SCALESTA 		= hw->SCALESTA;	
	irq_disable(scale_hw->irq_num);
	return 0;
}

int32 hgscale3_resume(struct dev_obj *obj){
	struct hgscale *scale_hw = (struct hgscale*)obj;
	struct hgscale3_hw *hw;
	struct hgscale3_hw *hw_cfg;
	//����Ѿ����򿪲���û�����߹�,ֱ�ӷ���
	if(!scale_hw->opened || !scale_hw->dsleep)
	{
		return RET_OK;
	}
	scale_hw->dsleep = 0;	
	hw_cfg = (struct hgscale3_hw*)scale_hw->cfg_backup;
	hw     = (struct hgscale3_hw*)scale_hw->hw;
	hw->SCALECON 		= hw_cfg->SCALECON;
	hw->SWINCON 		= hw_cfg->SWINCON;
	hw->SSTART 			= hw_cfg->SSTART;
	hw->TWINCON 		= hw_cfg->TWINCON;
	hw->SWIDTH_STEP 	= hw_cfg->SWIDTH_STEP;
	hw->SHEIGH_STEP 	= hw_cfg->SHEIGH_STEP;
	hw->INBUF_LINE_NUM 	= hw_cfg->INBUF_LINE_NUM;
	
	hw->INYDMA_STADR 	= hw_cfg->INYDMA_STADR;
	hw->INUDMA_STADR 	= hw_cfg->INUDMA_STADR;
	hw->INVDMA_STADR 	= hw_cfg->INVDMA_STADR;

	hw->OUTYDMA_STADR 	= hw_cfg->OUTYDMA_STADR;
	hw->OUTUDMA_STADR 	= hw_cfg->OUTUDMA_STADR;
	hw->OUTVDMA_STADR 	= hw_cfg->OUTVDMA_STADR;
	hw->SHEIGH_CNT 		= hw_cfg->SHEIGH_CNT;
	hw->SCALESTA 		= hw_cfg->SCALESTA;	
	irq_enable(scale_hw->irq_num);
	os_free(scale_hw->cfg_backup);
	return 0;
}


static const struct scale_hal_ops dev1_ops = {
    .open        = hgscale1_open,
    .close       = hgscale1_close,
    .ioctl       = hgscale1_ioctl,
    .request_irq = scaleirq1_register,
    .release_irq = scaleirq1_unregister,
#ifdef CONFIG_SLEEP	
	.ops.suspend = hgscale1_suspend,
	.ops.resume  = hgscale1_resume,
#endif      
};

static const struct scale_hal_ops dev2_ops = {
    .open        = hgscale2_open,
    .close       = hgscale2_close,
    .ioctl       = hgscale2_ioctl,
    .request_irq = scaleirq2_register,
    .release_irq = scaleirq2_unregister,
#ifdef CONFIG_SLEEP	
	.ops.suspend = hgscale2_suspend,
	.ops.resume  = hgscale2_resume,
#endif      
};

static const struct scale_hal_ops dev3_ops = {
    .open        = hgscale3_open,
    .close       = hgscale3_close,
    .ioctl       = hgscale3_ioctl,
    .request_irq = scaleirq3_register,
    .release_irq = scaleirq3_unregister,
#ifdef CONFIG_SLEEP	
	.ops.suspend = hgscale3_suspend,
	.ops.resume  = hgscale3_resume,
#endif      
};

int32 hgscale1_attach(uint32 dev_id, struct hgscale *scale){
    scale->opened          = 0;
    scale->use_dma         = 0;
    scale->irq_hdl                   = NULL;
    //memset(dvp->irq_hdl,0,sizeof(dvp->irq_hdl));
    scale->irq_data                  = 0;
	//memset(dvp->irq_data,0,sizeof(dvp->irq_data));

	scale->dev.dev.ops = (const struct devobj_ops *)&dev1_ops;

    irq_disable(scale->irq_num);
    dev_register(dev_id, (struct dev_obj *)scale);	
	return 0;
}


int32 hgscale2_attach(uint32 dev_id, struct hgscale *scale){
    scale->opened          = 0;
    scale->use_dma         = 0;
    scale->irq_hdl                   = NULL;
    //memset(dvp->irq_hdl,0,sizeof(dvp->irq_hdl));
    scale->irq_data                  = 0;
	//memset(dvp->irq_data,0,sizeof(dvp->irq_data));

	scale->dev.dev.ops = (const struct devobj_ops *)&dev2_ops;

    irq_disable(scale->irq_num);
    dev_register(dev_id, (struct dev_obj *)scale);	
	return 0;
}


int32 hgscale3_attach(uint32 dev_id, struct hgscale *scale){
    scale->opened          = 0;
    scale->use_dma         = 0;
    scale->irq_hdl                   = NULL;
    //memset(dvp->irq_hdl,0,sizeof(dvp->irq_hdl));
    scale->irq_data                  = 0;
	//memset(dvp->irq_data,0,sizeof(dvp->irq_data));
	scale->dev.dev.ops = (const struct devobj_ops *)&dev3_ops;


    irq_disable(scale->irq_num);
    dev_register(dev_id, (struct dev_obj *)scale);	
	return 0;
}



