SET(SOURCES screens/ui_Screen1.c
    screens/ui_Screen2.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_single6_png.c
    images/ui_img_battery2_png.c
    images/ui_img_ue_png.c
    images/ui_img_motor_disconnected_png.c)

add_library(ui ${SOURCES})
