#ifndef _LIBIRTEMP_H_
#define _LIBIRTEMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>

#define EPSILON 0.001
#define TEMP_NUM 14
#define DIST_NUM 64
#define TARGET_TEMP_NUM 56

#define HIGH_GAIN 1
#define LOW_GAIN  0

#define EMS_MAX 128
#define EMS_MIN  0
#define TAU_MAX 128
#define TAU_MIN  0

#define HIGH_GAIN_TA_MAX  430
#define HIGH_GAIN_TU_MAX  430
#define HIGH_GAIN_TA_MIN  230
#define HIGH_GAIN_TU_MIN  230


#define LOW_GAIN_TA_MAX   873
#define LOW_GAIN_TU_MAX   873
#define LOW_GAIN_TA_MIN   230
#define LOW_GAIN_TU_MIN   230

#define TAU_TEMP_MIN  -5
#define TAU_TEMP_MAX  55

#define TAU_DIST_MAX  50
#define TAU_DIST_MIN  0.25


#define TARGET_TEMP_MIN  248.15   //kilven temperature
#define TARGET_TEMP_MAX  1623.15  //kilven temperature

#define NUC_T_SIZE  8192
/**
* @brief  TempData resulution
*/
typedef struct {
	uint16_t width;		///< resolution width
	uint16_t height;	///< resolution height
}TempDataRes_t;


/**
* @brief  The search information of KT curve
*/
typedef struct {
	uint16_t start_point;		///< Start search point of KT curve
	uint16_t end_point;	        ///< End search point of KT curve
}SearchInfo_t;


/**
* @brief  The turning address of KT curve
*/
typedef struct {
	uint16_t start_addr;		///< Start turning address of KT curve
	uint16_t end_addr;	       ///< End turning address of KT curve
}KtInfo_t;


/**
* @brief  Some vtemp constants for vtemp convert to true temperature
*/
typedef struct {
	int16_t Ktemp;			///< Ktemp
	int16_t Btemp;	        ///< Btemp
	uint16_t Address_CA;    ///< Address_CA
}VtempInfo_t;


/**
* @brief  The environment temperature point of primary calibration 
*/
typedef struct {
	int TH;			///< High environment temperature point of primary calibration 
	int TL;	        ///< Low environment temperature point of primary calibration 
}CaliPoint_t;


/**
* @brief  The normalization coefficient of KT curve
*/
typedef struct {
	int Kn;			///< The K value of KT curve normalization coefficient
	int Bn;	        ///< The B value of KT curve normalization coefficient
}CaliFactor_t;


/**
* @brief  The environment temperature in double point calibration process
*/
typedef struct {
	double T_high;			///< The high environment temperature in double point calibration process
	double T_low;	        ///< The low environment temperature in double point calibration process
}EnvTemp_t;

/**
* @brief  The vtemp of sensor in double point calibration process
*/
typedef struct {
	int VTemp_High;			///< The vtemp of sensor when calibratie high environment temperature in double point calibration process
	int VTemp_Low;	        ///< The vtemp of sensor when calibratie low environment temperature in double point calibration process
}VTempSet_t;

/**
* @brief  The correction factor after recalibration process
*/
typedef struct {
	int K;			///< The K value of environment estimatimation after double/single point calibration process
	int B;	        ///< The B value of environment estimatimation after double/single point calibration process
}ReCaliFactor_t;

/**
* @brief  Environment factor correction parameter
*/
typedef struct {
	int EMS;		///< Target emissivity
	int TAU;		///< Atmospheric transmittance
	int Ta;			///< Atmospheric temperature
	int Tu;			///< Reflection temperature
}EnvParam_t;

/**
* @brief  Environment factor correction coefficient
*/
typedef struct {
	int K_E;		///<The K value of environment factor correction
	int B_E;		///<The B value of environment factor correction
}EnvFactor_t;


/**
* @brief  Temperature Threshold 
*/
typedef struct {
	int upper_limit;	///<The upper limit of temperature threshold 
	int lower_limit;	///<The lower limit of temperature threshold 
}TempThreshold_t;


/**
* @brief  Recalibration type
*/
typedef enum
{
	SINGLE_POINT = 0,   ///< The flag represent single point calibration process
	DOUBLE_POINT,		///< The flag represent double point calibration process
}ReCaliType_t;


/**
* @brief Alarm type in libirtemp library
*/
typedef enum
{
	TEMP_NORMAL = 0,		///< normal temperature
	OVER_HEAT = 1,			///< over heat
	OVER_COLD = 2,			///< over cold
	WRONG_TYPE = 3,			///< wrong type
}AlarmType_t;


