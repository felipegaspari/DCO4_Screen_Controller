SET(SOURCES ui_comp_bar2.c
    ui_comp_barcyberorange.c
    ui_comp.c
    ui_Main.c
    ui_Main1.c
    ui_MANUALCALIBRATION.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_techno_background_with_connecting_grid_2201_png.c
    fonts/ui_font_Square140.c
    fonts/ui_font_Square22.c
    fonts/ui_font_radj148Semi.c
    fonts/ui_font_radj22.c
    fonts/ui_font_radj22SEMI.c
    fonts/ui_font_radj26.c
    fonts/ui_font_radj26Medium.c
    fonts/ui_font_radjLight140.c
    fonts/ui_font_radjSemiBold140.c)

add_library(ui ${SOURCES})
