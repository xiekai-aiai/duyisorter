/*!
 * \Copyright   Copyright (C) 2013 Hefei Meyer Optoelectronic Technology Inc.\n
 *              All rights reserved.
 * \file        mylanguage.h
 * \brief       界面词根头文件
 * \date        2015.01.14
 */
#ifndef MYLANGUAGE_H
#define MYLANGUAGE_H

#include <QtCore>

/*!
 * \brief 界面词根类
 */
class MyLanguage : public QObject
{
public:
    MyLanguage(){}

    QString ok;
    QString cancel;
    QString enable;
    QString disable;
    QString back;
    QString space;
    QString input;
    QString del;
    QString on;
    QString off ;
    QString reboot;
    QString apply;
    QString name;
    QString list;
    QString start;
    QString stop;
    QString skip_preheat;
    QString page_communicate;
    QString communication_list;
    QString page_home;
    QString save_parameters;
    QString set_purview;
    QString enter_system;
    QString halt;
    QString engineering_mode;
    QString system_initial;
    QString preheat;
    QString proportion;
    QString background_lamp;
    QString sensitivity;
    QString sensitivity_minimum;
    QString sensitivity_maximum;
    QString sensitivity_set;
    QString vendor_mode;
    QString operator_mode;
    QString engineer;
    QString last_page;
    QString next_page;
    QString select_sensitivity;
    QString general;
    QString other;
    QString machine_info;
    QString engineer_info;
    QString vendor_info;
    QString device_info;
    QString device_id;
    QString times;
    QString set;
    QString capacity;
    QString clear;
    QString compute;
    QString weight;

    // delay code
    QString dc_input;
    QString dc_productsn;
    QString dc_productsn_mismatch;
    QString dc_save_erro;
    QString dc_illegal;
    QString dc_limit;
    QString dc_illegal_used;
    QString dc_limit_info;
    QString dc_rtc_erro;
    QString dc_cnt;

    // machine view & level
    QString chute;
    QString front;
    QString front_view;
    QString master_view;
    QString slave_view;
    QString rear;
    QString rear_view;
    QString front_rear_view;
    QString upper;
    QString mid;
    QString lower;
    QString upper_view;
    QString lower_view;
    QString first_front;
    QString first_rear;
    QString second_front;
    QString second_rear;
    QString third_front;
    QString third_rear;
    QString fourth_front;
    QString fourth_rear;
    QString first_upper;
    QString first_lower;
    QString second_upper;
    QString second_lower;
    QString first_down_front;
    QString first_up_front;
    QString first_down_rear;
    QString first_up_rear;
    QString second_down_front;
    QString second_up_front;
    QString second_down_rear;
    QString second_up_rear;
    QString upper_layer;
    QString lower_layer;
    QString mid_layer;
    QString layer;
    QString unit_info;
    QString first;
    QString second;
    QString third;
    QString fourth;
    QString enable_third_level;
    QString msg_channel_error;
    QString msg_need_first_unit;
    QString msg_need_second_unit;
    QString eject_num;
    QString front_sens;
    QString rear_sens;
    QString current_camera;

    //LD4&LDS2
    QString first_lower_layer;
    QString first_upper_layer;
    QString second_lower_layer;
    QString second_upper_layer;
    QString ld4_1st_upper_master;
    QString ld4_1st_lower_master;
    QString ld4_2nd_upper_master;
    QString ld4_2nd_lower_master;
    QString ld4_1st_upper_slave;
    QString ld4_1st_lower_slave;
    QString ld4_2nd_upper_slave;
    QString ld4_2nd_lower_slave;