/**
* @brief  NUC map factor
*/
typedef struct {
	int P0;			///< The constant term coefficient of T-NUC mapping
	int P1;			///< The primary term coefficient of T-NUC mapping
	int P2;			///< The quadratic term coefficient of T-NUC mapping
}NucFactor_t;


/**
* @brief  Dot_t definition 
*/
typedef struct {
	int x;		///< The horizontal coordinate of Dot_t
	int y;		///< The vertical coordinate of Dot_t
}Dot_t;


/**
* @brief  Line_t definition
*/
typedef struct {
	int start_x;	///< The start horizontal coordinate of Line_t
	int start_y;	///< The start vertical coordinate of Line_t
	int end_x;		///< The end horizontal coordinate of Line_t
	int end_y;		///< The end vertical coordinate of Line_t
}Line_t;


/**
* @brief  Area_t definition
*/
typedef struct {
	int start_x;	///< The start horizontal coordinate of Area_t
	int start_y;	///< The start vertical coordinate of Area_t
	int width;		///< The width of Area_t
	int height;		///< The height of Area_t
} Area_t;


/**
* @brief  Temperature infomation definition
*/
typedef struct {
	uint16_t max_temp;		///< The maxmium temperature of Area_t or Line_t
	uint16_t min_temp;		///< The minmium temperature of Area_t or Line_t
	uint16_t avr_temp;		///< The average temperature of Area_t or Line_t
	Dot_t    max_cord;		///< The maxmium temperature point coordinate of Area_t or Line_t
	Dot_t    min_cord;		///< The minmium temperature point coordinate of Area_t or Line_t
}TempInfo_t;

#if defined(_WIN32)
#define DLLEXPORT __declspec(dllexport)
#elif defined(linux) || defined(unix)
#define DLLEXPORT
#endif



/**
* @brief Log level definition in libirtemp library
*/
typedef enum {
	IRTEMP_LOG_DEBUG = 0,		///< print debug and error infomation
	IRTEMP_LOG_ERROR = 1,		///< only print error infomation
	IRTEMP_LOG_NO_PRINT = 2,	///< don't print debug and error infomation
}irtemp_log_level_t;



/**
* @brief Error type in libirtemp library
*/
typedef enum {
	IRTEMP_SUCCESS = 0,			///< function excute success
	IRTEMP_ERROR_PARAM = -1, 	///< invalid parameter
	MEMORY_ALLOC_FAIL=-2,       ///< dynamic alloc memory fail
	FILE_OPERATION_FAIL=-3		///< file operation fail
}irtemp_error_t;


#if defined(_WIN32)
	#define IRTEMP_DEBUG(format, ...) irtemp_debug_print("libirtemp debug [%s:%d/%s] " format "\n", \
														 __FILE__,__LINE__, __FUNCTION__, __VA_ARGS__)
	#define IRTEMP_ERROR(format, ...) irtemp_error_print("libirtemp error [%s:%d/%s] " format "\n", \
														__FILE__,__LINE__, __FUNCTION__, __VA_ARGS__)
#elif defined(linux) || defined(unix)
	#include <libgen.h>
	#define IRTEMP_DEBUG(format, ...) irtemp_debug_print("libirtemp debug [%s:%d/%s] " format "\n", \
														basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
	#define IRTEMP_ERROR(format, ...) irtemp_error_print("libirtemp error [%s:%d/%s] " format "\n", \
														basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

extern void (*irtemp_debug_print)(const char* fmt, ...);
extern void (*irtemp_error_print)(const char* fmt, ...);


/**
 * @brief Get current libirtemp library version
 *
 * @param NULL
 *
 * @return current libirtemp library version
 */
DLLEXPORT char* irtemp_version(void);


/**
 * @brief Get point's temperature,axis start from 0
 *
 * @param[in] src source temperature frame
 * @param[in] temp_res imput temperature frame resulution, detail see TempDataRes_t
 * @param[in] point imput point required to get temperature, detail see Dot_t
 * @param[out] dst object temperature value of the input point(kelvin_temp*16)  
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t get_point_temp(uint16_t* src, TempDataRes_t temp_res, Dot_t point, uint16_t* dst);


/**
 * @brief Get line's temperautre,axis start from 0,and contains the line's boundary
 *
 * @param[in] src source temperature frame
 * @param[in] temp_res imput temperature frame resulution, detail see TempDataRes_t
 * @param[in] line imput line required to get temperature, detail see Line_t
 * @param[out] temp_info Temperature infomation of the input line
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t get_line_temp(uint16_t* src,TempDataRes_t temp_res, Line_t line, TempInfo_t* temp_info);


/**
 * @brief Get rectangle's temperature,axis start from 0,and contains the rectangle's boundary
 *
 * @param[in] src source temperature frame
 * @param[in] temp_res imput temperature frame resulution, detail see TempDataRes_t
 * @param[in] rect input area required to get temperature, detail see Area_t
 * @param[out] temp_info Temperature infomation of the input area
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t get_rect_temp(uint16_t* src,TempDataRes_t temp_res, Area_t rect, TempInfo_t* temp_info);


/**
 * @brief Register log function depend on log level
 *
 * @param[in] log_level, detail see irtemp_log_level_t
 *
 */
