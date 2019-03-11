
#include "osCore.h"
#include "hal_key.h"
#include "driver/gpio.h"
#include "external_include.h"

struct Key_t Button;

#define GPIO_KEY_NUM0   0//4//0
#define GPIO_KEY_MASK0	(1<<GPIO_KEY_NUM0)

#define GPIO_KEY_NUM1   25
#define GPIO_KEY_MASK1	(((uint64_t)1)<<GPIO_KEY_NUM1)

#define GPIO_KEY_NUM2   26
#define GPIO_KEY_MASK2	(((uint64_t)1)<<GPIO_KEY_NUM2)

#define GPIO_KEY_NUM3   27
#define GPIO_KEY_MASK3	(((uint64_t)1)<<GPIO_KEY_NUM3)

static void FUC_ATTR hal_key_scan(int tick)
{
	uint8 temp;
	temp=0x00;
	if(!(gpio_input_get()&GPIO_KEY_MASK0))
		temp|=KEY0;
	if(!(gpio_input_get()&(GPIO_KEY_MASK1)))
		temp|=KEY1;
	if(!(gpio_input_get()&(GPIO_KEY_MASK2)))
		temp|=KEY2;
	if(!(gpio_input_get()&(GPIO_KEY_MASK3)))
		temp|=KEY3;
	if(temp)
	{

		if(Button.keep==0x00)
		{
			Button.keep=0x01;
			Button.down=0x01;
			Button.key=temp;
			Button.runtime=0;
			Button.up=0x00;
		}
		else
		{
			Button.runtime+=tick;
		}
	}
	else
	{
		if(Button.keep)
		{
			Button.keep=0x00;
			Button.up=0x01;
			Button.runtime=0;
		}
	}
}
void FUC_ATTR hal_key_init()
{
    gpio_config_t io_conf;
    osMemset(&io_conf,0,sizeof(io_conf));
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_KEY_MASK0|GPIO_KEY_MASK1|GPIO_KEY_MASK2|GPIO_KEY_MASK3;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}
int FUC_ATTR hal_key_keeptime(void)
{
  return Button.runtime;
}
uint8 FUC_ATTR hal_get_key(uint8 *val, int tick)
{
  hal_key_scan(tick);
  *val=Button.key;
  if(Button.plong)
  {
	Button.plong=0;
	return  HAL_KEY_LONG;
  }
  else if(Button.up)
  {
	  Button.up=0;
	  return HAL_KEY_UP;
  }
  else if(Button.down)
  {
      Button.down=0;
      return HAL_KEY_DOWN;
  }
  else if(Button.keep)
  {
    return HAL_KEY_KEEP;
  }
  *val=0x00;
  return 0x00;
}