    // ai
    QString ai_simulate;
    QString ai_basic;
    QString ai_more;
    QString ai_no_image;
    QString ai_reselect_whole;
    QString ai_reselect_last;
    QString ai_optimize_calculate;
    QString ai_optimize_sample;
    QString ai_set_material;
    QString ai_material;
    QString ai_calculate;
    QString ai_update_sample;
    QString ai_sample_color;
    QString ai_sample_sens;
    QString ai_trans_mode;
    QString ai_low_speed;
    QString ai_high_speed;
    QString ai_sample_mode;
    QString ai_trigger_threshold;
    QString ai_enable_mode;
    QString ai_normal;
    QString ai_infrared;
    QString ai_multi_mode;
    QString ai_material_name;
    QString ai_good;
    QString ai_bad;
    QString ai_cancel_optimize_calculate;
    QString ai_cancel_optimize_sample;
    QString ai_select_1000_pixels;
    QString ai_save_error;
    QString ai_set_good_bad;
    QString ai_set_more_material;
    QString ai_need_bad_material;
    QString ai_need_good_material;
    QString ai_name_material;
    QString ai_exceed_max_times;
    QString ai_image_capturing;
    QString ai_image_capture_error;
    QString ai_unsupervised_less_point;
    QString ai_unsupervised_more_point;
    QString ai_analysis;
    QString ai_analysis_set;
    QString ai_analysis_material;
    QString msg_computing ;
    QString msg_need_resample;
    QString msg_usb_insert;
    QString msg_copy_image;
    QString msg_export_image;
    QString msg_import_image;
    QString sd_no_imageBak_dir;
    QString dir_no_png_file;
    QString usb_no_imageBak_dir;
    QString select_png_dir;
    QString success;
    QString failed;
    QString msg_clear_local_pics;
    QString msg_set_material;
    QString msg_compute_error;
    QString msg_auto_computer_erro;
    QString msg_optimizing;
    QString cfm_reselect_sample;
    QString ai_color_space;
    QString ai_material_image;
    QString ai_auto_cal;
    QString ai_camera_mode;
    QString ai_del_img;
    QString ai_preview_mode;
    QString ai_cal_set_same;
    QString ai_cal_set_self;
    QString ai_class_mode_null;
    QString ai_class_mode_usual;
    QString ai_class_mode_multiple;
    QString ai_capture;
    QString ai_delete;
    QString ai_err_no_bad;
    QString ai_err_no_good;
    QString ai_err_no_set;
    QString ai_err_overflow;
    QString ai_cal_warn;
    QString ai_cal_open;
    QString ai_img_import;
    QString ai_img_export;
    QString ai_img_import_export;
    QString ai_img_threshold;
    QString ai_img_colorSpace;
    QString ai_img_colorSpace1;
    QString ai_img_colorSpace2;
    QString ai_img_msg_import;
    QString ai_img_msg_export;
    QString ai_mode;
    QString ai_mode1;
    QString ai_mode2;
    QString ai_start_purity;
    QString ai_unsupervisedLearning;
    QString ai_reserved_sens;
    QString ai_params_set;
    // material
    QString identify_params;
    QString infra_identify_params;
    QString material_general;
    QString infra_material_general;
    QString material_color;
    QString material_defect_grey_a;
    QString material_defect_grey_b ;
    QString material_defect_discolor_a;
    QString material_defect_discolor_b;
    QString material_cross;
    QString material_shape;
    QString material_shape_length;
    QString material_scale_params;
    QString material_shape_pole;
    QString material_shape_pole_a;
    QString material_shape_pole_b;
    QString material_shape_slice;
    QString material_shape_leaf;
    QString material_bud_1;
    QString material_bud_2;
    QString material_defect_a;
    QString material_defect_b;
    QString material_defect_c;
    QString material_defect_d;
    QString material_infra ;
    QString material_intelligent_mode_a;
    QString material_intelligent_mode_b;
    QString material_intelligent_mode_c ;
    QString material_intelligent_mode_d;
    QString material_tea;
    QString material_watermelon;
    QString material_reserved;
    QString material_maize_param;
    QString material_watermelon_edge_red;
    QString material_watermelon_red_sens;
    QString material_watermelon_red_ratio;
    QString material_watermelon_edge_damaged;
    QString material_watermelon_damaged_ratio;
    QString material_watermelon_damaged_sens;
    QString material_watermelon_rugged1;
    QString material_watermelon_rugged2;
    QString material_watermelon_white_small;
    QString material_watermelon_white_sens;
    QString material_watermelon_rugged_save_white;
    QString material_watermelon_rugged_save_black;
    QString material_watermelon_ai_whole;
    QString material_watermelon_ai_edge;
    QString cfm_use_watermelon;
    QString cfm_rename_impurity;
    QString defect_name ;
    QString mode;
    QString scale;
    QString purity;
    QString red;
    QString green;
    QString blue;
    QString dark;
    QString light;
    QString light_limit;
    QString red_green;
    QString red_blue;
    QString green_blue;
    QString sort_circular;
    QString sort_long;
    QString sort_short;
    QString sort_small;
    QString sort_big;
    QString params_limit;
    QString area;
    QString arithmetic_list;
    QString material_mode;
    QString first_second;
    QString all_seperate;
    QString front_rear;
    QString all_alike;
    QString reserved;
    QString reserved_1;
    QString reserved_2;
    QString reserved_dark_red;
    QString reserved_burnt;
    QString reserved_peeled;
    QString slice_sensitivity;
    QString slice_size;
    QString pole_size;
    QString grain_sensitivity;
    QString grain_size;
    QString balance;
    QString hard_code;
    QString min_limit;
    QString max_limit;
    QString threshold;