DLLEXPORT void irtemp_log_register(irtemp_log_level_t log_level);


/**
 * @brief Point temperature over threshold alarm 
 *
 * @param[in] temp_threshold Temperature threshold,include upper limit and lower limit
 * @param[in] cur_temp current temperature. Units:K(integer)
 *
 * @return see AlarmType_t (greater than upper limit return OVER_HEAT/less than lower limit return OVER_COLD)
 */
DLLEXPORT AlarmType_t point_over_threshold_alarm(TempThreshold_t temp_threshold, uint16_t cur_temp);


/**
 * @brief Line_t or rectangle temperature over threshold alarm
 *
 * @param[in] temp_threshold Temperature threshold,include upper limit and lower limit
 * @param[in] temp_info current temperature. Units:K(integer)
 *
 * @return see AlarmType_t (max or average value greater than upper limit return OVER_HEAT/min or average value less than lower limit return OVER_COLD)
 */
DLLEXPORT AlarmType_t line_rect_over_threshold_alarm(TempThreshold_t temp_threshold, TempInfo_t* temp_info);

/**
 * @brief Find start and end turning point of KT curve
 *
 * @param[in] KT_array KT array
 * @param[in] search_info The search information of KT curve contains start_point and end_point
 * @param[in] len the length of KT array
 * @param[out] kt_info The turning address of KT curve contains start and end turning address
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t find_start_and_end_addr(const uint16_t* kt_array, const SearchInfo_t* search_info, uint16_t len, KtInfo_t* kt_info);


/**
 * @brief Calculate normalization cofficient Kn and Bn of KT curve
 *
 * @param[in] vtemp_info Vtemp Infomation contains Ktemp, Btemp, Address_CA
 * @param[in] kt_info KT Infomation contains start and end turning address
 * @param[in] cali_point CaliPoint_t contains high and low environment temperature point of primary calibration 
 * @param[out] cali_factor The normalization coefficient of KT curve contains Kn and Bn
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t calculate_Kn_and_Bn(const VtempInfo_t* vtemp_info, const KtInfo_t* kt_info, \
		const CaliPoint_t* cali_point, CaliFactor_t* cali_factor);


/**
 * @brief calibrate K and B value with single point calibration process
 *
 * @param[in] cali_factor The normalization coefficient of KT curve contains Kn and Bn
 * @param[in] cur_vtemp current vtemp from sensor
 * @param[in] t_env current environment temperature (unit:Celcius)
 * @param[out] recali_factor The correction factor after recalibration process contains K and B
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t single_point_recalibrate_KB(const CaliFactor_t* cali_factor, int cur_vtemp, \
		double t_env, ReCaliFactor_t* recali_factor);


/**
 * @brief calibrate K and B value with double point calibration process
 *
 * @param[in] cali_factor The normalization coefficient of KT curve contains Kn and Bn
 * @param[in] vtemp_set current vtemp from sensor
 * @param[in] env_temp current environment temperature (unit:Celcius)
 * @param[out] recali_factor The correction factor after recalibration process contains K and B
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t double_point_recalibrate_KB(const CaliFactor_t* cali_factor, const VTempSet_t* vtemp_set, \
		const EnvTemp_t* env_temp, ReCaliFactor_t* recali_factor);

/**
 * @brief estimate current environment temperature
 *
 * @param[in] cali_factor The normalization coefficient of KT curve contains Kn and Bn
 * @param[in] recali_factor The correction factor after recalibration process contains K and B
 * @param[in] cur_vtemp current vtemp from sensor
 * @param[out] t_env current environment temperature (unit:Celcius)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t env_temp_calculate(const CaliFactor_t* cali_factor, const ReCaliFactor_t* recali_factor, \
		uint16_t cur_vtemp, double* t_env);


/**
 * @brief calculate KE and BE which run in firmware
 *
 * @param[in] env_param The environment factor correction parameter read from device
 * @param[in] nuc_factor The NUC map factor read from device
 * @param[in] gain_flag Gain flag for environment factor validity check
 * @param[out] env_factor Environment factor correction coefficient contains K_E and B_E
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t calculate_KE_and_BE(const EnvParam_t* env_param, const NucFactor_t* nuc_factor, \
		uint8_t gain_flag, EnvFactor_t* env_factor);


/**
 * @brief reverse calculate NUC value with environment correction
 *
 * @param[in] nuc_factor The NUC map factor read from device
 * @param[in] kelvin_temp object temperature(unit:K)
 * @param[out] nuc_cal NUC value with environment correction
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t reverse_calc_NUC_with_env_correct(const NucFactor_t* nuc_factor, double kelvin_temp, uint16_t* nuc_cal);


/**
 * @brief reverse calculate NUC value without environment correction
 *
 * @param[in] env_factor factor correction coefficient contains K_E and B_E
 * @param[in] nuc_cal  NUC value with environment correction
 * @param[out] nuc_org  NUC value without environment correction
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t  reverse_calc_NUC_without_env_correct(const EnvFactor_t* env_factor, uint16_t nuc_cal, uint16_t* nuc_org);


/**
 * @brief calculate new KE and BE with higher precision
 *
 * @param[in] env_param The new environment factor correction parameter(EMS,TAU,Ta,Tu)
 * @param[in] nuc_factor The NUC map factor read from device
 * @param[in] gain_flag Gain flag for for environment factor validity check
 * @param[out] env_factor Environment factor correction coefficient contains K_E and B_E
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t calculate_new_KE_and_BE(const EnvParam_t* env_param, const NucFactor_t* nuc_factor, \
		uint8_t gain_flag, EnvFactor_t* env_factor);


/**
 * @brief recalculate new NUC value with environment correction
 *
 * @param[in] env_factor Environment factor correction coefficient contains K_E and B_E
 * @param[in] nuc_org The NUC value without environment correction
 * @param[out] nuc_cal The NUC value with environment correction
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t recalc_NUC_with_env_correct(const EnvFactor_t* env_factor, uint16_t nuc_org, uint16_t* nuc_cal);


/**
 * @brief remap tempertature with nuc_table
 *
 * @param[in] nuc_table NUC_T table
 * @param[in] nuc_cal The NUC value with environment correction
 * @param[out] temp_data Tempertature data (kelvin_temp*16)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t  remap_temp(const uint16_t* nuc_table, uint16_t nuc_cal, uint16_t* temp_data);


/**
 * @brief get NUC value of specific point from NUC frame
 *
 * @param[in] src Soure NUC frame
 * @param[in] temp_res Imput temperature frame resulution, detail see TempDataRes_t
 * @param[in] point The point required to get NUC value, detail see Dot_t
 * @param[out] nuc_value The nuc value at specific point
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t get_NUC_value(uint16_t* src, TempDataRes_t temp_res, Dot_t point, uint16_t* nuc_value);


/**
 * @brief calculate Tau depend on nuc_dist_high,nuc_dist_low,nuc_25_high,nuc_25_low
 *
 * @param[in] nuc_dist_low NUC value at dist(unit:m) facing the high temperature blackbody
 * @param[in] nuc_dist_low NUC value at dist(unit:m) facing the low temperature blackbody
 * @param[in] nuc_25_high NUC value at 0.25m facing the high temperature blackbody
 * @param[in] nuc_25_low NUC value at 0.25m facing the low temperature blackbody
 *
 * @return NUC value
 */
