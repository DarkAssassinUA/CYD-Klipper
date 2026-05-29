#include "panel.h"
#include <stdio.h>
#include "../ui_utils.h"
#include "../../core/printer_integration.hpp"
#include "../../core/current_printer.h"

// Custom fonts (large for current temp, small for target)
extern const lv_font_t lv_font_montserrat_28;
extern const lv_font_t lv_font_montserrat_16;
extern const lv_font_t lv_font_montserrat_22;

static void update_hotend_temps(lv_event_t* e) {
    lv_obj_t ** labels = (lv_obj_t **)lv_event_get_user_data(e);
    lv_obj_t * current_label = labels[0];
    lv_obj_t * target_label = labels[1];

    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    
    lv_label_set_text_fmt(current_label, "%.0f°C", temp);
    if(target > 0) {
        lv_label_set_text_fmt(target_label, "Цель: %.0f°C", target);
    } else {
        lv_label_set_text(target_label, "Выкл");
    }
}

static void update_bed_temps(lv_event_t* e) {
    lv_obj_t ** labels = (lv_obj_t **)lv_event_get_user_data(e);
    lv_obj_t * current_label = labels[0];
    lv_obj_t * target_label = labels[1];

    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    
    lv_label_set_text_fmt(current_label, "%.0f°C", temp);
    if(target > 0) {
        lv_label_set_text_fmt(target_label, "Цель: %.0f°C", target);
    } else {
        lv_label_set_text(target_label, "Выкл");
    }
}

static void update_progress(lv_event_t* e){
    lv_obj_t ** objs = (lv_obj_t **)lv_event_get_user_data(e);
    lv_obj_t * bar = objs[0];
    lv_obj_t * label = objs[1];

    float progress = get_current_printer_data()->print_progress * 100;
    lv_bar_set_value(bar, (int)progress, LV_ANIM_ON);
    lv_label_set_text_fmt(label, "%.1f%%", progress);
}

static void update_status_label_dash(lv_event_t * e){
    lv_obj_t * label = lv_event_get_target(e);
    PrinterState state = get_current_printer_data()->state;
    if(state == PrinterState::PrinterStatePrinting) {
        lv_label_set_text_fmt(label, "Статус: Печать %s", get_current_printer_data()->print_filename);
    } else if (state == PrinterState::PrinterStatePaused) {
        lv_label_set_text_fmt(label, "Статус: Пауза %s", get_current_printer_data()->print_filename);
    } else if (state == PrinterState::PrinterStateIdle) {
        lv_label_set_text(label, "Статус: Ожидание");
    } else {
        lv_label_set_text(label, "Статус: Отключен");
    }
}

static void update_time_label(lv_event_t * e){
    lv_obj_t * label = lv_event_get_target(e);
    unsigned long time = get_current_printer_data()->remaining_time_s;
    unsigned long hours = time / 3600;
    unsigned long minutes = (time % 3600) / 60;
    unsigned long seconds = (time % 3600) % 60;

    if (get_current_printer_data()->state == PrinterState::PrinterStatePrinting || get_current_printer_data()->state == PrinterState::PrinterStatePaused) {
        lv_label_set_text_fmt(label, "Осталось: %02lu:%02lu:%02lu", hours, minutes, seconds);
    } else {
        lv_label_set_text(label, "");
    }
}

static lv_obj_t* noz_labels[2];
static lv_obj_t* bed_labels[2];
static lv_obj_t* prog_objs[2];

