#ifndef RESSTR_H
#define RESSTR_H
#include <string>
#include <vector>
typedef std::vector<std::string> TStringVector;
typedef enum {
    //main menu
    res_pwd,
    res_ssp,
    res_bypass_pwd,
    res_cancle_bypass,
    res_bypass_set,
    //alarm menu
    res_no,
    res_time,
    res_trolley,
    res_slewing,

    res_dist,
    res_weight,
    res_rate,
    res_angle,
    res_wild_speed,
//export
    res_export_ok,
    res_export_data,
    res_export_fail,
    res_export_no_usb,
//calib box
    res_hint,
    res_input_incorrect,
    res_height,
    res_param_save_ok,
    res_angle_ad,
    res_up_angle_ad,
    res_dist_ad,
    res_weight_ad,
    res_height_ad,
    res_min_height_record_ok,
    res_height_calib_ok,
    res_reset_height_first,
    res_min_dist_record_ok,
    res_dist_param_error,
    res_dist_calib_ok,
    res_click_min_dist_first,
    res_height_param_error,
    res_weight_param_error,
    res_input_calib_height,
    res_height_calib,
    res_input_first_weight,
    res_input_secord_weight,
    res_input_third_weight,
    res_reset_weight_ok,
    res_weight_calib_ok,
    res_reset_weight_first,
    res_a_point_record_ok,
    res_angle_param_error,
    res_angle_calib_ok,
    res_click_a_point_first,
    res_min_upangle_record_ok,
    res_upangle_param_error,
    res_upangle_calib_ok,
    res_click_min_upangle_first,
    res_moment_cfg_password,
    res_class,
    res_content,
    res_tower_no,
    res_dev_name,
    res_x_axis,
    res_y_axis,
    res_tower_height,
    res_long_arm_len,
    res_short_arm_len,
    res_min_dist,
    res_dy_type,
    res_max_upangle,
    res_min_upangle,
    res_car_pos,
    res_rate_weight,
    res_tower_type,
    res_arm_len,
    res_moment_save_ok,
    res_import_fail_usb_nodata,
    res_database_open_failed,
    res_import_ok,
    res_show_weight,
    res_show_height,
    res_show_wild,
    res_multi_mode,
    res_signal_mode,
    res_wild_13_alarm,
    res_wild_20_alarm,
    res_lang_zh,
    res_lang_en,
    res_struct_param,
    res_not_find_tctype

}str_index;

class CResStr
{
public:
    CResStr();
    static CResStr& Get();
    int LoadRes(std::string path);
    std::string at(int id);
    int size();
private:
    int m_size;

};

extern const char** g_str;
#endif // RESSTR_H