DLLEXPORT uint16_t calculate_tau(uint16_t nuc_dist_high, uint16_t nuc_dist_low, uint16_t nuc_25_high, uint16_t nuc_25_low);


/**
 * @brief read tau from correct table
 *
 * @param[in] correct table that store tau at different humidity,environment temperature and object distance
 * @param[in] hum humidity(0-1)
 * @param[in] t_env environment temperature (unit: Celcius)
 * @param[in] dist object distance (0.25-49.99) (unit: m)
 * @param[out] tau tau after double linear interpolation
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t read_tau(const uint16_t* correct_table, float hum, float t_env, float dist, uint16_t* tau);


/**
 * @brief write specific tau to correct table
 *
 * @param[in] correct table that store tau at different humidity,environment temperature and distance
 * @param[in] hum humidity(0-1)
 * @param[in] t_env environment temperature (unit: Celcius)
 * @param[in] dist object distance (0.25-49.99) (unit: m)
 * @param[out] tau tau that will be written
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t write_tau(uint16_t* correct_table, float hum , float t_env, float dist, uint16_t tau);


/**
 * @brief read tau from correct table
 *
 * @param[in] correct table that store tau at different target distance and target temperature
 * @param[in] target_temp target temperature (unit: Celcius) 
 * @param[in] dist target distance (0.25-49.99) (unit: m)
 * @param[out] tau tau after double linear interpolation
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t  read_tau_with_target_temp_and_dist(const uint16_t* correct_table, \
	float target_temp, float dist, uint16_t* tau);


/**
 * @brief write tau to correct table
 *
 * @param[in] correct table that store tau at different target distance and target temperature
 * @param[in] target_temp target temperature (unit: Celcius)
 * @param[in] dist target distance (0.25-49.99) (unit: m)
 * @param[out] tau tau after double linear interpolation
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t  write_tau_with_target_temp_and_dist(uint16_t* correct_table, \
	float target_temp, float dist, uint16_t tau);


/**
 * @brief temperature correction with new method
 *
 * @param[in] ems Target emissivity(0-1)
 * @param[in] tau Atmospheric transmittance(0-16384)
 * @param[in] ta  Atmospheric temperature(unit: Celcius)
 * @param[in] org_temp origin temperature before correction(unit: Celcius)
 * @param[out] new_temp new temperature after correction(unit: Celcius)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t temp_correct(float ems, uint16_t tau, float ta, float org_temp, float* new_temp);


/**
 * @brief reverse calculate temperature correction with new method
 *
 * @param[in] ems Target emissivity(0-1)
 * @param[in] tau Atmospheric transmittance(0-16384)
 * @param[in] ta  Atmospheric temperature(unit: Celcius)
 * @param[in] new_temp new temperature after correction(unit: Celcius)
 * @param[out] org_temp origin temperature before correction(unit: Celcius)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t reverse_temp_correct(float ems, uint16_t tau, float ta, float new_temp, float* org_temp);



/**
 * @brief calculate nuc with nuc factor(P0,P1,P2)
 *
 * @param[in] nuc_factor  nuc factor contain(P0,P1,P2)
 * @param[in] temp  target temperature(unit: Celcius)
 * @param[out] nuc_value nuc_value
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t calculate_nuc_with_nuc_factor(NucFactor_t* nuc_factor, float temp, uint16_t* nuc_value);

/**
 * @brief generate NUC_T table in high gain mode with nuc factor(P0,P1,P2)
 *
 * @param[in] nuc_factor  nuc factor contain(P0,P1,P2)
 * @param[out] nuc_table  NUC_T table (uin16_t*8192)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t generate_high_gain_nuc_t(NucFactor_t* nuc_factor, uint16_t* nuc_table);


/**
 * @brief generate NUC_T table in low gain mode with nuc factor(P0,P1,P2)
 *
 * @param[in] nuc_factor  nuc factor contain(P0,P1,P2)
 * @param[out] nuc_table  NUC_T table (uin16_t*8192)
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t generate_low_gain_nuc_t(NucFactor_t* nuc_factor, uint16_t* nuc_table);


/**
 * @brief reverse calculate NUC value with nuc_t table
 *
 * @param[in] nuc_table  NUC_T table (uin16_t*8192)
 * @param[in] temp  celcius temperature
 * @param[out] nuc_value  nuc value after reverse calculate.
 *
 * @return see irtemp_error_t
 */
