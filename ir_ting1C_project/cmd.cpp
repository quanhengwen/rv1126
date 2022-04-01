#include "cmd.h"

//command init.it need to be called before sending command.
void command_init(void)
{
	vdcmd_init();
}

//使用新的环境变量修正温度
//1.关闭模组中的距离修正算法，NUC映射系数(P0,P1,P2)。
//2.读取模组中的环境变量参数(EMS, TAU, Ta, Tu)，计算固件中的环境变量校正系数K_E和B_E。
//2.计算更高精度并且使用新的环境变量参数(目标发射率,距离,大气温度,反射温度,湿度)下的校正系数K_E和B_E
//3.重新修正温度结果

//该函数的功能是读取固件中的测温参数
int read_nuc_parameter(TempCalInfo_t* temp_cal_info)
{
	int i = 0;
	uint8_t temp = 0;


	uint8_t data[0x4000] = { 0 };
	if(spi_read(0xda000, 0x4000, data)!=IRUVC_SUCCESS)//high gain is 0xda000 / low gain is 0xd3000
	{
		printf("get nuc-t table failed\n");
		return FAIL;
	}
	for (i = 0; i < sizeof(data);)
	{
		temp = data[i];
		data[i] = data[i + 1];
		data[i + 1] = temp;
		i = i + 2;
	}
	memcpy(temp_cal_info->nuc_table, data, 0x4000);
	return FAIL;
}

//该函数的功能是计算固件中环境变量校正的参数
int calculate_org_env_cali_parameter()
{
	uint8_t gain_flag = HIGH_GAIN;
	uint8_t data[4] = { 0 };
	TempCalInfo_t* temp_cal_info = get_temp_cal_info();
	if(get_prop_tpd_params(TPD_PROP_EMS, (uint16_t*)&temp_cal_info->org_env_param->EMS) != IRUVC_SUCCESS)
	{
		printf("get EMS failed\n");
		return FAIL;
	}
	if (get_prop_tpd_params(TPD_PROP_TAU, (uint16_t*)&temp_cal_info->org_env_param->TAU) != IRUVC_SUCCESS)
	{
		printf("get TAU failed\n");
		return FAIL;
	}
	if (get_prop_tpd_params(TPD_PROP_TA, (uint16_t*)&temp_cal_info->org_env_param->Ta) != IRUVC_SUCCESS)
	{
		printf("get TA failed\n");
		return FAIL;
	}
	if (get_prop_tpd_params(TPD_PROP_TU, (uint16_t*)&temp_cal_info->org_env_param->Tu) != IRUVC_SUCCESS)
	{
		printf("get TU failed\n");
		return FAIL;
	}
	if (calculate_org_KE_and_BE_with_nuc_t(temp_cal_info->org_env_param, temp_cal_info->nuc_table,\
							temp_cal_info->gain_flag, temp_cal_info->org_env_factor) != IRTEMP_SUCCESS)
	{
		printf("calculate_KE_and_BE failed\n");
		return FAIL;
	}
	return SUCCESS;
}






//标定TAU
//1.首先应该将KTBT开关打开
//2.分别获取高低温黑体dist m ,高低温黑体0.25m处的 画面中心的NUC值
//3.计算该距离下的tau
//4.向二进制文件中写入当前湿度，温度和距离下的tau 

//下次使用直接根据湿度，温度和距离读取文件中的tau值(经过双线性插值)即可



