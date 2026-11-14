#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"
#include <TFT_eSPI.h>

#define FONT_9PT &FreeMono9pt7b
#define FONT_12PT &FreeMono12pt7b
#define FONT_12PT_BOLD &FreeMonoBold12pt7b

namespace esphome {
namespace airco_tft_panel {

class AircoTftPanel : public Component, public api::CustomAPIDevice {
 public:

	// -----------------------------
	// Definition of constant values
	// -----------------------------
 
	// define Home Assistant entity that speficy if house is occupied or not
	const std::string HomeOccupationEntity = "input_boolean.home_occupation";
	
    // define display names and related Home Assistant entities
	// data structure is { display name , 
	//					   input_boolean helper entity for auto mode,
	//					   any entity that can send the current airco state
	//					   input_number helper entity for storing median temp
	//					   input_number helper entity for storing target temp }
	//
	const std::string AircoData[6][5] = {   {"Upper living room", 
											 "input_boolean.ac_auto_salon_haut", 
											 "climate.ac_6_salon_haut",
											 "input_number.ac_median_salon_haut",
											 "input_number.ac_target_salon_haut"},
											{"Upper front bedroom", 
											 "input_boolean.ac_auto_ch_haut_avant", 
											 "climate.ac_3_chambre_haut_avant",
											 "input_number.ac_median_ch_haut_avant",
											 "input_number.ac_target_ch_haut_avant"},
											{"Upper back bedroom", 
											 "input_boolean.ac_auto_ch_haut_fond", 
											 "climate.ac_1_chambre_haut_fond",
											 "input_number.ac_median_ch_haut_fond",
											 "input_number.ac_target_ch_haut_fond"},
											{"Lower living room", 
											 "input_boolean.ac_auto_salon_bas", 
											 "climate.ac_5_salon_bas",
											 "input_number.ac_median_salon_bas",
											 "input_number.ac_target_salon_bas"},
											{"Lower front bedroom", 
											 "input_boolean.ac_auto_ch_bas_avant", 
											 "climate.ac_4_chambre_bas_avant",
											 "input_number.ac_median_ch_bas_avant",
											 "input_number.ac_target_ch_bas_avant"},
											{"Lower back bedroom", 
											 "input_boolean.ac_auto_ch_bas_fond", 
											 "climate.ac_2_chambre_bas_fond",
											 "input_number.ac_median_ch_bas_fond",
											 "input_number.ac_target_ch_bas_fond"}
										};
	
	// fixed values for screen saver
	const uint32_t sleepDelay = 60000;
	const uint16_t saverRefreshDelay = 5000;

	// fixed values to draw each raw of control buttons and info display 
	const uint16_t init_draw_y = 60;
	const uint8_t incr_draw_y = 70;
	const uint8_t offset_x = 180;
	const uint8_t offset_y = 25;
	const uint8_t btn_radius = 17;

	// -----------------------------
	// Definition of variable values
	// -----------------------------
 
    // hook to the TFT display component
	TFT_eSPI tft = TFT_eSPI();

	// record network connection state
	bool NetworkConnection = false;
	uint8_t connectionStrength = 0;
	
	// record HA time for display in screen saver
	std::string current_HA_time = "no network connection!";
	
	// record home occupation state
	bool HomeOccupation = false;

	// record individual airco states
	bool AircoAutoState[6] = { false, false, false, false, false, false};
	uint8_t AircoTargetTemp[6] = {0,0,0,0,0,0};
	uint8_t AircoMedianTemp[6] = {0,0,0,0,0,0};
	std::string AircoRunMode[6] = { "----", "----", "----", "----", "----", "----" };

	// A set of variables to debounce the UI on screen touch
	bool touchAction = false;
	float touchTime = 0;
	const uint16_t touchDelay = 300;
	
	// A set of variables to manage confirm screen without waiting loop
	bool confirmScreen = false;
	uint8_t confirmWhichAirco = 0;

	// Screen saver state
	bool screenSaverActive = false;
	uint16_t saver_offset_y = 50;

	// ------------------------
	// Main component functions
	// ------------------------
 	
    void setup() override;
    void loop() override;
    void dump_config() override;
	
	//------------------------------------------------------------------------------------------
	// Setup functions
	//------------------------------------------------------------------------------------------

	void subscribeHAStates();

	//------------------------------------------------------------------------------------------
	// Dynamic draw functions
	//------------------------------------------------------------------------------------------

	void draw_connection_state();
	void refreshSaver();
	void redraw_panel();
	void drawFixedPanel();
	void draw_airco_auto_mode(uint8_t aircoNumber);
	void draw_airco_run_mode(uint8_t aircoNumber);
	void draw_temp_buttons(uint8_t aircoNumber);
	void draw_target_temp(uint8_t aircoNumber);
	void flashRedTemp(uint8_t aircoNumber);
	void ConfirmAutoOff(uint8_t aircoNumber);
	void InfoAutoOn();

	//------------------------------------------------------------------------------------------
	// State change trigger functions
	//------------------------------------------------------------------------------------------

	void change_connection_strength(std::string state);
	void update_time_string(std::string state);
	void change_home_occupation(std::string state);
	void change_auto_mode_airco(uint8_t aircoNumber, std::string state);
	void change_run_mode_airco(uint8_t aircoNumber, std::string state);
	void change_request_temp_airco(uint8_t aircoNumber, std::string state);
	void change_request_median_temp_airco(uint8_t aircoNumber,std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO0
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco0(std::string state);
	void change_run_mode_airco0(std::string state);
	void change_request_temp_airco0(std::string state);
	void change_request_median_temp_airco0(std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO1
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco1(std::string state);
	void change_run_mode_airco1(std::string state);
	void change_request_temp_airco1(std::string state);
	void change_request_median_temp_airco1(std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO2
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco2(std::string state);
	void change_run_mode_airco2(std::string state);
	void change_request_temp_airco2(std::string state);
	void change_request_median_temp_airco2(std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO3
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco3(std::string state);
	void change_run_mode_airco3(std::string state);
	void change_request_temp_airco3(std::string state);
	void change_request_median_temp_airco3(std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO4
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco4(std::string state);
	void change_run_mode_airco4(std::string state);
	void change_request_temp_airco4(std::string state);
	void change_request_median_temp_airco4(std::string state);

	//------------------------------------------------------------------------------------------
	// AIRCO5
	//------------------------------------------------------------------------------------------

	void change_auto_mode_airco5(std::string state);
	void change_run_mode_airco5(std::string state);
	void change_request_temp_airco5(std::string state);
	void change_request_median_temp_airco5(std::string state);

	//------------------------------------------------------------------------------------------
	// Touch control functions
	//------------------------------------------------------------------------------------------

	void makeTouchAction(uint16_t x, uint16_t y);

};


}  // namespace airco_tft_panel
}  // namespace esphome