    // lamp
    QString lamp_front_bg_red;
    QString lamp_front_bg_green;
    QString lamp_front_bg_blue;
    QString lamp_rear_bg_red;
    QString lamp_rear_bg_green;
    QString lamp_rear_bg_blue;
    QString lamp_rear;
    QString lamp_rear_1;
    QString lamp_rear_2;
    QString lamp_rear_3;
    QString lamp_rear_4;
    QString lamp_front;
    QString lamp_front_1;
    QString lamp_front_2;
    QString lamp_front_3;
    QString lamp_upper_1;
    QString lamp_upper_2;
    QString lamp_upper_3;
    QString lamp_upper_4;
    QString lamp_upper_bg;
    QString lamp_low_1;
    QString lamp_low_2;
    QString lamp_low_3;
    QString lamp_low_4;
    QString lamp_low_bg;
    QString lamp_upper_bg_red;
    QString lamp_upper_bg_green;
    QString lamp_upper_bg_blue;
    QString lamp_low_bg_red;
    QString lamp_low_bg_green;
    QString lamp_low_bg_blue;
    QString lamp_mid_1;
    QString lamp_mid_2;
    QString lamp_mid_3;
    QString lamp_mid_4;
    QString lamp_mid_bg;
    QString lamp_rear_bg;
    QString lamp_front_bg;
    QString lamp_bg_red;
    QString lamp_bg_green;
    QString lamp_bg_blue;

    // feeder
    QString feeder_set;
    QString feeder;
    QString feed;
    QString feeder_ac;
    QString feedEnable;
    QString feeder_value;
    QString enable_all;
    QString disable_all;
    QString single;
    QString complex;
    QString feeder_level;
    QString feeder_empty;
    QString feeder_conbine;
    QString no_material;
    QString exist_material;
    // wipe
    QString wipe_set;
    QString wipe_duration;
    QString wipe_interval;
    QString wipe_enable;
    QString wipe_manual;
    QString wipe_wind;
    QString wipe_wind_duration;
    QString wipe_wind_interval;
    QString wipe_wind_manual;
    QString wipe_water;
    QString wipe_water_duration;
    QString wipe_water_delay;
    QString wipe_delay;

    // belt
    QString belt;

    // scheme
    QString select;
    QString scheme_select;
    QString scheme_parameter;
    QString scheme_manage;
    QString scheme_new;
    QString scheme_copy;
    QString scheme_delete;
    QString scheme_rename;
    QString scheme_restore;
    QString scheme_backup;
    QString scheme_change_mode;
    QString scheme_copy_major;
    QString cfm_scheme_create;
    QString cfm_scheme_copy;
    QString cfm_scheme_delete;
    QString msg_delete_forbidden;
    QString cfm_scheme_rename;
    QString msg_name_input;
    QString cfm_restore_from_local;
    QString cfm_restore_from_usb;
    QString msg_restore_from_usb_error;
    QString cfm_backup_to_local;
    QString cfm_backup_to_usb;
    QString cfm_select_current_scheme;
    QString cfm_change_profile_mode;
    QString msg_custom_language_input;
    QString msg_rename_custom_language;