void dashboard_panel_init(lv_obj_t* panel) {
    auto panel_width = CYD_SCREEN_PANEL_WIDTH_PX - CYD_SCREEN_GAP_PX * 2;
    auto panel_height = CYD_SCREEN_PANEL_HEIGHT_PX - CYD_SCREEN_GAP_PX * 2;

    lv_obj_t * container = lv_create_empty_panel(panel);
    lv_obj_set_size(container, panel_width, panel_height);
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

    // Nozzle Card
    lv_obj_t * noz_card = lv_obj_create(container);
    lv_obj_set_size(noz_card, 135, 100);
    lv_obj_align(noz_card, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_bg_color(noz_card, lv_color_hex(0x2A2A2A), 0);
    lv_obj_set_style_border_width(noz_card, 1, 0);
    lv_obj_set_style_border_color(noz_card, lv_color_hex(0x444444), 0);
    lv_obj_set_style_radius(noz_card, 10, 0);
    lv_obj_clear_flag(noz_card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * noz_title = lv_label_create(noz_card);
    lv_label_set_text(noz_title, LV_SYMBOL_WARNING " СОПЛО");
    lv_obj_set_style_text_color(noz_title, lv_color_hex(0x00E5FF), 0); // Cyan
    lv_obj_align(noz_title, LV_ALIGN_TOP_MID, 0, -10);

    lv_obj_t * noz_temp = lv_label_create(noz_card);
    lv_label_set_text(noz_temp, "0°C");
    lv_obj_set_style_text_font(noz_temp, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(noz_temp, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(noz_temp, LV_ALIGN_CENTER, 0, -5);

    lv_obj_t * noz_target = lv_label_create(noz_card);
    lv_label_set_text(noz_target, "Цель: 0°C");
    lv_obj_set_style_text_color(noz_target, lv_color_hex(0xAAAAAA), 0);
    lv_obj_align(noz_target, LV_ALIGN_BOTTOM_MID, 0, 10);

    noz_labels[0] = noz_temp;
    noz_labels[1] = noz_target;
    lv_obj_add_event_cb(noz_card, update_hotend_temps, LV_EVENT_MSG_RECEIVED, noz_labels);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, noz_card, NULL);

    // Bed Card
    lv_obj_t * bed_card = lv_obj_create(container);
    lv_obj_set_size(bed_card, 135, 100);
    lv_obj_align(bed_card, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(bed_card, lv_color_hex(0x2A2A2A), 0);
    lv_obj_set_style_border_width(bed_card, 1, 0);
    lv_obj_set_style_border_color(bed_card, lv_color_hex(0x444444), 0);
    lv_obj_set_style_radius(bed_card, 10, 0);
    lv_obj_clear_flag(bed_card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * bed_title = lv_label_create(bed_card);
    lv_label_set_text(bed_title, LV_SYMBOL_LIST " СТОЛ");
    lv_obj_set_style_text_color(bed_title, lv_color_hex(0x00E5FF), 0);
    lv_obj_align(bed_title, LV_ALIGN_TOP_MID, 0, -10);

    lv_obj_t * bed_temp = lv_label_create(bed_card);
    lv_label_set_text(bed_temp, "0°C");
    lv_obj_set_style_text_font(bed_temp, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(bed_temp, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(bed_temp, LV_ALIGN_CENTER, 0, -5);

    lv_obj_t * bed_target = lv_label_create(bed_card);
    lv_label_set_text(bed_target, "Цель: 0°C");
    lv_obj_set_style_text_color(bed_target, lv_color_hex(0xAAAAAA), 0);
    lv_obj_align(bed_target, LV_ALIGN_BOTTOM_MID, 0, 10);

    bed_labels[0] = bed_temp;
    bed_labels[1] = bed_target;
    lv_obj_add_event_cb(bed_card, update_bed_temps, LV_EVENT_MSG_RECEIVED, bed_labels);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, bed_card, NULL);

    // STATUS LABEL
    lv_obj_t * status_label = lv_label_create(container);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -80);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(status_label, panel_width - 20);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(status_label, "Статус: Ожидание");
    lv_obj_add_event_cb(status_label, update_status_label_dash, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, status_label, NULL);

    // PROGRESS BAR
    lv_obj_t * bar = lv_bar_create(container);
    lv_obj_set_size(bar, panel_width - 40, 24);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -45);
    lv_obj_set_style_radius(bar, 12, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, 12, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x00E5FF), LV_PART_INDICATOR); // Cyan

    lv_obj_t * pct_label = lv_label_create(bar);
    lv_obj_align(pct_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(pct_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(pct_label, "0.0%");

    prog_objs[0] = bar;
    prog_objs[1] = pct_label;
    lv_obj_add_event_cb(bar, update_progress, LV_EVENT_MSG_RECEIVED, prog_objs);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, bar, NULL);

    // TIME LABEL
    lv_obj_t * time_label = lv_label_create(container);
    lv_obj_align(time_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_label_set_text(time_label, "");
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xAAAAAA), 0);
    lv_obj_add_event_cb(time_label, update_time_label, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, time_label, NULL);
}
