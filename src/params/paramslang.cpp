#include "globalflow.h"

void GlobalFlow::initLan()
{
    // general
    myLan.ok = tr("ok");
    myLan.cancel = tr("cancel");
    myLan.enable = tr("enable");
    myLan.disable = tr("disable");
    myLan.back = tr("back");
    myLan.space = tr("space");
    myLan.input = tr("input");
    myLan.del = tr("del");
    myLan.on = tr("on");
    myLan.off = tr("off");
    myLan.reboot = tr("reboot");
    myLan.apply = tr("apply");
    myLan.name = tr("name");
    myLan.list = tr("list");
    myLan.start = tr("start");
    myLan.stop = tr("stop");
    myLan.skip_preheat = tr("skip_preheat");
    myLan.page_communicate = tr("page_communicate");
    myLan.communication_list = tr("communication_list");
    myLan.page_home = tr("page_home");
    myLan.save_parameters = tr("save_parameters");
    myLan.set_purview = tr("set_purview");
    myLan.enter_system = tr("enter_system");
    myLan.halt = tr("halt");
    myLan.engineering_mode = tr("engineering_mode");
    myLan.system_initial = tr("system_initial");
    myLan.preheat = tr("preheat");
    myLan.sensitivity = tr("sensitivity");
    myLan.proportion = tr("proportion");
    myLan.background_lamp = tr("background_lamp");
    myLan.sensitivity_minimum = tr("sensitivity_minimum");
    myLan.sensitivity_maximum = tr("sensitivity_maximum");
    myLan.sensitivity_set = tr("sensitivity_set");
    myLan.vendor_mode = tr("vendor_mode");
    myLan.operator_mode = tr("operator_mode");
    myLan.engineer = tr("engineer");
    myLan.last_page = tr("last_page");
    myLan.next_page = tr("next_page");
    myLan.camera_signal = tr("camera_signal");
    myLan.select_sensitivity = tr("select_sensitivity");
    myLan.general = tr("general");
    myLan.other = tr("other");
    myLan.machine_info = tr("machine_info");
    myLan.engineer_info = tr("engineer_info");
    myLan.vendor_info = tr ("vendor_info");
    myLan.device_info = tr("device_info");
    myLan.device_id = tr("device_id");
    myLan.times = tr("times");
    myLan.set = tr("set");
    myLan.capacity = tr("capacity");
    myLan.clear = tr("clear");
    myLan.compute = tr("compute");
    myLan.weight = tr("weight");

    //delay code
    myLan.dc_input  = tr("dc_input");
    myLan.dc_productsn = tr("dc_productsn");
    myLan.dc_productsn_mismatch = tr("dc_productsn_mismatch");
    myLan.dc_illegal = tr("dc_illegal");
    myLan.dc_illegal_used = tr("dc_illegal_used");
    myLan.dc_save_erro = tr("dc_save_erro");
    myLan.dc_limit = tr("dc_limit");
    myLan.dc_limit_info = tr("dc_limit_info");
    myLan.dc_rtc_erro   = tr("dc_rtc_erro");
    myLan.dc_cnt        = tr("dc_cnt");

    // machine view & level
    myLan.chute = tr("chute");
    myLan.front = tr("front");
    myLan.front_view = tr("front_view");
    myLan.master_view = tr("master_view");
    myLan.slave_view = tr("slave_view");
    myLan.rear = tr("rear");
    myLan.rear_view = tr("rear_view");
    myLan.front_rear_view = tr("front_rear_view");
    myLan.upper = tr("upper");
    myLan.upper_view = tr("upper_view");
    myLan.mid = tr("mid");
    myLan.lower = tr("lower");
    myLan.lower_view = tr("lower_view");
    myLan.first_front = tr("first_front");
    myLan.first_rear = tr("first_rear");
    myLan.second_front = tr("second_front");
    myLan.second_rear = tr("second_rear");
    myLan.third_front = tr("third_front");
    myLan.third_rear = tr("third_rear");
    myLan.fourth_front = tr("fourth_front");
    myLan.fourth_rear = tr("fourth_rear");
    myLan.first_upper = tr("first_upper");
    myLan.first_lower = tr("first_lower");
    myLan.second_upper = tr("second_upper");
    myLan.second_lower = tr("second_lower");
    myLan.first_down_front = tr("first_down_front");
    myLan.first_up_front = tr("first_up_front");
    myLan.first_down_rear = tr("first_down_rear");
    myLan.first_up_rear = tr("first_up_rear");
    myLan.second_down_front = tr("second_down_front");
    myLan.second_up_front = tr("second_up_front");
    myLan.second_down_rear = tr("second_down_rear");
    myLan.second_up_rear = tr("second_up_rear");
    myLan.upper_layer = tr("upper_layer");
    myLan.lower_layer = tr("lower_layer");
    myLan.mid_layer = tr("middle_layer");
    myLan.layer = tr("layer");
    myLan.unit_info = tr("unit_info");
    myLan.first = tr("first");
    myLan.second = tr("second");
    myLan.third = tr("third");
    myLan.fourth = tr("fourth");
    myLan.enable_third_level = tr("enable_third_level");
    myLan.msg_channel_error = tr("msg_channel_error");
    myLan.msg_need_first_unit = tr("msg_need_first_unit");
    myLan.msg_need_second_unit = tr("msg_need_second_unit");
    myLan.eject_num = tr("eject_num");
    myLan.front_sens = tr("front_sens");
    myLan.rear_sens = tr("rear_sens");
    myLan.current_camera = tr("current_camera");

    // ai
    myLan.ai_simulate = tr("ai_simulate");
    myLan.ai_basic = tr("ai_basic");
    myLan.ai_more = tr("ai_more");
    myLan.ai_no_image = tr("ai_no_image");
    myLan.ai_reselect_whole = tr("ai_reselect_whole");
    myLan.ai_reselect_last = tr("ai_reselect_last");
    myLan.ai_optimize_calculate = tr("ai_optimize_calculate");
    myLan.ai_optimize_sample = tr("ai_optimize_sample");
    myLan.ai_set_material = tr("ai_set_material");
    myLan.ai_material = tr("ai_material");
    myLan.msg_set_material = tr("ai_msg_set_material");
    myLan.ai_calculate = tr("ai_calculate");
    myLan.ai_update_sample = tr("ai_update_sample");
    myLan.ai_sample_color = tr("ai_sample_color");
    myLan.ai_sample_sens = tr("ai_sample_sens");
    myLan.ai_trans_mode = tr("ai_trans_mode");
    myLan.ai_low_speed = tr("ai_low_speed");
    myLan.ai_high_speed = tr("ai_high_speed");
    myLan.ai_sample_mode = tr("ai_sample_mode");
    myLan.ai_trigger_threshold = tr("ai_trigger_threshold");
    myLan.ai_enable_mode = tr("ai_enable_mode");
    myLan.ai_normal = tr("ai_normal");
    myLan.ai_infrared = tr("ai_infrared");
    myLan.ai_multi_mode = tr("ai_multi_mode");
    myLan.ai_material_name = tr("ai_material_name");
    myLan.ai_good = tr("ai_good");
    myLan.ai_bad = tr("ai_bad");
    myLan.ai_cancel_optimize_calculate = tr("ai_cancel_optimize_calculate");
    myLan.ai_cancel_optimize_sample = tr("ai_cancel_optimize_sample");
    myLan.ai_select_1000_pixels = tr("ai_select_1000_pixels");
    myLan.msg_computing = tr("ai_computing");
    myLan.ai_save_error = tr("ai_save_error");
    myLan.msg_need_resample = tr("ai_msg_need_resample");
    myLan.msg_usb_insert = tr("ai_msg_usb_insert");
    myLan.msg_copy_image = tr("ai_msg_copy_image");
    myLan.msg_export_image = tr("msg_export_image");
    myLan.msg_import_image = tr("msg_import_image");
    myLan.success = tr("success");
    myLan.failed = tr("failed");
    myLan.sd_no_imageBak_dir = tr("sd_no_imageBak_dir");
    myLan.dir_no_png_file = tr("dir_no_png_file");
    myLan.usb_no_imageBak_dir = tr("usb_no_imageBak_dir");
    myLan.select_png_dir = tr("select_png_dir");
    myLan.msg_clear_local_pics = tr("ai_msg_clear_local_pics");
    myLan.ai_set_good_bad = tr("ai_set_good_bad");
    myLan.ai_set_more_material = tr("ai_set_more_material");
    myLan.ai_need_bad_material = tr("ai_need_bad_material");
    myLan.ai_need_good_material = tr("ai_need_good_material");
    myLan.ai_name_material = tr("ai_name_material");
    myLan.ai_exceed_max_times = tr("ai_exceed_max_times");
    myLan.ai_image_capturing = tr("ai_image_capturing");
    myLan.ai_image_capture_error = tr("ai_image_capture_error");
    myLan.ai_unsupervised_less_point = tr("ai_unsupervised_less_point");
    myLan.ai_unsupervised_more_point = tr("ai_unsupervised_more_point");
    myLan.cfm_reselect_sample = tr("ai_cfm_reselect_sample");
    myLan.msg_optimizing = tr("ai_optimizing");
    myLan.ai_analysis = tr("ai_analysis");
    myLan.ai_analysis_set = tr("ai_analysis_set");
    myLan.ai_analysis_material = tr("ai_analysis_material");
    myLan.msg_compute_error = tr("ai_msg_compute_error");
    myLan.msg_auto_computer_erro = tr("ai_auto_computer_erro");
    myLan.ai_camera_mode = tr("ai_camera_mode");
    myLan.ai_color_space = tr("ai_color_space");
    myLan.ai_material_image = tr("ai_material_image");
    myLan.ai_auto_cal = tr("ai_auto_cal");
    myLan.ai_del_img = tr("ai_del_img");
    myLan.ai_preview_mode = tr("ai_preview_mode");
    myLan.ai_cal_set_same = tr("ai_cal_set_same");
    myLan.ai_cal_set_self = tr("ai_cal_set_self");
    myLan.ai_class_mode_null = tr("ai_class_mode_null");
    myLan.ai_class_mode_usual= tr("ai_class_mode_usual");
    myLan.ai_class_mode_multiple = tr("ai_class_mode_multiple");
    myLan.ai_capture = tr("ai_capture");
    myLan.ai_delete = tr("ai_delete");
    myLan.ai_err_no_bad = tr("ai_err_no_bad");
    myLan.ai_err_no_good = tr("ai_err_no_good");
    myLan.ai_err_no_set = tr("ai_err_no_set");
    myLan.ai_err_overflow = tr("ai_err_overflow");
    myLan.ai_cal_warn = tr("ai_cal_warn");
    myLan.ai_cal_open = tr("ai_cal_open");
    myLan.ai_img_import = tr("ai_img_import");
    myLan.ai_img_export = tr("ai_img_export");
    myLan.ai_img_import_export = tr("ai_img_import_export");
    myLan.ai_img_threshold = tr("ai_img_threshold");
    myLan.ai_img_colorSpace = tr("ai_img_colorSpace");
    myLan.ai_img_colorSpace1 = tr("ai_img_colorSpace1");
    myLan.ai_img_colorSpace2 = tr("ai_img_colorSpace2");
    myLan.ai_img_msg_export = tr("ai_img_msg_export");
    myLan.ai_img_msg_import = tr("ai_img_msg_import");
    myLan.ai_mode           = tr("ai_mode");
    myLan.ai_mode1          = tr("ai_mode1");
    myLan.ai_mode2          = tr("ai_mode2");
    myLan.ai_start_purity = tr("ai_start_purity");
    myLan.ai_unsupervisedLearning = tr("ai_unsupervisedLearning");
    myLan.ai_reserved_sens = tr("ai_reserved_sens");
    myLan.ai_params_set  =tr("ai_params_set");
    // material
    myLan.identify_params = tr("identify_params");
    myLan.infra_identify_params = tr("infra_identify_params");
    myLan.material_general = tr("material_general");
    myLan.infra_material_general = tr("infra_material_general");
    myLan.material_color = tr("material_color");
    myLan.material_defect_grey_a = tr("material_defect_grey_a");
    myLan.material_defect_grey_b = tr("material_defect_grey_b");
    myLan.material_defect_discolor_a = tr("material_defect_discolor_a");
    myLan.material_defect_discolor_b = tr("material_defect_discolor_b");
    myLan.material_cross = tr("material_cross");
    myLan.material_shape = tr("material_shape");
    myLan.material_shape_length = tr("material_shape_length");
    myLan.material_shape_pole   = tr("material_shape_pole");
    myLan.material_shape_pole_a = tr("material_shape_pole_a");
    myLan.material_shape_pole_b = tr("material_shape_pole_b");
    myLan.material_scale_params = tr("material_scale_params");
    myLan.material_shape_slice  = tr("material_shape_slice");
    myLan.material_shape_leaf   = tr("material_shape_leaf");
    myLan.material_bud_1 = tr("material_bud_1");
    myLan.material_bud_2 = tr("material_bud_2");
    myLan.material_defect_a = tr("material_defect_a");
    myLan.material_defect_b = tr("material_defect_b");
    myLan.material_defect_c = tr("material_defect_c");
    myLan.material_defect_d = tr("material_defect_d");
    myLan.material_infra = tr("material_infra");
    myLan.material_intelligent_mode_a = tr("material_intelligent_mode_a");
    myLan.material_intelligent_mode_b = tr("material_intelligent_mode_b");
    myLan.material_intelligent_mode_c = tr("material_intelligent_mode_c");
    myLan.material_intelligent_mode_d = tr("material_intelligent_mode_d");
    myLan.material_tea = tr("material_tea");
    myLan.material_watermelon = tr("material_watermelon");
    myLan.material_reserved = tr("material_reserved");
    myLan.material_maize_param = tr("material_maize_param");
    myLan.material_watermelon_edge_red = tr("material_watermelon_edge_red");
    myLan.material_watermelon_red_sens = tr("material_watermelon_red_sens");
    myLan.material_watermelon_red_ratio = tr("material_watermelon_red_ratio");
    myLan.material_watermelon_edge_damaged = tr("material_watermelon_edge_damaged");
    myLan.material_watermelon_damaged_ratio = tr("material_watermelon_damaged_ratio");
    myLan.material_watermelon_damaged_sens = tr("material_watermelon_damaged_sens");
    myLan.material_watermelon_rugged1 = tr("material_watermelon_rugged1");
    myLan.material_watermelon_rugged2 = tr("material_watermelon_rugged2");
    myLan.material_watermelon_white_small = tr("material_watermelon_white_small");
    myLan.material_watermelon_white_sens = tr("material_watermelon_white_sens");
    myLan.material_watermelon_rugged_save_white = tr("material_watermelon_rugged_save_white");
    myLan.material_watermelon_rugged_save_black = tr("material_watermelon_rugged_save_black");
    myLan.material_watermelon_ai_whole = tr("material_watermelon_ai_whole");
    myLan.material_watermelon_ai_edge = tr("material_watermelon_ai_edge");
    myLan.cfm_use_watermelon = tr("cfm_use_watermelon");
    myLan.cfm_rename_impurity = tr("cfm_rename_impurity");
    myLan.defect_name = tr("defect_name");
    myLan.mode = tr("mode");
    myLan.scale = tr("scale");
    myLan.purity = tr("purity");
    myLan.red = tr("red");
    myLan.green = tr("green");
    myLan.blue = tr("blue");
    myLan.dark = tr("dark");
    myLan.light = tr("light");
    myLan.light_limit = tr("light_limit");
    myLan.red_green = tr("red_green");
    myLan.red_blue = tr("red_blue");
    myLan.green_blue = tr("green_blue");
    myLan.sort_circular = tr("sort_circular");
    myLan.sort_long = tr("sort_long");
    myLan.sort_short = tr("sort_short");
    myLan.sort_small = tr("sort_small");
    myLan.sort_big = tr("sort_big");
    myLan.params_limit = tr("params_limit");
    myLan.area = tr("area");
    myLan.arithmetic_list = tr("arithmetic_list");
    myLan.material_mode = tr("material_mode");
    myLan.first_second = tr("first_second");
    myLan.all_seperate = tr("all_seperate");
    myLan.front_rear = tr("front_rear");
    myLan.all_alike = tr("all_alike");
    myLan.reserved = tr("reserved");
    myLan.reserved_1 = tr("reserved_1");
    myLan.reserved_2 = tr("reserved_2");
    myLan.reserved_dark_red = tr("reserved_dark_red");
    myLan.reserved_peeled = tr("reserved_peeled");
    myLan.reserved_burnt = tr("reserved_burnt");
    myLan.slice_sensitivity = tr("slice_sensitivity");
    myLan.slice_size = tr("slice_size");
    myLan.pole_size = tr("pole_size");
    myLan.grain_sensitivity = tr("grain_sensitivity");
    myLan.grain_size = tr("grain_size");
    myLan.balance = tr("balance");
    myLan.hard_code = tr("hard_code");
    myLan.min_limit = tr("min_limit");
    myLan.max_limit = tr("max_limit");
    myLan.threshold = tr("threshold");

    // lamp
    myLan.lamp_front_bg_red = tr("lamp_front_bg_red");
    myLan.lamp_front_bg_green = tr("lamp_front_bg_green");
    myLan.lamp_front_bg_blue = tr("lamp_front_bg_blue");
    myLan.lamp_rear_bg_red = tr("lamp_rear_bg_red");
    myLan.lamp_rear_bg_green = tr("lamp_rear_bg_green");
    myLan.lamp_rear_bg_blue = tr("lamp_rear_bg_blue");
    myLan.lamp_rear = tr("lamp_rear");
    myLan.lamp_rear_1 = tr("lamp_rear_1");
    myLan.lamp_rear_2 = tr("lamp_rear_2");
    myLan.lamp_rear_3 = tr("lamp_rear_3");
    myLan.lamp_rear_4 = tr("lamp_rear_4");
    myLan.lamp_upper_1 = tr("lamp_upper_1");
    myLan.lamp_upper_2 = tr("lamp_upper_2");
    myLan.lamp_upper_3 = tr("lamp_upper_3");
    myLan.lamp_upper_4 = tr("lamp_upper_4");
    myLan.lamp_front = tr("lamp_front");
    myLan.lamp_front_1 = tr("lamp_front_1");
    myLan.lamp_front_2 = tr("lamp_front_2");
    myLan.lamp_front_3 = tr("lamp_front_3");
    myLan.lamp_upper_bg = tr("lamp_upper_bg");
    myLan.lamp_low_1 = tr("lamp_low_1");
    myLan.lamp_low_2 = tr("lamp_low_2");
    myLan.lamp_low_3 = tr("lamp_low_3");
    myLan.lamp_low_4 = tr("lamp_low_4");
    myLan.lamp_low_bg = tr("lamp_low_bg");
    myLan.lamp_upper_bg_red = tr("lamp_upper_bg_red");
    myLan.lamp_upper_bg_green = tr("lamp_upper_bg_green");
    myLan.lamp_upper_bg_blue = tr("lamp_upper_bg_blue");
    myLan.lamp_low_bg_red = tr("lamp_low_bg_red");
    myLan.lamp_low_bg_green = tr("lamp_low_bg_green");
    myLan.lamp_low_bg_blue = tr("lamp_low_bg_blue");
    myLan.lamp_mid_1 = tr("lamp_mid_1");
    myLan.lamp_mid_2 = tr("lamp_mid_2");
    myLan.lamp_mid_3 = tr("lamp_mid_3");
    myLan.lamp_mid_4 = tr("lamp_mid_4");
    myLan.lamp_mid_bg = tr("lamp_mid_bg");
    myLan.lamp_rear_bg = tr("lamp_rear_bg");
    myLan.lamp_front_bg = tr("lamp_front_bg");

    // feeder
    myLan.feeder_set = tr("feeder_set");
    myLan.feeder = tr("feeder");
    myLan.feed = tr("feed");
    myLan.feeder_ac = tr("feeder_ac");
    myLan.feedEnable = tr("feed_enable");
    myLan.feeder_value = tr("feeder_value");
    myLan.enable_all = tr("enable_all");
    myLan.disable_all = tr("disable_all");
    myLan.single = tr("single");
    myLan.complex = tr("complex");
    myLan.feeder_level = tr("feeder_level");
    myLan.feeder_empty = tr("feeder_empty");
    myLan.feeder_conbine = tr("feeder_conbine");
    myLan.no_material = tr("no_material");
    myLan.exist_material = tr("exist_material");

    // belt
    myLan.belt = tr("belt");

    // wipe
    myLan.wipe_set = tr("wipe_set");
    myLan.wipe_duration = tr("wipe_duration");
    myLan.wipe_interval = tr("wipe_interval");
    myLan.wipe_enable = tr("wipe_enable");
    myLan.wipe_manual = tr("wipe_manual");
    myLan.wipe_wind = tr("wipe_wind");
    myLan.wipe_wind_duration = tr("wipe_wind_duration");
    myLan.wipe_wind_interval = tr("wipe_wind_interval");
    myLan.wipe_wind_manual = tr("wipe_wind_manual");
    myLan.wipe_water = tr("wipe_water");
    myLan.wipe_water_duration = tr("wipe_water_duration");
    myLan.wipe_water_delay = tr("wipe_water_delay");
    myLan.wipe_delay = tr("wipe_delay");

    // scheme
    myLan.select = tr("select");
    myLan.scheme_select = tr("scheme_select");
    myLan.scheme_parameter = tr("scheme_parameter");
    myLan.scheme_manage = tr("scheme_manage");
    myLan.scheme_new = tr("scheme_new");
    myLan.scheme_copy = tr("scheme_copy");
    myLan.scheme_delete = tr("scheme_delete");
    myLan.scheme_rename = tr("scheme_rename");
    myLan.scheme_restore = tr("scheme_restore");
    myLan.scheme_backup = tr("scheme_backup");
    myLan.scheme_change_mode = tr("scheme_change_mode");
    myLan.scheme_copy_major = tr("scheme_copy_major");
    myLan.cfm_scheme_create = tr("cfm_create_scheme");
    myLan.cfm_scheme_copy = tr("cfm_scheme_copy");
    myLan.cfm_scheme_delete = tr("cfm_scheme_delete");
    myLan.msg_delete_forbidden = tr("msg_delete_forbidden");
    myLan.cfm_scheme_rename = tr("cfm_scheme_rename");
    myLan.msg_name_input = tr("msg_scheme_name_input");
    myLan.cfm_restore_from_local = tr("cfm_restore_from_local");
    myLan.cfm_restore_from_usb = tr("cfm_restore_from_usb");
    myLan.msg_restore_from_usb_error = tr("msg_restore_from_usb_error");
    myLan.cfm_backup_to_local = tr("cfm_backup_to_local");
    myLan.cfm_backup_to_usb = tr("cfm_backup_to_usb");
    myLan.cfm_select_current_scheme = tr("cfm_select_current_scheme");
    myLan.cfm_change_profile_mode = tr("cfm_change_profile_mode");
    myLan.msg_custom_language_input = tr("msg_custom_language_input");
    myLan.msg_rename_custom_language = tr("msg_rename_custom_language");

    // background
    myLan.background_set = tr("background_set");
    myLan.value = tr("value");
    myLan.percent = tr("percent");
    myLan.black = tr("black");
    myLan.white = tr("white");
    myLan.automatic = tr("automatic");
    myLan.msg_auto_bgk_ckeck_error = tr("msg_auto_bgk_ckeck_error");
    myLan.msg_strip_frame_data_error = tr("msg_strip_frame_data_error");
    // eject tick
    myLan.eject_on = tr("eject_on");
    myLan.eject_duration = tr("eject_duration");
    myLan.eject_delay = tr("eject_delay");
    myLan.eject_bias = tr("eject_bias");
    myLan.eject_test = tr("eject_test");
    myLan.ejector_current = tr("ejector_current");
    myLan.ejector_keep_loop = tr("ejector_keep_loop");
    myLan.eject_time = tr("eject_time");
    myLan.eject_fast_test = tr("eject_fast_test");
    myLan.lamp_control = tr("lamp_control");
    myLan.tick_params = tr("tick_params");
    myLan.infra_tick_params = tr("infra_tick_params");
    myLan.infra_background_set = tr("infra_background_set");
    myLan.fixed_mode = tr("fixed_mode");
    myLan.extended_mode = tr("extended_mode");
    myLan.accurate_mode = tr("accurate_mode");
    myLan.eject_mode = tr("eject_mode");
    myLan.infect_arithmetic = tr("infect_arithmetic");
    myLan.edge_cut = tr("edge_cut");
    myLan.remove_joint = tr("remove_joint");
    myLan.remove_tick = tr("remove_tick");
    myLan.edge_revise = tr("edge_revise");
    myLan.mat_width = tr("mat_width");
    myLan.bad_number = tr("bad_number");
    myLan.good_number = tr("good_number");
    myLan.enable_balance = tr("enable_balance");
    myLan.logic_or = tr("logic_or");
    myLan.logic_and = tr("logic_and");
    myLan.logical_relation = tr("logical_relation");
    myLan.sort = tr("sort");
    myLan.reverse = tr("reverse");
    myLan.sort_mode = tr("sort_mode");
    myLan.unknown = tr("unknown");
    myLan.enable_default = tr("enable_default");
    myLan.color_sort = tr("color_sort");
    myLan.shape_sort = tr("shape_sort");
    myLan.huff_mode = tr("huff_mode");
    myLan.huff_width = tr("huff_width");

    // system status
    myLan.system_stat = tr("system_stat");
    myLan.screen = tr("screen");
    myLan.interface_board = tr("interface_board");
    myLan.control_board = tr("control_board");
    myLan.color_board = tr("color_board");
    myLan.color = tr("color");
    myLan.mono_board = tr("mono_board");
    myLan.infra_board = tr("infra_board");
    myLan.light_src_board = tr("light_src_board");
    myLan.recheck = tr("recheck");
    myLan.infra = tr("infra");
    myLan.software_version = tr("software_version");
    myLan.normal = tr("normal");
    myLan.abnormal = tr("abnormal");
    myLan.eject_frequency = tr("eject_frequency");
    myLan.system_time = tr("system_time");
    myLan.year = tr("year");
    myLan.month = tr("month");
    myLan.day = tr("day");
    myLan.hour = tr("hour");
    myLan.minute = tr("minute");
    myLan.sec = tr("sec");
    myLan.information = tr("information");
    myLan.this_run = tr("this_run");
    myLan.this_feed_run = tr("this_feed_run");
    myLan.total_run = tr("total_run");
    myLan.total_feed_run = tr("total_feed_run");
    myLan.this_pressure_alarm_times = tr("this_pressure_alarm_times");
    myLan.total_pressure_alarm = tr("total_pressure_alarm");
    myLan.feed_stat = tr("feed_stat");
    myLan.eject_stat = tr("eject_stat");
    myLan.belt_1_status = tr("belt_1_status");
    myLan.belt_2_status = tr("belt_2_status");
    myLan.alarm_set = tr("alarm_set");
    myLan.msg_pressure_alarm_check_pressure = tr("msg_pressure_alarm_check_pressure");
    myLan.msg_pressure_alarm = tr("msg_pressure_alarm");
    myLan.pressure_not_enough = tr("pressure_not_enough");
    myLan.warm = tr("warm");
    myLan.blower_alarm = tr("blower_alarm");
    myLan.lamp_alarm = tr("lamp_alarm");
    myLan.pressure_alarm = tr("pressure_alarm");
    myLan.enable_press_alarm = tr("enable_press_alarm");
    myLan.enable_blower_alarm = tr("enable_blower_alarm");
    myLan.enable_lamp_alarm = tr("enable_lamp_alarm");
    myLan.enable_belt_status = tr("enable_belt_status");
    myLan.enable_temperature_alarm = tr("enable_temperature_alarm");
    myLan.enable_communication_alarm = tr("enable_communication_alarm");
    myLan.auto_calibrate = tr("auto_calibrate");
    myLan.auto_calibrate_tips = tr("auto_calibrate_tips");
    myLan.auto_calibrate_step1 = tr("auto_calibrate_step1");
    myLan.auto_calibrate_step2 = tr("auto_calibrate_step2");
    myLan.auto_calibrate_step3 = tr("auto_calibrate_step3");
    myLan.auto_calibrate_step4 = tr("auto_calibrate_step4");
    myLan.auto_calibrate_step5 = tr("auto_calibrate_step5");
    myLan.auto_calibrate_step6 = tr("auto_calibrate_step6");
    myLan.background_angle     = tr("background_angle");
    myLan.howto = tr("HowTo");
    myLan.contact_us = tr ("contact_us");
    myLan.reload = tr("Reload");
    myLan.one_key_feed = tr("one_key_feed");
    myLan.communication_alarm = tr("communication_alarm");
    myLan.level_stat = tr("level_stat");
    myLan.level_empty_alarm = tr("level_empty_alarm");
    myLan.level_full_alarm = tr("level_full_alarm");
    myLan.level_sensor_alarm = tr("level_sensor_alarm");
    myLan.video = tr("video");
    myLan.sorter_manager = tr("sorter_manager");
    myLan.auto_analysis = tr("auto_analysis");
    myLan.temperature = tr("temperature");
    myLan.slave_list = tr("slave_list");

    // net
    myLan.remote_control = tr("remote_control");
    myLan.network_config = tr("network_config");
    myLan.local_host = tr("local_host");
    myLan.ip_address = tr("ip_address");
    myLan.subnet_mask = tr("subnet_mask");
    myLan.default_gateway = tr("default_gateway");
    myLan.dns_server = tr("dns_server");
    myLan.mac_address = tr("mac_address");
    myLan.route_address = tr("route_address");
    myLan.set_server = tr("set_server");
    myLan.set_sms = tr("set_sms");
    myLan.receiver_num = tr("receiver_num");
    myLan.sms_center_num = tr("sms_center_num");
    myLan.sms_content = tr("sms_content");
    myLan.sms_send = tr("sms_send");
    myLan.test_signal = tr("test_signal");
    myLan.obtain_auto = tr("obtain_auto");
    myLan.qrcode = tr("qrcode");
    myLan.qrcode_generator = tr("qrcode_generator");
    myLan.devId =  tr("Device info");

    // factory set
    myLan.factory_set = tr("factory_set");
    myLan.language = tr("language");
    myLan.product = tr("product");
    myLan.clear_running_time = tr("clear_running_time");
    myLan.aging_test = tr("aging_test");
    myLan.upgrade_screen = tr("upgrade_screen");
    myLan.upgrade_fpga = tr("upgrade_fpga");
    myLan.init_mode = tr("init_mode");
    myLan.ts_calibrate = tr("ts_calibrate");
    myLan.throughput_test = tr("throughput_test");
    myLan.run_mode = tr("run_mode");
    myLan.run_test = tr("run_test");
    myLan.run_fre = tr("run_fre");
    myLan.test_time = tr("test_time");
    myLan.mode_1 = tr("mode_1");
    myLan.mode_2 = tr("mode_2");
    myLan.mode_3 = tr("mode_3");
    myLan.english = tr("english");
    myLan.chinese_simplified = tr("chinese_simplified");
    myLan.chinese_traditonal = tr("chinese_traditonal");
    myLan.russian = tr("russian");
    myLan.vietnam = tr("vietnam");
    myLan.thailand = tr("thailand");
    myLan.spanish = tr("spanish");
    myLan.turkey = tr("turkey");
    myLan.farsi = tr("farsi");
    myLan.french = tr("french");
    myLan.uyghur = tr("uyghur");
    myLan.korean = tr("korean");
    myLan.arabic = tr("arabic");
    myLan.bulgarian = tr("bulgarian");
    myLan.slovak = tr("slovak");
    myLan.portuguese = tr("portuguese");
    myLan.myanmar = tr("myanmar");
    myLan.bengali = tr("bengali");
    myLan.indonesia = tr("indonesia");
    myLan.poland=tr("poland");
    myLan.cfm_change_language = tr("cfm_change_language");
    myLan.cfm_clear_time = tr("cfm_clear_time");
    myLan.cfm_ts_calibrate = tr("cfm_ts_calibrate");
    myLan.cfm_upgrade_screen = tr("cfm_upgrade_screen");
    myLan.cfm_upgrade_fpga = tr("cfm_upgrade_fpga");
    myLan.feeder_voltage = tr("feeder_voltage");
    myLan.feeder_voltage_high = tr("feeder_voltage_high");
    myLan.feeder_voltage_low = tr("feeder_voltage_low");
    myLan.standalone = tr("standalone");
    myLan.master = tr("master");
    myLan.slave = tr("slave");

    // remote control
    myLan.server = tr("server");
    myLan.server_ip = tr("server_ip");
    myLan.server_port = tr("server_port");
    myLan.local_virtual_ip = tr("local_virtual_ip");
    myLan.username = tr("username");
    myLan.password = tr("password");
    myLan.connect = tr("connect");
    myLan.disconnect = tr("disconnect");
    myLan.msg_connecting = tr("msg_connecting");
    myLan.msg_connect_fail = tr("msg_connect_fail");

    // big data
    myLan.big_data = tr("big_data");
    myLan.data_center = tr("data_center");
    myLan.value_added = tr("value_added");
    myLan.service = tr("service");

    // fpga upgrade
    myLan.msg_insert_udisk = tr("msg_insert_udisk");
    myLan.last = tr("last");
    myLan.next = tr("next");
    myLan.msg_insert_data_wire = tr("msg_insert_data_wire");
    myLan.msg_remove_data_wire = tr("msg_remove_data_wire");
    myLan.user_mode = tr("user_mode");
    myLan.board_num = tr("board_num");
    myLan.upgrade = tr("upgrade");
    myLan.upgrade_retry = tr("upgrade_retry");
    myLan.upgrade_whole = tr("upgrade_whole");
    myLan.upgrade_return = tr("update_return");
    myLan.msg_int_to_factory = tr("msg_int_to_factory");
    myLan.msg_int_download_file = tr("msg_int_download_file");
    myLan.msg_int_burn_file = tr("msg_int_burn_file");
    myLan.msg_int_to_user = tr("msg_int_to_user");
    myLan.msg_color_download_file = tr("msg_color_download_file");
    myLan.msg_color_to_factory = tr("msg_color_to_factory");
    myLan.msg_color_burn_file = tr("msg_color_burn_file");
    myLan.msg_color_to_user = tr("msg_color_to_user");
    myLan.msg_inf_download_file = tr("msg_inf_download_file");
    myLan.msg_inf_to_factory = tr("msg_inf_to_factory");
    myLan.msg_inf_burn_file = tr("msg_inf_burn_file");
    myLan.msg_inf_to_user = tr("msg_inf_to_user");
    myLan.msg_ctrl_to_factory = tr("msg_ctrl_to_factory");
    myLan.msg_ctrl_erase = tr("msg_ctrl_erase");
    myLan.msg_ctrl_burn_file = tr("msg_ctrl_burn_file");
    myLan.msg_ctrl_to_user = tr("msg_ctrl_to_user");

    myLan.msg_enable_upgrade_board = tr("msg_enable_upgrade_board");
    myLan.msg_major_to_factory = tr("msg_major_to_factory");
    myLan.msg_major_file_to_int = tr("msg_major_file_to_int");
    myLan.msg_major_download_file = tr("msg_major_download_file");
    myLan.msg_major_burn_file = tr("msg_major_burn_file");
    myLan.msg_major_to_user = tr("msg_major_to_user");

    myLan.msg_assist_to_factory = tr("msg_assist_to_factory");
    myLan.msg_assist_file_to_int = tr("msg_assist_file_to_int");
    myLan.msg_assist_file_download_to_major = tr("msg_assist_file_download_to_major");
    myLan.msg_assist_download_file = tr("msg_assist_download_file");
    myLan.msg_assist_burn_file = tr("msg_assist_burn_file");
    myLan.msg_assist_to_user = tr("msg_assist_to_user");

    myLan.upgrade_file_list = tr("upgrade_file_list");
    myLan.upgrade_usb_communication = tr("upgrade_usb_communication");
    myLan.upgrade_board = tr("upgrade_board");
    myLan.upgrade_doing = tr("upgrade_doing");
    myLan.upgrade_fail = tr("upgrade_fail");
    myLan.upgrade_untreated = tr("upgrade_untreated");
    myLan.upgrade_ok = tr("upgrade_ok");
    myLan.msg_file_open_error = tr("msg_file_open_error");
    myLan.msg_no_inf = tr("msg_no_inf");
    myLan.msg_no_color = tr("msg_no_color");
    myLan.msg_unknown_file = tr("msg_unknown_file");
    myLan.cfm_insert_datawire = tr("cfm_insert_datawire");
    myLan.msg_select_user_mode = tr("msg_select_user_mode");
    myLan.cfm_upgrade_whole = tr("cfm_upgrade_whole");
    myLan.select_file = tr("select_file");
    myLan.msg_select_right_file = tr("msg_select_right_file");

    // color camera
    myLan.color_camera = tr("color_camera");
    myLan.confirm_set_camera = tr("confirm_set_camera");
    myLan.confirm_set_infra_camera = tr("confirm_set_infra_camera");
    myLan.color_calibrate = tr("color_calibrate");
    myLan.color_gain_adjust = tr("color_gain_adjust");
    myLan.color_analog_gain = tr("color_analog_gain");
    myLan.color_lens_params = tr("color_lens_params");
    myLan.color_biasing = tr("color_biasing");
    myLan.color_digital_gain = tr("color_digital_gain");
    myLan.color_digital_gain_set = tr("color_digital_gain_set");
    myLan.color_sensor_params = tr("color_sensor_params");
    myLan.gain_rough = tr("gain_rough");
    myLan.gain_tiny = tr("gain_tiny");
    myLan.channel_begin = tr("channel_begin");
    myLan.channel_end = tr("channel_end");
    myLan.channel_restore = tr("channel_restore");
    myLan.sensor = tr("sensor");
    myLan.row_fre = tr("row_fre");
    myLan.sensor_mode = tr("sensor_mode");
    myLan.sensor_light = tr("sensor_light");
    myLan.sensor_dark = tr("sensor_dark");
    myLan.sensor_black_white = tr("sensor_black_white");
    myLan.sensor_pixel = tr("sensor_pixel");
    myLan.front_begin = tr("front_begin");
    myLan.front_end = tr("front_end");
    myLan.rear_begin = tr("rear_begin");
    myLan.rear_end = tr("rear_end");
    myLan.upper_view_begin = tr("upper_view_begin");
    myLan.upper_view_end = tr("upper_view_end");
    myLan.lower_view_begin = tr("lower_view_begin");
    myLan.lower_view_end = tr("lower_view_end");
    myLan.upper_layer_begin = tr("upper_layer_begin");
    myLan.upper_layer_end = tr("upper_layer_end");
    myLan.lower_layer_begin = tr("lower_layer_begin");
    myLan.lower_layer_end = tr("lower_layer_end");
    myLan.mid_layer_begin = tr("mid_layer_begin");
    myLan.mid_layer_end = tr("mid_layer_end");
    myLan.begin = tr("begin");
    myLan.end = tr("end");
    myLan.auto_divide = tr("auto_divide");
    myLan.align = tr("align");
    myLan.update_capture = tr("update_capture");
    myLan.update_save = tr("update_save");
    myLan.update_capture_dark = tr("update_capture_dark");
    myLan.update_save_dark = tr("update_save_dark");
    myLan.update_auto = tr("update_auto");
    myLan.update_auto_level = tr("update_auto_level");
    myLan.channel_divide = tr("channel_divide");
    myLan.front_upper_view = tr("front_upper_view");
    myLan.front_lower_view = tr("front_lower_view");
    myLan.rear_upper_view = tr("rear_upper_view");
    myLan.rear_lower_view = tr("rear_lower_view");
    myLan.ejector = tr("ejector");
    myLan.pixel = tr("pixle");
    myLan.single_calibrate = tr("single_calibrate");
    myLan.whole_calibrate = tr("whole_calibrate");
    myLan.default_params = tr("default_params");
    myLan.auto_cal_note = tr("auto_cal_note");
    myLan.auto_cal_lightset = tr("auto_cal_lightset");
    myLan.auto_cal_gainset = tr("auto_cal_gainset");
    myLan.auto_cal_get_target = tr("auto_cal_get_target");
    myLan.rear_no_signal = tr("rear_no_signal");
    myLan.front_no_signal = tr("front_no_signal");
    /* LD4 */
    myLan.upper_master_view = tr("upper_master_view");
    myLan.lower_master_view = tr("lower_master_view");
    myLan.upper_slave_view = tr("upper_slave_view");
    myLan.lower_slave_view = tr("lower_slave_view");

    myLan.front_upper_view_begin = tr("front_upper_view_begin");
    myLan.front_lower_view_begin = tr("front_lower_view_begin");
    myLan.rear_upper_view_begin = tr("rear_upper_view_begin");
    myLan.rear_lower_view_begin = tr("rear_lower_view_begin");
    myLan.front_upper_view_end = tr("front_upper_view_end");
    myLan.front_lower_view_end = tr("front_lower_view_end");
    myLan.rear_upper_view_end = tr("rear_upper_view_end");
    myLan.rear_lower_view_end = tr("rear_lower_view_end");
    myLan.left_side = tr("left_side");
    myLan.right_side = tr("right_side");
    myLan.upper_view_page = tr("upper_view_page");
    myLan.lower_view_page = tr("lower_view_page");
    myLan.front_view_page = tr("front_view_page");
    myLan.rear_view_page = tr("rear_view_page");
    myLan.upper_layer_page = tr("upper_layer_page");
    myLan.lower_layer_page = tr("lower_layer_page");
    myLan.down_front_left_view_begin= tr("down_front_left_view_begin");
    myLan.down_front_left_view_end= tr("down_front_left_view_end");
    myLan.down_front_right_view_begin= tr("down_front_right_view_begin");
    myLan.down_front_right_view_end= tr("down_front_right_view_end");
    myLan.down_rear_left_view_begin= tr("down_rear_left_view_begin");
    myLan.down_rear_left_view_end= tr("down_rear_left_view_end");
    myLan.down_rear_right_view_begin= tr("down_rear_right_view_begin");
    myLan.down_rear_right_view_end= tr("down_rear_right_view_end");
    myLan.up_front_left_view_begin= tr("up_front_left_view_begin");
    myLan.up_front_left_view_end= tr("up_front_left_view_end");
    myLan.up_front_right_view_begin= tr("up_front_right_view_begin");
    myLan.up_front_right_view_end= tr("up_front_right_view_end");
    myLan.up_rear_left_view_begin= tr("up_rear_left_view_begin");
    myLan.up_rear_left_view_end= tr("up_rear_left_view_end");
    myLan.up_rear_right_view_begin= tr("up_rear_right_view_begin");
    myLan.up_rear_right_view_end= tr("up_rear_right_view_end");

    //LD4&LDS2
    myLan.first_lower_layer = tr("first_lower_layer");
    myLan.first_upper_layer = tr("first_upper_layer");
    myLan.second_lower_layer = tr("second_lower_layer");
    myLan.second_upper_layer = tr("second_upper_layer");
    myLan.ld4_1st_upper_master = tr("1st_upper_master");
    myLan.ld4_1st_lower_master = tr("1st_lower_master");
    myLan.ld4_2nd_upper_master = tr("2nd_upper_master");
    myLan.ld4_2nd_lower_master = tr("2nd_lower_master");
    myLan.ld4_1st_upper_slave = tr("1st_upper_slave");
    myLan.ld4_1st_lower_slave = tr("1st_lower_slave");
    myLan.ld4_2nd_upper_slave = tr("2nd_upper_slave");
    myLan.ld4_2nd_lower_slave = tr("2nd_lower_slave");

    // infrared camera
    myLan.inf_camera = tr("inf_camera");
    myLan.inf_gain = tr("inf_gain");
    myLan.inf_channel = tr("inf_channel");
    myLan.inf_biasing = tr("inf_biasing");
    myLan.inf_update = tr("inf_update");
    myLan.inf_odd = tr("inf_odd");
    myLan.inf_even = tr("inf_even");
    myLan.inf_positive = tr("inf_positive");
    myLan.inf_negative = tr("inf_negative");
    myLan.inf_pixel_base = tr("inf_pixel_base");
    myLan.inf_reference = tr("inf_reference");
    myLan.inf = tr("inf");

    // screen saver
    myLan.set_screensaver = tr("set_screensaver");
    myLan.enable_screensaver = tr("enable_screensaver");
    myLan.input_screensaver_password = tr("input_screensaver_password");
    myLan.confirm_screensaver_password = tr("confirm_screensaver_password");

    // message
    myLan.msg_statistic = tr("msg_statistic");
    myLan.msg_applying = tr("msg_applying");
    myLan.msg_turning_on = tr("msg_turning_on");
    myLan.msg_turning_off = tr("msg_turning_off");
    myLan.msg_turn_off_first = tr("msg_turn_off_first");
    myLan.msg_shutdown = tr("msg_shutdown");
    myLan.msg_wiping = tr("msg_wiping");
    myLan.msg_password_error = tr("msg_password_error");
    myLan.msg_divide_channel = tr("msg_divide_channel");
    myLan.msg_communicating = tr("msg_communicating");
    myLan.msg_require_version = tr("msg_require_version");
    myLan.msg_system_init = tr("msg_system_init");
    myLan.msg_preheating = tr("msg_preheating");
    myLan.msg_input_name = tr("msg_input_name");
    myLan.msg_mode_changing = tr("msg_mode_changing");
    myLan.msg_testing = tr("msg_testing");
    myLan.msg_change_init_mode = tr("msg_change_init_mode");
    myLan.msg_delay_get_zero = tr("msg_delay_get_zero");
    myLan.msg_saving_params =  tr("msg_saving_params");
    myLan.msg_params_save_error = tr("msg_params_save_error");
    myLan.msg_usb_init_error = tr("msg_usb_init_error");
    myLan.timeout = tr("timeout");
    myLan.msg_auto_calibrating = tr("msg_auto_calibrating");
    myLan.msg_auto_calibrate = tr("msg_auto_calibrate");
    myLan.msg_length_exceed_limit = tr("msg_length_exceed_limit");
    myLan.msg_set_impure = tr("msg_set_impure");

    // confirm
    myLan.cfm_save_parameter = tr("cfm_save_parameter");
    myLan.cfm_turn_on_while_belt_off = tr("cfm_turn_on_while_belt_off");
    myLan.cfm_shut_down = tr("cfm_shut_down");
    myLan.cfm_turn_on = tr("cfm_turn_on");
    myLan.cfm_auto_calibrate = tr("cfm_auto_calibrate");
    myLan.cfm_copy_to_all = tr("cfm_copy_to_all");
    myLan.cfm_change_analog_gain = tr("cfm_change_analog_gain");
    myLan.cfm_auto_set = tr("cfm_auto_set");
    myLan.cfm_set_background_color = tr("cfm_set_background_color");

    myLan.init_autoSig = tr("init_autoSig");
    myLan.cfm_init_autoSig = tr("cfm_init_autoSig");
    myLan.msg_initing_autoSig = tr("msg_initing_autoSig");
    myLan.msg_init_autoSig_fail = tr("msg_init_sutoSig_fail");
    myLan.cfm_reboot = tr("cfm_reboot");

    // general
    myLan.config_major = tr("config_major");
    myLan.config_assist = tr("config_assist");
    myLan.row = tr("row");
    myLan.col = tr("col");
    myLan.mode_dark = tr("mode_dark");
    myLan.mode_discolor = tr("mode_discolor");
    myLan.mode_discolor_reverse = tr("mode_discolor_reverse");
    myLan.mode_milky = tr("mode_milky");
    myLan.mode_milky_reverse = tr("mode_milky_reverse");
    myLan.mode_yellow_milky = tr("mode_yellow_milky");
    myLan.mode_yellow_trans = tr("mode_yellow_trans");
    myLan.mode_yellow_yellow_reverse = tr("mode_yellow_yellow_reverse");
    myLan.mode_yellow_white_reverse = tr("mode_yellow_white_reverse");
    myLan.mode_user_defined = tr("mode_user_defined");
    myLan.detect_wait = tr("detect_wait");

    // sort params
    myLan.bad_discolor = tr("bad_discolor");
    myLan.bad_area = tr("bad_area");
    myLan.auto_set = tr("auto_set");
    myLan.super_brain = tr("super_brain");
    myLan.eject_set = tr("eject_set");
    myLan.set_by_times = tr("set_by_times");
    myLan.set_alone = tr("set_alone");
    myLan.arith_scale = tr("arith_scale");
    myLan.arith_shape = tr("arith_shape");

    // camera signal
    myLan.camera_signal = tr("camera_signal");
    myLan.channel_biasing = tr("channel_biasing");
    myLan.odd_biasing = tr("odd_biasing");
    myLan.even_biasing = tr("even_biasing");
    myLan.channel_range = tr("channel_range");
    myLan.channel_cross = tr("channel_cross");
    myLan.gain_target = tr("gain_target");
    myLan.eject_times = tr("eject_times");
    myLan.image_capture = tr("image_capture");

    // vender set
    myLan.lamp_protect = tr("lamp_protect");
    myLan.feeder_biasing = tr("feeder_biasing");
    myLan.system_test = tr("system_test");
    myLan.assist_camera_enable = tr("assist_camera_enable");
    myLan.advanced_parameters = tr("advanced_parameters");
    myLan.exit_list = tr("exit_list");
    myLan.exposure_time = tr("exposure_time");
    myLan.lens_select = tr("lens_select");
    myLan.time_short = tr("time_short");
    myLan.time_middle = tr("time_middle");
    myLan.time_long = tr("time_long");
    myLan.delay_biasing = tr("delay_biasing");
    myLan.degauss_time = tr("degauss_time");
    myLan.truncation_time = tr("truncation_time");
    myLan.clear_total = tr("clear_total");
    myLan.preheat_time = tr("preheat_time");
    myLan.machine_type_select = tr("machine_type_select");
    myLan.times_allocation = tr("times_allocation");
    myLan.pass_allocation = tr("pass_allocation");
    myLan.feeder_control = tr("feeder_control");
    myLan.feeder_control_enable = tr("feeder_control_enable");
    myLan.feeder_remote_control = tr("feeder_remote_control");
    myLan.forward_direction = tr("forward_direction");
    myLan.opposite_direction = tr("opposite_direction");
    myLan.input_ai_params = tr("input_ai_params");
    myLan.identify_analysis = tr("identify_analysis");

    // system test
    myLan.test_pass_num = tr("test_pass_num");
    myLan.weight_test_before = tr("weight_test_before");
    myLan.weight_test_after = tr("weight_test_after");
    myLan.single_pass_output = tr("single_pass_output");
    myLan.auto_running = tr("auto_running");
    myLan.output_test_set = tr("output_test_set");
    myLan.output_test_start = tr("output_test_start");

    // image capture
    myLan.grey = tr("grey");
    myLan.matter_area = tr("matter_area");
    myLan.image_derive = tr("image_derive");
    myLan.rice_sum = tr("rice_sum");
    myLan.broken_rice_num = tr("broken_rice_num");
    myLan.broken_rice_ratio = tr("broken_rice_ratio");

    //video
    myLan.video_starting_info = tr("video starting");
    myLan.video_stoping_info = tr("video stoping");
    myLan.real_output = tr("output");
    myLan.real_dirtper = tr("dirt per");

    // auto set
    myLan.current_feeder = tr("current_feeder");
    myLan.fixed_feeder = tr("fixed_feeder");
    myLan.reference_chute = tr("reference_chute");
    myLan.special_for_deep = tr("special_for_deep");

    // bad area
    myLan.pale_yellow_area = tr("pale_yellow_area");
    myLan.bad_width = tr("bad_width");

    // inf set
    myLan.infrared_set = tr("infrared_set");
    myLan.special_for_glass = tr("special_for_glass");
    myLan.follow_lower_limit = tr("follow_lower_limit");

    // lamp set
    myLan.front_sort = tr("front_sort");
    myLan.rear_sort = tr("rear_sort");

    // eject times
    myLan.sampling_time = tr("sampling_time");
    myLan.eject_times_limit = tr("eject_times_limit");

    // white board adjust
    myLan.white_board_adjust = tr("white_board_adjust");
    myLan.adjust_start = tr("adjust_start");
    myLan.adjust_finish = tr("adjust_finish");
    myLan.auto_copy = tr("auto_copy");
    myLan.co_capture = tr("co_capture");
    myLan.cuteoff_bg = tr("cuteoff_bg");
    myLan.adjust_copy = tr("adjust_copy");
    myLan.adjust_wave = tr("adjust_wave");
    myLan.channel = tr("channel");

    // message
    myLan.msg_input_weight_before = tr("msg_input_weight_before");
    myLan.msg_input_weight_after = tr("msg_input_weight_after");
    myLan.msg_system_testing = tr("msg_system_testing");
    myLan.msg_insert_white_board = tr("msg_insert_white_board");
    myLan.msg_white_board_adjusting = tr("msg_white_board_adjusting");
    myLan.msg_remove_white_board = tr("msg_remove_white_board");

    // copy major params
    myLan.scheme_cur = tr("scheme_cur");
    myLan.scheme_des = tr("scheme_des");
    myLan.scheme_all = tr("scheme_all");

    /*!<< pmt */
    myLan.pmt_run    = tr("pmt_run");
    myLan.pmt_front_frame = tr("pmt_front_frame");
    myLan.pmt_rear_frame  = tr("pmt_rear_frame");
    myLan.pmt_enable_red1 = tr("pmt_enable_red1");
    myLan.pmt_enable_red2 = tr("pmt_enable_red2");
    myLan.pmt_enable_green= tr("pmt_enable_green");
    myLan.pmt_enable_blue = tr("pmt_enable_blue");
    myLan.pmt_ctrl_voltage= tr("pmt_ctrl_voltage");
    myLan.pmt_ctrl        = tr("pmt_ctrl");

    //feederAI
    myLan.feeder_AI = tr("feeder_AI");
    myLan.eject_stat_enable = tr("eject_stat_enable");
    myLan.current_eject_freq = tr("current_eject_freq");
    myLan.feeder_range = tr("feeder_range");
    myLan.ejector_range = tr("ejector_range");

    //log
    myLan.log = tr("Log");
    myLan.from = tr("From");
    myLan.to = tr("To");
    myLan.fresh = tr("Load");
    myLan.all = tr("All");
    myLan.power_switch = tr("Power Switch");
    myLan.feeder_switch = tr("Feeder Switch");
    myLan.machine = tr("Machine");
    myLan.profile = tr("Profile");
    myLan.camera = tr("Camera");
    myLan.bg = tr("Backgroud");
    myLan.arith = tr("Arith");
    myLan.reject = tr("Reject");
    myLan.wipe = tr("Wipe");
    //myLan.feeder_value = ;
    myLan.debug = tr("Debug");

    //LD4
    myLan.upper_master = tr("upper_master");
    myLan.upper_slave = tr("upper_slave");
    myLan.lower_master = tr("lower_master");
    myLan.lower_slave = tr("lower_slave");
    myLan.upper_like_lower = tr("upper same as lower");
    myLan.master_page = tr("Master-Page");
    myLan.slave_page = tr("Slave-Pge");
    myLan.upper_L_begin = tr("upper_L_begin");
    myLan.upper_L_end = tr("upper_L_end");
    myLan.upper_R_begin = tr("upper_R_begin");
    myLan.upper_R_end = tr("upper_R_end");
    myLan.lower_L_begin = tr("lower_L_begin");
    myLan.lower_L_end = tr("lower_L_end");
    myLan.lower_R_begin = tr("lower_R_begin");
    myLan.lower_R_end = tr("lower_R_end");
    myLan.unused = tr("Unused");

    //color select
    myLan.color_select_mode = tr("color_select_mode");
    myLan.set_color_select_mode = tr("set_color_select_mode");
    myLan.black_tea = tr("black_tea");
    myLan.white_bar_tea = tr("white_bar_tea");
    myLan.yeloow_green_bar_piece = tr("yeloow_green_bar_piece");
    myLan.red_bar_red_piece = tr("red_bar_red_piece");

    //bigData
    myLan.bigData = tr("bigData");
    myLan.state = tr("state");
    myLan.state_info = tr("state_info");
    myLan.backgroud = tr("backgroud");
    myLan.material = tr("material");
    myLan.backgroud_statistic = tr("backgroud_statistic");
    myLan.identify = tr("identify");
    myLan.identify_statistic = tr("identify_statistic");
    myLan.eject = tr("eject");
    myLan.eject_statistic = tr("eject_statistic");
    myLan.histogram = tr("histogram");
    myLan.histogram_statistic = tr("histogram_statistic");
    myLan.configration = tr("configration");
    myLan.cameraReference = tr("cameraReference");
    myLan.cameraLine = tr("cameraLine");
    myLan.cameraList = tr("cameraList");
    myLan.time_statistic = tr("time_statistic");
    myLan.material_total = tr("material_total");
    myLan.material_bad_total = tr("material_bad_total");

    //accurate tick
    myLan.common = tr("common");
    myLan.accurate = tr("accurate");
    myLan.eject_width_max = tr("eject_width_max");
    myLan.eject_width_min = tr("eject_width_min");
    myLan.eject_width_dif = tr("eject_width_dif");
    myLan.rounds = tr("rounds");
    myLan.tick_mode = tr("tick_mode");
    myLan.tick_sort_mode_warm = tr("tick_sort_mode_warm");
    //contra test
    myLan.contra_test = tr("contra_test");
    myLan.bias = tr("bias");
    myLan.correct = tr("correct");
    myLan.send_bias = tr("send_bias");
    myLan.msg_accept_data = tr("msg_accept_data");
    myLan.msg_receiving = tr("msg_receiving");

    myLan.pic_customer = tr("pic_customer");
    myLan.schemeNew_select = tr("schemeNew_select");
    myLan.videoNew = tr("videoNew");
    myLan.save_parameterNew = tr("save_parameterNew");
    myLan.working_stop = tr("working_stop");
    myLan.working_start = tr("working_start");

    myLan.set_sens = tr("set_sens");
    myLan.feeder_speed = tr("feeder_speed");
    myLan.test_ejector = tr("test_ejector");
    myLan.set_wiper = tr("set_wiper");
    myLan.set_function = tr("set_function");
    myLan.operation_level = tr("operation_level");

    myLan.new_scheme = tr("new_scheme");
    myLan.copy_scheme = tr("copy_scheme");
    myLan.delte_scheme = tr("delte_scheme");
    myLan.rename_scheme = tr("rename_scheme");
    myLan.recover_scheme = tr("recover_scheme");
    myLan.backup_scheme = tr("backup_scheme");

    myLan.customer_model = tr("customer_model");
    myLan.sys_set = tr("sys_set");
    myLan.network = tr("network");
    myLan.help = tr("help");
    myLan.device = tr("device");
    myLan.capture = tr("capture");
    myLan.algorithm = tr("algorithm");
    myLan.aux_parameter = tr("aux_parameter");
    myLan.set_time = tr("set_time");
    myLan.io_set = tr("io_set");

    myLan.front_material = tr("front_material");
    myLan.front_background = tr("front_background");
    myLan.back_material = tr("back_material");
    myLan.back_background = tr("back_background");

    myLan.camera_new = tr("camera_new");
    myLan.set_machine = tr("set_machine");
    myLan.test_output = tr("test_output");
    myLan.old_test = tr("old_test");
    myLan.set_vib_voltage = tr("set_vib_voltage");
    myLan.tdemag = tr("tdemag");
    myLan.setLight = tr("setLight");
//    myLan.tick_mode = tr("tick_mode");
    myLan.encrypt = tr("encrypt");
    myLan.out = tr("out");
    myLan.bad_ratio = tr("bad_ratio");
    myLan.background_proportion = tr("background_proportion");

    //新增
    myLan.salesperson = tr("salesperson");
    myLan.telephone = tr("telephone");
    myLan.expiration_time = tr("expiration_time");

    myLan.illegal_password_decryption = tr("illegal_password_decryption");
    myLan.decryption_successful = tr("decryption_successful");
    myLan.decryption_process_error = tr("decryption_process_error");

    myLan.illegal_password_encryption = tr("illegal_password_encryption");
    myLan.encryption_successful = tr("encryption_successful");
    myLan.encryption_process_error = tr("encryption_process_error");
    myLan.target_value = tr("target_value");

    myLan.download_image = tr("download_image");
        myLan.image_reasoning = tr("image_reasoning");
        myLan.image_acquisition = tr("image_acquisition");

        myLan.pixel_info = tr("pixel_info");
        myLan.ejector_para = tr("ejector_para");
        myLan.image_height = tr("image_height");
        myLan.model_para = tr("model_para");
        myLan.model_update = tr("model_update");
        myLan.ai_version = tr("ai_version");
        myLan.disk_space = tr("disk_space");

        myLan.img_downloading = tr("img_downloading");
        myLan.ftp_status_error = tr("ftp_status_error");
        myLan.img_download_finished = tr("img_download_finished");
        myLan.delete_original_image = tr("delete_original_image");
        myLan.deleting = tr("deleting");
        myLan.img_list_blank = tr("img_list_blank");
        myLan.delete_original_image_finished = tr("delete_original_image_finished");
        myLan.setting_error = tr("setting_error");
        myLan.setting_succeeded = tr("setting_succeeded");
        myLan.setting_value_error = tr("setting_value_error");

        myLan.model_list_blank = tr("model_list_blank");
        myLan.model_para_error = tr("model_para_error");
        myLan.beginning = tr("beginning");
        myLan.error = tr("error");
        myLan.succeed = tr("succeed");
        myLan.fixed_delay = tr("fixed_delay");
        myLan.dynamic_delay = tr("dynamic_delay");
        myLan.acquisition_height = tr("acquisition_height");
        myLan.reasoning_height = tr("reasoning_height");

        myLan.rename = tr("rename");
        myLan.add = tr("add");
        myLan.model = tr("model");
        myLan.id_blank = tr("id_blank");
        myLan.json_file = tr("json_file");
        myLan.dlc_file = tr("dlc_file");
        myLan.content = tr("content");
        myLan.open = tr("open");
        myLan.notexist = tr("notexist");
        myLan.upload = tr("upload");
        myLan.model_exist_msg = tr("model_exist_msg");

        myLan.spray_valve_number = tr("spray_valve_number");
        myLan.nozzle_number = tr("nozzle_number");
        myLan.spray_time_interval = tr("spray_time_interval");

}