    // background
    QString background_set;
    QString value;
    QString percent;
    QString black;
    QString white;
    QString automatic;
    QString msg_auto_bgk_ckeck_error;
    QString msg_strip_frame_data_error;
    // eject tick
    QString eject_on;
    QString eject_duration;
    QString eject_delay;
    QString eject_bias;
    QString eject_test;
    QString ejector_current;
    QString ejector_keep_loop;
    QString eject_time;
    QString eject_fast_test;
    QString lamp_control;
    QString tick_params;
    QString infra_tick_params;
    QString infra_background_set;
    QString fixed_mode;
    QString extended_mode;
    QString accurate_mode;
    QString eject_mode;
    QString infect_arithmetic;
    QString edge_cut;
    QString edge_revise;
    QString remove_joint;
    QString remove_tick;
    QString mat_width;
    QString bad_number;
    QString good_number;
    QString enable_balance;
    QString logic_or;
    QString logic_and;
    QString logical_relation;
    QString sort;
    QString reverse;
    QString sort_mode;
    QString unknown;
    QString enable_default;
    QString color_sort;
    QString shape_sort;
    QString huff_mode;
    QString huff_width;

    // system status
    QString system_stat;
    QString screen;
    QString interface_board;
    QString control_board;
    QString color_board;
    QString color;
    QString mono_board;
    QString infra_board;
    QString light_src_board;
    QString recheck;
    QString infra;
    QString software_version;
    QString normal;
    QString abnormal;
    QString eject_frequency;
    QString system_time;
    QString year;
    QString month;
    QString day;
    QString hour;
    QString minute;
    QString sec;
    QString information;
    QString this_run;
    QString this_feed_run;
    QString total_run;
    QString total_feed_run;
    QString this_pressure_alarm_times;
    QString total_pressure_alarm;
    QString feed_stat;
    QString eject_stat;
    QString belt_1_status;
    QString belt_2_status;
    QString alarm_set;
    QString warm;
    QString msg_pressure_alarm_check_pressure;
    QString msg_pressure_alarm;
    QString pressure_not_enough;
    QString blower_alarm;
    QString lamp_alarm;
    QString pressure_alarm;
    QString enable_press_alarm;
    QString enable_blower_alarm;
    QString enable_lamp_alarm;
    QString enable_belt_status;
    QString enable_temperature_alarm;
    QString enable_communication_alarm;
    QString auto_calibrate;
    QString auto_calibrate_tips;
    QString auto_calibrate_step1;
    QString auto_calibrate_step2;
    QString auto_calibrate_step3;
    QString auto_calibrate_step4;
    QString auto_calibrate_step5;
    QString auto_calibrate_step6;
    QString background_angle;
    QString howto;
    QString reload;
    QString one_key_feed;
    QString communication_alarm;
    QString level_stat;
    QString level_empty_alarm;
    QString level_full_alarm;
    QString level_sensor_alarm;
    QString contact_us;
    QString video;
    QString sorter_manager;
    QString auto_analysis;
    QString temperature;
    QString slave_list;

    // net
    QString remote_control;
    QString network_config;
    QString local_host;
    QString ip_address;
    QString subnet_mask;
    QString default_gateway;
    QString dns_server;
    QString mac_address;
    QString route_address;
    QString set_server;
    QString set_sms;
    QString receiver_num;
    QString sms_center_num;
    QString sms_content;
    QString sms_send;
    QString test_signal;
    QString obtain_auto;
    QString qrcode;
    QString qrcode_generator;
    QString devId;