DLLEXPORT irtemp_error_t reverse_calc_NUC_with_nuc_t(uint16_t* nuc_table, float temp, uint16_t* nuc_value);



/**
* @brief calculate origin KE and BE with NUC-T table.
*
* @param[in] env_param The origin environment factor correction parameter(EMS,TAU,Ta,Tu)
* @param[in]  nuc_table  NUC_T table (uin16_t*8192)
* @param[in] gain_flag Gain flag for for environment factor validity check
* @param[out] env_factor Environment factor correction coefficient contains K_E and B_E
*
* @return see irtemp_error_t
*/
DLLEXPORT irtemp_error_t calculate_org_KE_and_BE_with_nuc_t(const EnvParam_t* env_param, const uint16_t* nuc_table, \
	uint8_t gain_flag, EnvFactor_t* env_factor);




/**
* @brief calculate new KE and BE with higher precision.
*
* @param[in] env_param The new environment factor correction parameter(EMS,TAU,Ta,Tu)
* @param[in]  nuc_table  NUC_T table (uin16_t*8192)
* @param[in] gain_flag Gain flag for for environment factor validity check
* @param[out] env_factor Environment factor correction coefficient contains K_E and B_E
*
* @return see irtemp_error_t
*/
DLLEXPORT irtemp_error_t calculate_new_KE_and_BE_with_nuc_t(const EnvParam_t* env_param, const uint16_t* nuc_table, \
	uint8_t gain_flag, EnvFactor_t* env_factor);



#ifdef __cplusplus
}
#endif

#endif