/* 设置界面语言 */
void GlobalFlow::setLang()
{
    QTranslator *trans = new QTranslator;
    switch(struCnfg.nLang){
    case LANG_CHS:
        if(QFile(QString("%1zh-cn.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1zh-cn.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1zh-cn.qm").arg(LANG_DIR));
        }
        break;
    case LANG_ENG:
        if(QFile(QString("%1en.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1en.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1en.qm").arg(LANG_DIR));
        }
        break;
    case LANG_SPAN:
        if(QFile(QString("%1es.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1es.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1es.qm").arg(LANG_DIR));
        }
        break;
    case LANG_RUS:
        if(QFile(QString("%1ru.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1ru.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1ru.qm").arg(LANG_DIR));
        }
        break;
    case LANG_VIET:
        if(QFile(QString("%1vi.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1vi.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1vi.qm").arg(LANG_DIR));
        }
        break;
    case LANG_THAILAND:
        if(QFile(QString("%1th.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1th.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1th.qm").arg(LANG_DIR));
        }
        break;
    case LANG_CHT:
        if(QFile(QString("%1zh-tw.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1zh-tw.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1zh-tw.qm").arg(LANG_DIR));
        }
        break;
    case LANG_TURKEY:
        if(QFile(QString("%1tr.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1tr.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1tr.qm").arg(LANG_DIR));
        }
        break;
   case LANG_FARSIE:
        if(QFile(QString("%1fa.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1fa.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1fa.qm").arg(LANG_DIR));
        }
        break;
    case LANG_FRENCHE:
         if(QFile(QString("%1fr.qm").arg(LANG_DIR_SD)).exists()) {
             trans->load(QString("%1fr.qm").arg(LANG_DIR_SD));
         } else {
             trans->load(QString("%1fr.qm").arg(LANG_DIR));
         }
         break;
    case LANG_UYGHUR:
        if(QFile(QString("%1ug.qm").arg(LANG_DIR_SD)).exists()){
            trans->load(QString("%1ug.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1ug.qm").arg(LANG_DIR));
        }
        break;
    case LANG_KOREANE:
        if(QFile(QString("%1ko.qm").arg(LANG_DIR_SD)).exists()){
            trans->load(QString("%1ko.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1ko.qm").arg(LANG_DIR));
        }
        break;
    case LANG_ARABICE:
        if(QFile(QString("%1ar.qm").arg(LANG_DIR_SD)).exists()){
            trans->load(QString("%1ar.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1ar.qm").arg(LANG_DIR));
        }
        break;
    case LANG_BULGARIANE:
        if (QFile(QString("%1bg.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1bg.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1bg.qm").arg(LANG_DIR));
        }
        break;
    case LANG_SLOVAKE:
        if (QFile(QString("%1sk.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1sk.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1sk.qm").arg(LANG_DIR));
        }
        break;
    case LANG_PORTUGUESEE:
        if (QFile(QString("%1pt.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1pt.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1pt.qm").arg(LANG_DIR));
        }
        break;
    case LANG_MYANMAR:
        if (QFile(QString("%1my.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1my.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1my.qm").arg(LANG_DIR));
        }
        break;
    case LANG_BENGALIE:
        if (QFile(QString("%1bn.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1bn.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1bn.qm").arg(LANG_DIR));
        }
        break;
    case LANG_INDONESIA:
        if (QFile(QString("%1id.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1id.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1id.qm").arg(LANG_DIR));
        }
        break;
    case LANG_POLAND:
        if (QFile(QString("%1pl.qm").arg(LANG_DIR_SD)).exists()) {
            trans->load(QString("%1pl.qm").arg(LANG_DIR_SD));
        } else {
            trans->load(QString("%1pl.qm").arg(LANG_DIR));
        }
        break;
    default:
        break;
    }

    QCoreApplication::installTranslator(trans);
    initLan();
}