    // factory set
    QString factory_set;
    QString language;
    QString product;
    QString clear_running_time;
    QString aging_test;
    QString upgrade_screen;
    QString upgrade_fpga;
    QString init_mode;
    QString ts_calibrate;
    QString throughput_test;
    QString run_mode;
    QString run_test;
    QString run_fre;
    QString test_time;
    QString mode_1;
    QString mode_2;
    QString mode_3;
    QString english;
    QString chinese_simplified;
    QString chinese_traditonal;
    QString russian;
    QString vietnam;
    QString thailand;
    QString spanish;
    QString turkey;
    QString farsi;
    QString french;
    QString uyghur;
    QString korean;
    QString arabic;
    QString bulgarian;
    QString slovak;
    QString portuguese;
    QString myanmar;
    QString bengali;
    QString indonesia;
    QString poland;
    QString cfm_change_language;
    QString cfm_clear_time;
    QString cfm_ts_calibrate;
    QString cfm_upgrade_screen;
    QString cfm_upgrade_fpga;
    QString feeder_voltage;
    QString feeder_voltage_high;
    QString feeder_voltage_low;
    QString standalone;
    QString master;
    QString slave;

    // remote control
    QString server;
    QString server_ip;
    QString server_port;
    QString local_virtual_ip;
    QString username;
    QString password;
    QString connect;
    QString disconnect;
    QString msg_connecting;
    QString msg_connect_fail;

    // big data
    QString big_data;
    QString data_center;
    QString value_added;
    QString service;

    // fpga upgrade
    QString msg_insert_udisk;
    QString last;
    QString next;
    QString msg_insert_data_wire;
    QString msg_remove_data_wire;
    QString user_mode;
    QString board_num;
    QString upgrade;
    QString upgrade_retry;
    QString upgrade_whole;
    QString upgrade_return;
    QString msg_int_to_factory;
    QString msg_int_download_file;
    QString msg_int_burn_file;
    QString msg_int_to_user;
    QString msg_color_download_file;
    QString msg_color_to_factory;
    QString msg_color_burn_file;
    QString msg_color_to_user;
    QString msg_inf_download_file;
    QString msg_inf_to_factory;
    QString msg_inf_burn_file;
    QString msg_inf_to_user;
    QString msg_ctrl_to_factory;
    QString msg_ctrl_erase;
    QString msg_ctrl_burn_file;
    QString msg_ctrl_to_user;

    QString msg_enable_upgrade_board;
    QString msg_major_to_factory;
    QString msg_major_file_to_int;
    QString msg_major_download_file;
    QString msg_major_burn_file;
    QString msg_major_to_user;
    QString msg_assist_to_factory;
    QString msg_assist_file_to_int;
    QString msg_assist_file_download_to_major;
    QString msg_assist_download_file;
    QString msg_assist_burn_file;
    QString msg_assist_to_user;

    QString upgrade_file_list;
    QString upgrade_usb_communication;
    QString upgrade_board;
    QString upgrade_doing;
    QString upgrade_fail;
    QString upgrade_untreated;
    QString upgrade_ok;
    QString msg_file_open_error;
    QString msg_no_inf;
    QString msg_no_color;
    QString msg_unknown_file;
    QString cfm_insert_datawire;
    QString msg_select_user_mode;
    QString cfm_upgrade_whole;
    QString select_file;
    QString msg_select_right_file;

    // color camera
    QString color_camera;
    QString confirm_set_camera;
    QString confirm_set_infra_camera;
    QString color_calibrate;
    QString color_gain_adjust;
    QString color_analog_gain;
    QString color_lens_params;
    QString color_biasing;
    QString color_digital_gain;
    QString color_digital_gain_set;
    QString color_sensor_params;
    QString gain_rough;
    QString gain_tiny;
    QString channel_begin;
    QString channel_end;
    QString channel_restore;
    QString sensor;
    QString row_fre;
    QString sensor_mode;
    QString sensor_light;
    QString sensor_dark;
    QString sensor_black_white;
    QString sensor_pixel;
    QString front_begin;
    QString front_end;
    QString rear_begin;
    QString rear_end;
    QString upper_view_begin;
    QString upper_view_end;
    QString lower_view_begin;
    QString lower_view_end;
    QString upper_layer_begin;
    QString upper_layer_end;
    QString lower_layer_begin;
    QString lower_layer_end;
    QString mid_layer_begin;
    QString mid_layer_end;
    QString begin;
    QString end;
    QString auto_divide;
    QString align;
    QString update_capture;
    QString update_save;
    QString update_capture_dark;
    QString update_save_dark;
    QString update_auto;
    QString update_auto_level;
    QString channel_divide;
    QString front_upper_view;
    QString front_lower_view;
    QString rear_upper_view;
    QString rear_lower_view;
    QString ejector;
    QString pixel;
    QString single_calibrate;
    QString whole_calibrate;
    QString default_params;
    QString auto_cal_note;
    QString auto_cal_lightset;
    QString auto_cal_gainset;
    QString auto_cal_get_target;
    QString rear_no_signal;
    QString front_no_signal;