//command selection
void command_sel(int cmd_type)
{
	int rst = 0;
	double  t_env = 0;


	uint8_t id_data[8] = { 0 };
	uint8_t flash_data[0x30] = { 0 };
	uint16_t temp_data = 0;
	uint8_t r_mirror_flip = 0;

	IruvcPoint_t point_1 = { 160,160 };
	IruvcRect_t rect1 = { 100, 100, 300, 300 };
	IruvcRect_t rect2 = { 10, 10, 100, 100 };
	TpdLineRectTempInfo_t rect1_temp_info = { 0 };

	IruvcPoint_t point_pos = { 80,60 };

	double org_temp,new_temp;
	uint16_t temp;
	TempCalInfo_t* temp_cal_info = get_temp_cal_info();
	FILE* fp;
	float ems = 1;
	float ta = 25;
	float new_temp1 = 0;
	float new_temp2 = 0;
	float d = 0;
	uint16_t tau;
	uint8_t ooc_data[0xd9c8] = { 0 };
	
	uint16_t BlackTemp = 0;
	switch (cmd_type)
	{
	case 0:
		get_device_info(DEV_INFO_PROJECT_INFO, id_data);
		printf("get_device_info:");
		for (int j = 0; j < 8; j++)
		{
			printf("0x%x ", id_data[j]);
		}
		printf("\n");
		break;
	case 1:
		shutter_sta_set(SHUTTER_CTL_EN);
		ooc_b_update(B_UPDATE);
		printf("shutter\n");
		break;
	case 2:
		shutter_sta_set(SHUTTER_CTL_DIS);
		shutter_manual_switch(SHUTTER_OPEN);
		ooc_b_update(B_UPDATE);
		shutter_sta_set(SHUTTER_CTL_EN);
		printf("shutter background\n");
		break;
	case 3:
		rst = zoom_center_up(PREVIEW_PATH0, ZOOM_STEP4);
		printf("zoom_center_up\n");
		break;
	case 4:
		rst = zoom_center_down(PREVIEW_PATH0, ZOOM_STEP4);
		printf("zoom_center_down\n");
		break;
	case 5:
		get_device_info(DEV_INFO_PROJECT_INFO, id_data);
		printf("get device info project info:");
		for (int j = 0; j < 4; j++)
		{
			printf("0x%x ", id_data[j]);
		}
		printf("\n");
		break;
	case 10:
		tpd_get_rect_temp_info(rect1, &rect1_temp_info);
		printf("tpd_get_rect_temp_info:min(%d,%d):%d, max(%d,%d):%d\n", \
			rect1_temp_info.min_temp_point.x,rect1_temp_info.min_temp_point.y, \
			rect1_temp_info.temp_info_value.min_temp,\
			rect1_temp_info.max_temp_point.x, rect1_temp_info.max_temp_point.y,\
			rect1_temp_info.temp_info_value.max_temp);
		break;
	case 11:
		cur_vtemp_get(&temp_data);
		printf("cur_vtemp_get:%d\n", temp_data);
		break;
	case 13:
		spi_read(0x28e000, 0x30, flash_data);
		for (int j = 0; j < 0x30; j++)
		{
			printf("0x%x ", flash_data[j]);
		}
		printf("spi_read 0x28e000\n");
		break;
	case 14:
		y16_preview_start(PREVIEW_PATH0, (y16_isp_stream_src_types)Y16_MODE_SNR);
		printf("y16_preview_start temp\n");
		break;
	case 15:
		y16_preview_stop(PREVIEW_PATH0);
		printf("y16_preview_stop\n");
		break;
	case 16:  //temperature correction with origin method
		read_nuc_parameter(temp_cal_info);
		printf("read_nuc_parameter\n");
		printf("nuc_table[0]=%d\n", temp_cal_info->nuc_table[0]);
		calculate_org_env_cali_parameter();
		printf("EMS=%d\n", temp_cal_info->org_env_param->EMS);
		printf("TAU=%d\n", temp_cal_info->org_env_param->TAU);
		printf("TA=%d\n", temp_cal_info->org_env_param->Ta);
		printf("Tu=%d\n", temp_cal_info->org_env_param->Tu);
		break;
	case 17: //temperature correction with origin method
		fp = fopen("tau_H.bin", "rb");
		fread(correct_table, 1, sizeof(correct_table), fp);
		fclose(fp);
		calculate_new_env_cali_parameter(correct_table,1, 27, 27, 0.25, 1);
		tpd_get_point_temp_info(point_pos, &temp);
		org_temp = (double)temp / 16;
		temp_calc_with_new_env_calibration(temp_cal_info, org_temp, &new_temp);
		printf("org_temp=%f\n", org_temp - 273.15);
		printf("new_temp=%f\n", new_temp - 273.15);
		break;
	case 18:  //temperature correction with new method
		fp = fopen("new_tau_H.bin", "rb");
		fread(correct_table, 1, sizeof(correct_table), fp);
		fclose(fp);
		d = 5;
		tpd_get_point_temp_info(point_pos, &temp);
		org_temp = (double)temp / 16 - 273.15;
		read_tau_with_target_temp_and_dist(correct_table, org_temp, d, &tau);
		temp_correct(ems, tau, ta, org_temp, &new_temp1);
		read_tau_with_target_temp_and_dist(correct_table, new_temp1, d, &tau);
		temp_correct(ems, tau, ta, org_temp, &new_temp2);
		printf("org_temp=%f\n", org_temp);
		printf("new_temp=%f\n", new_temp2);
		break;
	case 19:
		set_prop_tpd_params(TPD_PROP_GAIN_SEL, 0);
		printf("set_prop_tpd_params to low\n");
		break;
	case 20:
		set_prop_tpd_params(TPD_PROP_GAIN_SEL, 1);
		printf("set_prop_tpd_params to high\n");
		break;
	case 23:
		dpc_auto_calibration(30);
		printf("dpc_auto_detect completed\n");
		break;
	case 24:
		restore_default_cfg(DEF_CFG_DEAD_PIXEL);
		printf("dpc_auto_detect completed\n");
		break;
	case 25:
		zoom_position_up(PREVIEW_PATH0, ZOOM_STEP4, point_pos);
		printf("zoom_position_up completed\n");
		break;
	case 26:
		zoom_position_down(PREVIEW_PATH0, ZOOM_STEP4, point_pos);
		printf("zoom_center_down completed\n");
		break;
	case 27:
		set_prop_image_params(IMAGE_PROP_SEL_MIRROR_FLIP, 3);
		printf("IMAGE_PROP_SEL_MIRROR_FLIP completed\n");
		break;
	case 28:
		set_prop_image_params(IMAGE_PROP_SEL_MIRROR_FLIP, 2);
		printf("IMAGE_PROP_SEL_MIRROR_FLIP completed\n");
		break;
	case 29:
		pseudo_color_set(PREVIEW_PATH0, PSEUDO_COLOR_MODE_4);
		printf("PSEUDO_COLOR_SET completed\n");
		break;
	case 30:
		restore_default_cfg(DEF_CFG_TPD);
		printf("RESET_TPD completed\n");
		break;
	case 31:
		set_prop_auto_shutter_params(SHUTTER_PROP_SWITCH, 0);
		printf("CLOSE_AUTO_FFC completed\n");
		break;
	case 32:
		set_prop_auto_shutter_params(SHUTTER_PROP_SWITCH, 1);
		printf("OPEN_AUTO_FFC completed\n");
		break; 
	case 33:
		ooc_b_update(B_UPDATE);
		printf("FFC completed\n");
		break;
	case 34:
		printf("Input the temp of low temp blackbody in Celsius\n");
		scanf("%d", &BlackTemp);
		printf("The temp of low temp blackbody is %d\n", BlackTemp);
		printf("TPD_KTBT_RECAL_P1 start\n");
		BlackTemp += 273;
		tpd_ktbt_recal_2point(TPD_KTBT_RECAL_P1, BlackTemp);
		printf("TPD_KTBT_RECAL_P1 completed\n");
		break;
	case 35:
		printf("Input the temp of high temp blackbody in Celsius\n");
		scanf("%d", &BlackTemp);
		printf("The temp of high temp blackbody is %d\n", BlackTemp);
		printf("TPD_KTBT_RECAL_P2 start\n");
		BlackTemp += 273;
		tpd_ktbt_recal_2point(TPD_KTBT_RECAL_P2, BlackTemp);
		printf("TPD_KTBT_RECAL_P2 completed\n");
		break;
	default:
		break;

	}
	//printf("command rst:%d\n", rst);
}

//command thread function
void* cmd_function(void* threadarg)
{
	int cmd = 1;
	while (is_streaming)
	{
		scanf("%d", &cmd);
		if (is_streaming)
		{
			command_sel(cmd);
		}
	}
	printf("cmd thread exit!!\n");
	return NULL;
}
