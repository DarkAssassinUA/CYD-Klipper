#include "panel.h"
#include <stdio.h>
#include "../ui_utils.h"
#include "../../core/printer_integration.hpp"
#include "../../core/current_printer.h"

static void update_hotend_arc(lv_event_t* e) {
    lv_obj_t * arc = lv_event_get_target(e);
    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    
    if(target > 0) {
        lv_arc_set_value(arc, (int)temp);
        lv_arc_set_bg_end_angle(arc, 135 + (int)((target / 300.0f) * 270.0f)); 
    } else {
        lv_arc_set_value(arc, 0);
    }
}

static void update_hotend_label(lv_event_t* e) {
    lv_obj_t * label = lv_event_get_target(e);
    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexNozzle1];
    char buff[32];
    sprintf(buff, "%.0f\n/ %.0f", temp, target);
    lv_label_set_text(label, buff);
}

static void update_bed_arc(lv_event_t* e) {
    lv_obj_t * arc = lv_event_get_target(e);
    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    if(target > 0) {
        lv_arc_set_value(arc, (int)temp);
        lv_arc_set_bg_end_angle(arc, 135 + (int)((target / 120.0f) * 270.0f)); 
    } else {
        lv_arc_set_value(arc, 0);
    }
}

static void update_bed_label(lv_event_t* e) {
    lv_obj_t * label = lv_event_get_target(e);
    float temp = get_current_printer_data()->temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    float target = get_current_printer_data()->target_temperatures[PrinterTemperatureDeviceIndex::PrinterTemperatureDeviceIndexBed];
    char buff[32];
    sprintf(buff, "%.0f\n/ %.0f", temp, target);
    lv_label_set_text(label, buff);
}

static void progress_bar_update_dash(lv_event_t* e){
    lv_obj_t * bar = lv_event_get_target(e);
    lv_bar_set_value(bar, get_current_printer_data()->print_progress * 100, LV_ANIM_ON);
}

static void update_percentage_label_dash(lv_event_t * e){
    lv_obj_t * label = lv_event_get_target(e);
    char buff[12];
    sprintf(buff, "%.1f%%", get_current_printer_data()->print_progress * 100);
    lv_label_set_text(label, buff);
}

static void update_status_label_dash(lv_event_t * e){
    lv_obj_t * label = lv_event_get_target(e);
    PrinterState state = get_current_printer_data()->state;
    if(state == PrinterState::PrinterStatePrinting) {
        lv_label_set_text_fmt(label, "Печать: %s", get_current_printer_data()->print_filename);
    } else if (state == PrinterState::PrinterStatePaused) {
        lv_label_set_text_fmt(label, "Пауза: %s", get_current_printer_data()->print_filename);
    } else if (state == PrinterState::PrinterStateIdle) {
        lv_label_set_text(label, "Ожидание / Готов");
    } else {
        lv_label_set_text(label, "Отключен или Ошибка");
    }
}

void dashboard_panel_init(lv_obj_t* panel) {
    auto panel_width = CYD_SCREEN_PANEL_WIDTH_PX - CYD_SCREEN_GAP_PX * 2;
    auto panel_height = CYD_SCREEN_PANEL_HEIGHT_PX - CYD_SCREEN_GAP_PX * 2;

    lv_obj_t * container = lv_create_empty_panel(panel);
    lv_obj_set_size(container, panel_width, panel_height);
    lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

    // Style for Arcs
    static lv_style_t arc_style;
    lv_style_init(&arc_style);
    lv_style_set_arc_width(&arc_style, 10);
    
    static lv_style_t arc_indic_style;
    lv_style_init(&arc_indic_style);
    lv_style_set_arc_width(&arc_indic_style, 10);

    // HOTEND ARC
    lv_obj_t * hotend_arc = lv_arc_create(container);
    lv_obj_set_size(hotend_arc, 130, 130);
    lv_arc_set_rotation(hotend_arc, 135);
    lv_arc_set_bg_angles(hotend_arc, 0, 270);
    lv_arc_set_value(hotend_arc, 0);
    lv_arc_set_range(hotend_arc, 0, 300);
    lv_obj_remove_style(hotend_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(hotend_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(hotend_arc, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_add_style(hotend_arc, &arc_style, LV_PART_MAIN);
    lv_obj_add_style(hotend_arc, &arc_indic_style, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(hotend_arc, lv_color_hex(0xFF4500), LV_PART_INDICATOR); // Orange-Red
    lv_obj_add_event_cb(hotend_arc, update_hotend_arc, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, hotend_arc, NULL);

    lv_obj_t * hotend_icon = lv_label_create(container);
    lv_label_set_text(hotend_icon, LV_SYMBOL_WARNING " Сопло");
    lv_obj_align_to(hotend_icon, hotend_arc, LV_ALIGN_OUT_TOP_MID, 0, -5);

    lv_obj_t * hotend_label = lv_label_create(hotend_arc);
    lv_obj_align(hotend_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(hotend_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(hotend_label, "0\n/ 0");
    lv_obj_add_event_cb(hotend_label, update_hotend_label, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, hotend_label, NULL);

    // BED ARC
    lv_obj_t * bed_arc = lv_arc_create(container);
    lv_obj_set_size(bed_arc, 130, 130);
    lv_arc_set_rotation(bed_arc, 135);
    lv_arc_set_bg_angles(bed_arc, 0, 270);
    lv_arc_set_value(bed_arc, 0);
    lv_arc_set_range(bed_arc, 0, 120);
    lv_obj_remove_style(bed_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(bed_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(bed_arc, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_add_style(bed_arc, &arc_style, LV_PART_MAIN);
    lv_obj_add_style(bed_arc, &arc_indic_style, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(bed_arc, lv_color_hex(0x1E90FF), LV_PART_INDICATOR); // Blue
    lv_obj_add_event_cb(bed_arc, update_bed_arc, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, bed_arc, NULL);

    lv_obj_t * bed_icon = lv_label_create(container);
    lv_label_set_text(bed_icon, LV_SYMBOL_LIST " Стол");
    lv_obj_align_to(bed_icon, bed_arc, LV_ALIGN_OUT_TOP_MID, 0, -5);

    lv_obj_t * bed_label = lv_label_create(bed_arc);
    lv_obj_align(bed_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_align(bed_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bed_label, "0\n/ 0");
    lv_obj_add_event_cb(bed_label, update_bed_label, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, bed_label, NULL);

    // STATUS LABEL
    lv_obj_t * status_label = lv_label_create(container);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(status_label, panel_width - 20);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(status_label, "...");
    lv_obj_add_event_cb(status_label, update_status_label_dash, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, status_label, NULL);

    // PROGRESS BAR
    lv_obj_t * bar = lv_bar_create(container);
    lv_obj_set_size(bar, panel_width - 80, 20);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, -20, -20);
    lv_obj_add_event_cb(bar, progress_bar_update_dash, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, bar, NULL);

    lv_obj_t * pct_label = lv_label_create(container);
    lv_obj_align_to(pct_label, bar, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_label_set_text(pct_label, "0%");
    lv_obj_add_event_cb(pct_label, update_percentage_label_dash, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(DATA_PRINTER_DATA, pct_label, NULL);

}