    /* LD4 */
    QString upper_master_view;
    QString lower_master_view;
    QString upper_slave_view;
    QString lower_slave_view;

    QString front_upper_view_begin;
    QString front_lower_view_begin;
    QString rear_upper_view_begin;
    QString rear_lower_view_begin;
    QString front_upper_view_end;
    QString front_lower_view_end;
    QString rear_upper_view_end;
    QString rear_lower_view_end;
    QString left_side;
    QString right_side;
    QString upper_view_page;
    QString lower_view_page;
    QString front_view_page;
    QString rear_view_page;
    QString upper_layer_page;
    QString lower_layer_page;
    QString down_front_left_view_begin;
    QString down_front_left_view_end;
    QString down_front_right_view_begin;
    QString down_front_right_view_end;
    QString down_rear_left_view_begin;
    QString down_rear_left_view_end;
    QString down_rear_right_view_begin;
    QString down_rear_right_view_end;
    QString up_front_left_view_begin;
    QString up_front_left_view_end;
    QString up_front_right_view_begin;
    QString up_front_right_view_end;
    QString up_rear_left_view_begin;
    QString up_rear_left_view_end;
    QString up_rear_right_view_begin;
    QString up_rear_right_view_end;

    // infrared camera
    QString inf_camera;
    QString inf_gain;
    QString inf_channel;
    QString inf_biasing;
    QString inf_update;
    QString inf_odd;
    QString inf_even;
    QString inf_positive;
    QString inf_negative;
    QString inf_pixel_base;
    QString inf_reference;
    QString inf;

    // screen saver
    QString set_screensaver;
    QString enable_screensaver;
    QString input_screensaver_password;
    QString confirm_screensaver_password;

    // message
    QString msg_statistic;
    QString msg_applying;
    QString msg_turning_on;
    QString msg_turning_off;
    QString msg_turn_off_first;
    QString msg_shutdown;
    QString msg_wiping;
    QString msg_password_error;
    QString msg_divide_channel;
    QString msg_communicating;
    QString msg_require_version;
    QString msg_system_init;
    QString msg_preheating;
    QString msg_input_name;
    QString msg_mode_changing;
    QString msg_testing;
    QString msg_change_init_mode;
    QString msg_delay_get_zero;
    QString msg_saving_params;
    QString msg_params_save_error;
    QString msg_usb_init_error;
    QString timeout;
    QString msg_auto_calibrating;
    QString msg_auto_calibrate;
    QString msg_length_exceed_limit;
    QString msg_set_impure;

    // confirm
    QString cfm_save_parameter;
    QString cfm_turn_on_while_belt_off;
    QString cfm_shut_down;
    QString cfm_turn_on;
    QString cfm_auto_calibrate;
    QString cfm_copy_to_all;
    QString cfm_change_analog_gain;
    QString cfm_auto_set;
    QString cfm_set_background_color;

    QString init_autoSig;
    QString cfm_init_autoSig;
    QString msg_initing_autoSig;
    QString msg_init_autoSig_fail;
    QString cfm_reboot;

    // general
    QString config_major;
    QString config_assist;
    QString row;
    QString col;
    QString mode_dark;
    QString mode_discolor;
    QString mode_discolor_reverse;
    QString mode_milky;
    QString mode_milky_reverse;
    QString mode_yellow_milky;
    QString mode_yellow_trans;
    QString mode_yellow_yellow_reverse;
    QString mode_yellow_white_reverse;
    QString mode_user_defined;
    QString detect_wait;

