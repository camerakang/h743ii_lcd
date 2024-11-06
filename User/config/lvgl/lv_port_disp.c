/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "../../lvgl.h"

#include "lcd_rgb.h"
#include "tim.h"
#include "stm32h7xx_hal_ltdc.h"
#include "string.h"
/*********************
 *      DEFINES
 *********************/

extern LTDC_HandleTypeDef hltdc; // LTDC句柄

#define LVGL_MemoryAdd (LCD_MemoryAdd + LCD_Width * LCD_Height * BytesPerPixel_0) // 显示缓冲区地址

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
// static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//         const lv_area_t * fill_area, lv_color_t color);
 static lv_color_t *buf_3_1 = (lv_color_t *)(LVGL_MemoryAdd);                                               /*A screen sized buffer*/
    static lv_color_t *buf_3_2 = (lv_color_t *)(LVGL_MemoryAdd + LCD_Width * LCD_Height * sizeof(lv_color_t)); /*Another screen sized buffer*/
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 2) */
    //      static lv_disp_draw_buf_t draw_buf_dsc_2;
    //      static lv_color_t *buf_2_1 = (lv_color_t * )(0x2403FC00);
    //		static lv_color_t *buf_2_2 = (lv_color_t * )(0x24057300);
    //		lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, LCD_Width * 100);   /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    // static lv_color_t *buf_3_1 = (lv_color_t *)(LVGL_MemoryAdd);                                               /*A screen sized buffer*/
    // static lv_color_t *buf_3_2 = (lv_color_t *)(LVGL_MemoryAdd + LCD_Width * LCD_Height * sizeof(lv_color_t)); /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, LCD_Width * LCD_Height);                          /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);   /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LCD_Width;
    disp_drv.ver_res = LCD_Height;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;

    /*Required for Example 3)*/
    disp_drv.full_refresh = 1; // 双全缓冲需要打开此设置

    // disp_drv.sw_rotate = 1;
    // disp_drv.rotated = LV_DISP_ROT_90;
    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    // disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    HAL_TIM_Base_Start_IT(&htim17);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    
    SCB_CleanInvalidateDCache();
    while(!(LTDC->CDSR & LTDC_CDSR_VSYNCS));
    LTDC_Layer1->CFBAR = (uint32_t)color_p; // 切换显存地址
    // memcpy(buf_3_1,buf_3_2,LCD_Width * LCD_Height );
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/**
 * @brief  Line Event callback.
 * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
 *                the configuration information for the specified LTDC.
 * @retval None
 */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
    // 重新载入参数，新显存地址生效，此时显示才会更新
    // 每次进入中断才会更新显示，这样能有效避免撕裂现象
    __HAL_LTDC_RELOAD_CONFIG(hltdc);
    HAL_LTDC_ProgramLineEvent(hltdc, 0); // 重新设置中断
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
