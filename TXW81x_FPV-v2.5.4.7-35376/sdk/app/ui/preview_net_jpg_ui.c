/*********************************************************************************************************** 
 *  主要用于显示网络接收的图像数据
SR_OTHER_JPG    ---->   S_JPG_DECODE   ---->   R_VIDEO_P0
                        (320x240)              (320x240)   
********************************************************************************************************** */
#include "lvgl/lvgl.h"
#include "lvgl_ui.h"
#include "osal/string.h"
#include "stream_frame.h"
#include "custom_mem/custom_mem.h"
#include "other_jpg_show/other_jpg_show_stream.h"
//data申请空间函数
#define STREAM_MALLOC     custom_malloc_psram
#define STREAM_FREE       custom_free_psram
#define STREAM_ZALLOC     custom_zalloc_psram

//结构体申请空间函数
#define STREAM_LIBC_MALLOC     custom_malloc
#define STREAM_LIBC_FREE       custom_free
#define STREAM_LIBC_ZALLOC     custom_zalloc

extern lv_indev_t * indev_keypad;
extern lv_style_t g_style;
struct preview_net_jpg_s
{
    lv_group_t  *last_group;
    lv_obj_t    *base_ui;

    lv_group_t  *now_group;
    lv_obj_t    *now_ui;

//这个是需要的流,但命名不一定对应,有可能通过中间流进行中转实现
    stream *net_jpg_s;
    stream *decode_s;
};

static void exit_preview_net_jpg_ui(lv_event_t * e)
{
    struct preview_net_jpg_s *ui_s = (struct preview_net_jpg_s *)lv_event_get_user_data(e);
    lv_indev_set_group(indev_keypad, ui_s->last_group);
    lv_obj_clear_flag(ui_s->base_ui, LV_OBJ_FLAG_HIDDEN);
    lv_group_del(ui_s->now_group);
    ui_s->now_group = NULL;
    close_stream(ui_s->net_jpg_s);
    close_stream(ui_s->decode_s);
    lv_obj_del(ui_s->now_ui);
}

static void enter_preview_net_jpg_ui(lv_event_t * e)
{
    struct preview_net_jpg_s *ui_s = (struct preview_net_jpg_s *)lv_event_get_user_data(e);
    lv_obj_add_flag(ui_s->base_ui, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t *ui = lv_obj_create(lv_scr_act());  
    ui_s->now_ui = ui;
    lv_obj_add_style(ui, &g_style, 0);
    lv_obj_set_size(ui, LV_PCT(100), LV_PCT(100));
    ui_s->decode_s = jpg_decode_stream_not_bind(S_JPG_DECODE);
    //将解码的数据推送到Video P0显示
    streamSrc_bind_streamDest(ui_s->decode_s,R_VIDEO_P0); 
    enable_stream(ui_s->decode_s,1);  

    //接收网络的jpg数据,配置需要解码的参数(320x240),然后给到S_JPG_DECODE进行解码,最后给到P0去显示(设置时P0的类型)
    ui_s->net_jpg_s = other_jpg_stream_not_bind(SR_OTHER_JPG,320,240,320,240,YUV_P0);
    //将other_jpg的数据给到S_JPG_DECODE进行编码
    streamSrc_bind_streamDest(ui_s->net_jpg_s,S_JPG_DECODE);
    enable_stream(ui_s->net_jpg_s,1); 

    lv_group_t *group;
    group = lv_group_create();
    lv_indev_set_group(indev_keypad, group);

    lv_group_add_obj(group, ui);
    ui_s->now_group = group;
    lv_obj_add_event_cb(ui, exit_preview_net_jpg_ui, LV_EVENT_PRESSED, ui_s);
}

lv_obj_t *preview_net_jpg_ui(lv_group_t *group,lv_obj_t *base_ui)
{
    struct preview_net_jpg_s *ui_s = (struct preview_net_jpg_s*)STREAM_LIBC_ZALLOC(sizeof(struct preview_net_jpg_s));
	ui_s->last_group = group;
    ui_s->base_ui = base_ui;
    lv_obj_t *btn =  lv_list_add_btn(base_ui, NULL, "preview_net_jpg");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, enter_preview_net_jpg_ui, LV_EVENT_PRESSED, ui_s);
    return btn;
}