    // sort params
    QString bad_discolor;
    QString bad_area;
    QString auto_set;
    QString super_brain;
    QString eject_set;
    QString set_by_times;
    QString set_alone;
    QString arith_scale;
    QString arith_shape;

    // camera signal
    QString camera_signal;
    QString channel_biasing;
    QString odd_biasing;
    QString even_biasing;
    QString channel_range;
    QString channel_cross;
    QString gain_target;
    QString eject_times;
    QString image_capture;

    // vender set
    QString lamp_protect;
    QString feeder_biasing;
    QString system_test;
    QString assist_camera_enable;
    QString advanced_parameters;
    QString exit_list;
    QString exposure_time;
    QString lens_select;
    QString time_short;
    QString time_middle;
    QString time_long;
    QString delay_biasing;
    QString degauss_time;
    QString truncation_time;
    QString clear_total;
    QString preheat_time;
    QString machine_type_select;
    QString times_allocation;
    QString pass_allocation;
    QString feeder_control;
    QString feeder_control_enable;
    QString feeder_remote_control;
    QString forward_direction;
    QString opposite_direction;
    QString input_ai_params;
    QString identify_analysis;

    // system test
    QString test_pass_num;
    QString weight_test_before;
    QString weight_test_after;
    QString single_pass_output;
    QString auto_running;
    QString output_test_set;
    QString output_test_start;

    // image capture
    QString grey;
    QString matter_area;
    QString image_derive;
    QString rice_sum;
    QString broken_rice_num;
    QString broken_rice_ratio;

    //video
    QString video_starting_info;
    QString video_stoping_info;
    QString real_output;
    QString real_dirtper;

    // auto set
    QString current_feeder;
    QString fixed_feeder;
    QString reference_chute;
    QString special_for_deep;

    // bad area
    QString pale_yellow_area;
    QString bad_width;

    // inf set
    QString infrared_set;
    QString special_for_glass;
    QString follow_lower_limit;

    // lamp set
    QString front_sort;
    QString rear_sort;

    // eject times
    QString sampling_time;
    QString eject_times_limit;

    // white board adjust
    QString white_board_adjust;
    QString adjust_start;
    QString adjust_finish;
    QString auto_copy;
    QString co_capture;
    QString cuteoff_bg;
    QString adjust_copy;
    QString adjust_wave;
    QString channel;

    // message
    QString msg_input_weight_before;
    QString msg_system_testing;
    QString msg_input_weight_after;
    QString msg_insert_white_board;
    QString msg_white_board_adjusting;
    QString msg_remove_white_board;

    // copy major params
    QString scheme_cur;
    QString scheme_des;
    QString scheme_all;

    // feederAI
    QString feeder_AI;
    QString eject_stat_enable;
    QString current_eject_freq;
    QString feeder_range;
    QString ejector_range;

    /* PMT */
    QString pmt_run;
    QString pmt_front_frame;
    QString pmt_rear_frame;
    QString pmt_enable_red1;
    QString pmt_enable_red2;
    QString pmt_enable_green;
    QString pmt_enable_blue;
    QString pmt_ctrl_voltage;
    QString pmt_ctrl;

    //log
    QString log;
    QString from;
    QString to;
    QString fresh;
    QString all;
    QString power_switch;
    QString feeder_switch;
    QString machine;
    QString profile;
    QString camera;
    QString bg;
    QString arith;
    QString reject;
    QString wipe;
    QString debug;

    //LD4
    QString upper_master;
    QString upper_slave;
    QString lower_master;
    QString lower_slave;
    QString upper_like_lower;
    QString master_page;
    QString slave_page;
    QString upper_L_begin;
    QString upper_L_end;
    QString upper_R_begin;
    QString upper_R_end;
    QString lower_L_begin;
    QString lower_L_end;
    QString lower_R_begin;
    QString lower_R_end;
    QString unused;

    //color select
    QString color_select_mode;
    QString set_color_select_mode;
    QString black_tea;
    QString white_bar_tea;
    QString yeloow_green_bar_piece;
    QString red_bar_red_piece;

    //bigData
    QString bigData;
    QString state;
    QString state_info;
    QString backgroud;
    QString material;
    QString backgroud_statistic;
    QString identify;
    QString identify_statistic;
    QString eject;
    QString eject_statistic;
    QString histogram;
    QString histogram_statistic;
    QString configration;
    QString cameraReference;
    QString cameraLine;
    QString cameraList;
    QString time_statistic;
    QString material_total;
    QString material_bad_total;

    //accurate tick
    QString common;
    QString accurate;
    QString eject_width_max;
    QString eject_width_min;
    QString eject_width_dif;
    QString rounds;
    QString tick_mode;
    QString tick_sort_mode_warm;
    //contra test
    QString contra_test;
    QString bias;
    QString correct;
    QString send_bias;
    QString msg_accept_data;
    QString msg_receiving;

    // new pic;
    QString pic_customer;
    QString schemeNew_select;
    QString videoNew;
    QString save_parameterNew;
    QString working_stop;
    QString working_start;


    QString set_sens;
    QString feeder_speed;
    QString test_ejector;
    QString set_wiper;
    QString set_function;
    QString operation_level;

    QString new_scheme;
    QString copy_scheme;
    QString delte_scheme;
    QString rename_scheme;
    QString recover_scheme;
    QString backup_scheme;

    QString customer_model;
    QString sys_set;

    QString network;
    QString help;
    QString device;
    QString capture;

    QString algorithm;
    QString aux_parameter;
    QString set_time;
    QString io_set;

    QString front_material;
    QString front_background;
    QString back_material;
    QString back_background;

    QString camera_new;
    QString set_machine;
    QString test_output;
    QString old_test;
    QString set_vib_voltage;
    QString tdemag;
    QString setLight;
//    QString tick_mode;
    QString encrypt;
    QString out;
    QString bad_ratio;
    QString background_proportion;

    QString salesperson;
    QString telephone;
    QString expiration_time;

    //解密码非法
    QString illegal_password_decryption;
    QString decryption_successful;
    QString decryption_process_error;

    //加密码非法
    QString illegal_password_encryption;
    QString encryption_successful;
    QString encryption_process_error;

    QString target_value;

    // ai翻译
      QString download_image;      // 图像下载
      QString image_reasoning;     // 图像推理
      QString image_acquisition;   // 图像采集

      QString pixel_info;          // 像元信息
      QString ejector_para;        // 喷阀参数
      QString image_height;        // 图像高度
      QString model_para;          // 模型参数
      QString model_update;        // 模型升级
      QString ai_version;          // 版本查询
      QString disk_space;          // 磁盘空间

      QString img_downloading;         //图像下载中
      QString ftp_status_error;        //ftp状态异常
      QString img_download_finished;    //图像下载完成
      QString delete_original_image;   //是否删除原图
      QString deleting;                //删除中
      QString img_list_blank;          //图像文件为空
      QString delete_original_image_finished;//原图删除成功
      QString setting_error;           //设置失败
      QString setting_succeeded;       //设置成功
      QString setting_value_error;     //返回值错误

      QString model_list_blank;          //模型列表为空
      QString model_para_error;          //模型参数异常
      QString beginning;          //进行中
      QString error;               //失败
      QString succeed;             //成功
      QString fixed_delay;             //固定延迟
      QString dynamic_delay;           //动态延迟
      QString acquisition_height;      //采集高度
      QString reasoning_height;        //推理高度

      QString rename;                  //重命名
  //    QString del;                     //删除
      QString add;                     //新增
      QString model;                    //模型
      QString id_blank;                 //id为空
      QString json_file;                //json文件
      QString dlc_file;                 //dlc文件
      QString content;                  //内容
      QString open;                     //打开
      QString notexist;                 //不存在
      QString upload;                 //上传
      QString model_exist_msg;         //模型已存在，无法新增，仅更新模型
                                      //The model already exists. New addition is not possible. Only model update is allowed.

      QString   spray_valve_number;   // 喷阀编号
      QString   nozzle_number;        // 喷嘴编号
      QString   spray_time_interval;  // 喷阀间隔


};

extern MyLanguage myLan;

#endif // MYLANGUAGE_H
