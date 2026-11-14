#include "esphome/core/log.h"
#include "airco_tft_panel.h"

namespace esphome {
namespace airco_tft_panel {

static const char *TAG = "airco_tft_panel.component";

void AircoTftPanel::setup() {

		// Screen calibration data (UI calibration process does not work)
		uint16_t calData[5] = { 288, 3220, 357, 3548, 0};
		
		// Initialise the TFT screen
		tft.init();
		tft.setRotation(2);
		
		// initialise the touch component without using calibration procedure
		tft.setTouch(calData);
		
		// Subscribe to Home Assistant states
		subscribeHAStates();
		
		// start counting time for the screen saver trigger logic
		touchTime = millis();
		
		// draw initial panel before connection
		redraw_panel();

        ESP_LOGD("DEBUG","End setup");
}

void AircoTftPanel::loop() {

		uint16_t x, y;


		// check if we're still connected and update state record
		NetworkConnection = is_connected();
		if (!NetworkConnection) draw_connection_state();


		// check if a new touch action is detected
		if (!touchAction && tft.getTouch(&x, &y)) {
			touchAction = true;
			touchTime = millis();
			if (screenSaverActive&&!confirmScreen) {
				screenSaverActive = false;
				redraw_panel();
			} else if (HomeOccupation && NetworkConnection) {
				makeTouchAction(x,y);
			}
		}
		

		// If a touch action is ongoing, allow to release only after a delay and when touch is finished
		if (touchAction && !tft.getTouch(&x, &y)) {
			if (touchTime + touchDelay < millis()) {
				touchAction = false;
			}
		}
		// check for sleep trigger if no touch happened
		if ((touchTime + sleepDelay < millis()) and  (!screenSaverActive or (screenSaverActive && confirmScreen))) {
			screenSaverActive=true;
			confirmScreen = false;
			refreshSaver();
			touchTime = millis();
		}
		// cycle screen saver display if still on
		if (screenSaverActive && !confirmScreen && (touchTime + saverRefreshDelay < millis())) {
			refreshSaver();
			touchTime = millis();
		}
		
}

void AircoTftPanel::dump_config(){
    ESP_LOGCONFIG(TAG, "Airco TFT Panel");
}

//------------------------------------------------------------------------------------------
// Setup functions
//------------------------------------------------------------------------------------------

	void AircoTftPanel::subscribeHAStates() {
		// get state for wifi signal strength
		subscribe_homeassistant_state(&AircoTftPanel::change_connection_strength, "sensor.airco_tft_panel_wifi_signal_sensor");
		
		// get time from HA via template sensor
		subscribe_homeassistant_state(&AircoTftPanel::update_time_string, "sensor.current_time");
		
		// get state for home occupation
		subscribe_homeassistant_state(&AircoTftPanel::change_home_occupation, HomeOccupationEntity);

		// get states for Airco0 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco0, AircoData[0][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco0, AircoData[0][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco0, AircoData[0][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco0, AircoData[0][4]);

		// get states for Airco1 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco1, AircoData[1][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco1, AircoData[1][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco1, AircoData[1][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco1, AircoData[1][4]);

		// get states for Airco2 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco2, AircoData[2][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco2, AircoData[2][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco2, AircoData[2][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco2, AircoData[2][4]);

		// get states for Airco3 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco3, AircoData[3][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco3, AircoData[3][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco3, AircoData[3][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco3, AircoData[3][4]);

		// get states for Airco4 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco4, AircoData[4][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco4, AircoData[4][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco4, AircoData[4][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco4, AircoData[4][4]);

		// get states for Airco5 helpers
		subscribe_homeassistant_state(&AircoTftPanel::change_auto_mode_airco5, AircoData[5][1]);
		subscribe_homeassistant_state(&AircoTftPanel::change_run_mode_airco5, AircoData[5][2]);
		subscribe_homeassistant_state(&AircoTftPanel::change_request_median_temp_airco5, AircoData[5][3]);
	 	subscribe_homeassistant_state(&AircoTftPanel::change_request_temp_airco5, AircoData[5][4]);

	}

//------------------------------------------------------------------------------------------
// Dynamic draw functions
//------------------------------------------------------------------------------------------

	void AircoTftPanel::draw_connection_state() {
		uint32_t drawColor;

		if (!screenSaverActive) {
			if (NetworkConnection) tft.fillRect(285,10,25,25,TFT_DARKGREY);
			(NetworkConnection && connectionStrength > 3) ? tft.drawArc(288, 30, 18, 15, 180, 270, TFT_GREEN, TFT_BLACK, false) : tft.drawSmoothArc(288, 30, 18, 16, 180, 270, TFT_BLACK, TFT_BLACK, true);
			(NetworkConnection && connectionStrength > 2) ? tft.drawArc(288, 30, 13, 10, 180, 270, TFT_GREEN, TFT_BLACK, false) : tft.drawSmoothArc(288, 30, 13, 11, 180, 270, TFT_BLACK, TFT_BLACK, true);
			(NetworkConnection && connectionStrength > 1) ? tft.drawArc(288, 30, 8, 5, 180, 270, TFT_GREEN, TFT_BLACK, false) : tft.drawSmoothArc(288, 30, 8, 6, 180, 270, TFT_BLACK, TFT_BLACK, true);
			(NetworkConnection) ? tft.drawArc(288, 30, 3, 0, 180, 270, TFT_GREEN, TFT_BLACK, false) : tft.drawSmoothArc(288, 30, 3, 0, 180, 270, TFT_RED, TFT_BLACK, true);
		}

		ESP_LOGD("DEBUG","Network state: %s",NetworkConnection ? "true" : "false");
		ESP_LOGD("DEBUG","Connection strength: %i", connectionStrength);

	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::refreshSaver() {
		tft.fillScreen(TFT_BLACK);
		tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
		tft.setFreeFont(FONT_9PT);
		tft.setCursor(40,saver_offset_y - 25);
		if (NetworkConnection) {
			tft.printf("%s",current_HA_time.c_str());
		} else {
			tft.print("no network connection!");
		}
		tft.setCursor(40,saver_offset_y);
		tft.print("Air conditioning panel");
		tft.setCursor(40,saver_offset_y + 25);
		tft.print("-> Tap to activate <-");
		saver_offset_y += 30;
		if (saver_offset_y > 460) saver_offset_y = 50;
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::redraw_panel() {
		ESP_LOGD("DEBUG","Full redraw called");
		drawFixedPanel();
		draw_connection_state();
		for (uint8_t row = 0; row < 6; row++) {
			draw_airco_auto_mode(row);
			draw_airco_run_mode(row);
			draw_temp_buttons(row);
			draw_target_temp(row);
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::drawFixedPanel() {
		uint16_t start_draw_y = init_draw_y;

		if (!screenSaverActive) {
			// paint it black
			tft.fillScreen(TFT_BLACK);
			// Draw top area background
			tft.fillRect(0, 0, 320, 50, TFT_DARKGREY);
			tft.setCursor(30,30);
			tft.setTextColor(TFT_WHITE,TFT_DARKGREY, true);
			tft.setFreeFont(FONT_12PT_BOLD);
			tft.printf("Air Conditioning");
			// draw six "auto" buttons
			for (uint8_t row = 0; row < 6; row++) {
				// print airco title in small white letters
				tft.setCursor(0,start_draw_y+14);
				tft.setTextColor(TFT_WHITE,TFT_BLACK);
				tft.setFreeFont(FONT_9PT);
				tft.printf("%s",AircoData[row][0].c_str());
				// print AUTO button outline
				tft.drawRoundRect(5,start_draw_y+offset_y,82,incr_draw_y-35,4,TFT_WHITE);
				// draw line before next block
				tft.drawLine(0,start_draw_y+incr_draw_y-1,320,start_draw_y+incr_draw_y-1,TFT_DARKGREY);

				// iterate for next block
				start_draw_y = start_draw_y + incr_draw_y;
			}
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::draw_airco_auto_mode(uint8_t aircoNumber) {
		const uint16_t y_position = init_draw_y + aircoNumber * incr_draw_y;
		uint32_t drawColor;

		if (!screenSaverActive) {
			// Determine color for the button text
			if (!HomeOccupation || !NetworkConnection) {
				drawColor = TFT_BLACK;
			} else if (AircoAutoState[aircoNumber]) {
				drawColor=TFT_GREEN;
			} else {
				drawColor=TFT_DARKGREY;
			}
			tft.setCursor(17,y_position+50);
			tft.setTextColor(drawColor,TFT_BLACK,true);
			tft.setFreeFont(FONT_12PT);
			tft.print("AUTO");
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::draw_airco_run_mode(uint8_t aircoNumber) {
		const uint16_t y_position = init_draw_y + aircoNumber * incr_draw_y;
		uint32_t drawColor;

		if (!screenSaverActive) {
			// Determine color for the text
			// Possible states are: auto, cool, dry, fan_only, heat, off
			if (!NetworkConnection || !HomeOccupation || !AircoAutoState[aircoNumber] || (AircoRunMode[aircoNumber] == "----")) {
				drawColor = TFT_DARKGREY;
			} else if (AircoRunMode[aircoNumber] == "heat") {
				drawColor = TFT_RED;
			} else if (AircoRunMode[aircoNumber] == "cool") {
				drawColor = TFT_BLUE;
			} else {
				drawColor = TFT_WHITE;
			}
			// print background color to delete previous state
			tft.fillRect(104,y_position+offset_y-2,80,37,TFT_BLACK);
			// print state
			tft.setCursor(105,y_position+50);
			tft.setTextColor(drawColor,TFT_BLACK,true);
			tft.setFreeFont(FONT_12PT);
			if (NetworkConnection) {
				if (AircoRunMode[aircoNumber] == "fan_only") {
					tft.print("fan");
				} else {
					tft.printf("%s",AircoRunMode[aircoNumber].c_str());
				}
			} else {
				tft.print("----");
			}
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::draw_temp_buttons(uint8_t aircoNumber) {
		const uint16_t y_position = init_draw_y + aircoNumber * incr_draw_y;
		uint32_t drawColor;

		if (!screenSaverActive) {
			// Determine color for the text
			if (NetworkConnection && HomeOccupation && AircoAutoState[aircoNumber] && AircoRunMode[aircoNumber] != "----") {
				drawColor = TFT_WHITE;
			} else {
				drawColor = TFT_BLACK;
			}
			// print minus button
			tft.drawCircle(offset_x+btn_radius,y_position+offset_y+btn_radius,btn_radius,drawColor);
			tft.drawLine(offset_x+5,y_position+offset_y+btn_radius,offset_x+2*btn_radius-5,y_position+offset_y+btn_radius,drawColor);
			// print plus button
			tft.drawCircle(offset_x+95+btn_radius,y_position+offset_y+btn_radius,btn_radius,drawColor);
			tft.drawLine(offset_x+95+5,y_position+offset_y+btn_radius,offset_x+95+2*btn_radius-5,y_position+offset_y+btn_radius,drawColor);
			tft.drawLine(offset_x+95+btn_radius,y_position+offset_y+5,offset_x+95+btn_radius,y_position+offset_y+2*btn_radius-5,drawColor);
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::draw_target_temp(uint8_t aircoNumber) {
		const uint16_t y_position = init_draw_y + aircoNumber * incr_draw_y;
		uint32_t drawColor;

		if (!screenSaverActive) {
			tft.setCursor(offset_x+50,y_position+50);
			tft.setFreeFont(FONT_12PT);
			// Determine color for the text
			if (!NetworkConnection) {
				tft.fillRect(offset_x+48,y_position+offset_y,35,35,TFT_BLACK);
				tft.setTextColor(TFT_DARKGREY,TFT_BLACK,true);
				tft.print("--");
			} else if (!HomeOccupation || !AircoAutoState[aircoNumber] || AircoRunMode[aircoNumber] == "----") {
				tft.fillRect(offset_x+48,y_position+offset_y,35,35,TFT_BLACK);
			} else {
				tft.fillRect(offset_x+48,y_position+offset_y,35,35,TFT_DARKGREY);
				if (AircoTargetTemp[aircoNumber] == AircoMedianTemp[aircoNumber]) {
					tft.setTextColor(TFT_GREEN,TFT_BLACK,true);
				} else {
					tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
				}
				tft.printf("%i",AircoTargetTemp[aircoNumber]);
			}
		}
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::flashRedTemp(uint8_t aircoNumber) {
		const uint16_t y_position = init_draw_y + aircoNumber * incr_draw_y;

		tft.setCursor(offset_x+50,y_position+50);
		tft.setFreeFont(&FreeMono12pt7b);
		tft.setTextColor(TFT_RED,TFT_BLACK,true);
		tft.printf("%i",AircoTargetTemp[aircoNumber]);
		delay(200);
		tft.setCursor(offset_x+50,y_position+50);
		tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
		tft.printf("%i",AircoTargetTemp[aircoNumber]);
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::ConfirmAutoOff(uint8_t aircoNumber) {
		tft.fillRect(0,51,320,430,TFT_BLACK);
		tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
		tft.setFreeFont(FONT_9PT);
		tft.setCursor(80,100);
		tft.print("Device will be");
		tft.setCursor(80,120);
		tft.print("operated manually");
		tft.setCursor(80,140);
		tft.print("with its remote.");
		tft.setCursor(80,160);
		tft.print("Are you sure");
		tft.setCursor(80,180);
		tft.print("you want to");
		tft.setCursor(80,200);
		tft.print("disable auto");
		tft.setCursor(80,220);
		tft.print("mode ?");
		tft.drawRoundRect(80,260,50,50,3,TFT_RED);
		tft.setTextColor(TFT_RED,TFT_BLACK,true);
		tft.setCursor(85,300);
		tft.print("NO");
		tft.drawRoundRect(180,260,50,50,3,TFT_GREEN);
		tft.setTextColor(TFT_GREEN,TFT_BLACK,true);
		tft.setCursor(185,300);
		tft.print("YES");
		confirmScreen = true;
		confirmWhichAirco = aircoNumber;
		screenSaverActive = true;
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::InfoAutoOn() {
		tft.fillRect(0,51,320,430,TFT_BLACK);
		tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
		tft.setFreeFont(FONT_9PT);
		tft.setCursor(80,120);
		tft.print("Automatic mode");
		tft.setCursor(80,140);
		tft.print("activated. Device");
		tft.setCursor(80,160);
		tft.print("added to the");
		tft.setCursor(80,180);
		tft.print("starting queue");
		delay(4000);
		redraw_panel();
	}

//------------------------------------------------------------------------------------------
// State change trigger functions
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_connection_strength(std::string state) {
		connectionStrength = floor((std::stoi(state) + 25) / 25);
		draw_connection_state();
		ESP_LOGD("DEBUG","Connection strength refreshed");
	}

//------------------------------------------------------------------------------------------
	
	void AircoTftPanel::update_time_string(std::string state) {
		current_HA_time = state;
		ESP_LOGD("DEBUG","Clock refreshed");
	}
	
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_home_occupation(std::string state) {
		if (state == "on") {
			HomeOccupation = true;
		} else {
			HomeOccupation = false;
		}
		
		ESP_LOGD("DEBUG","Occupation: %s",HomeOccupation ? "true" : "false");

		redraw_panel();
	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco(uint8_t aircoNumber, std::string state) {
		// capture current state
		ESP_LOGD("DEBUG","State Auto mode airco %i: %s",aircoNumber,state.c_str());
		if (state == "on") {
			AircoAutoState[aircoNumber] = true;
		} else {
			AircoAutoState[aircoNumber] = false;
		}
		draw_airco_auto_mode(aircoNumber);
		draw_airco_run_mode(aircoNumber);
		draw_temp_buttons(aircoNumber);
		draw_target_temp(aircoNumber);

		ESP_LOGD("DEBUG","Auto mode airco %i: %s",aircoNumber,AircoAutoState[aircoNumber] ? "true" : "false");

	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_run_mode_airco(uint8_t aircoNumber, std::string state) {
		AircoRunMode[aircoNumber] = state;
		draw_airco_run_mode(aircoNumber);
		draw_temp_buttons(aircoNumber);
		draw_target_temp(aircoNumber);

		ESP_LOGD("DEBUG","Mode for airco %i: %s",aircoNumber,AircoRunMode[aircoNumber].c_str());

	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_request_temp_airco(uint8_t aircoNumber, std::string state) {
		AircoTargetTemp[aircoNumber] = std::stoi(state);
		draw_target_temp(aircoNumber);

		ESP_LOGD("DEBUG","Request temp for airco %i: %i",aircoNumber,AircoTargetTemp[aircoNumber]);

	}

//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_request_median_temp_airco(uint8_t aircoNumber,std::string state) {
		AircoMedianTemp[aircoNumber] = std::stoi(state);
		draw_target_temp(aircoNumber);

		ESP_LOGD("DEBUG","Median temp for airco %i: %i",aircoNumber,AircoMedianTemp[aircoNumber]);

	}


//------------------------------------------------------------------------------------------
// AIRCO0
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco0(std::string state) {
		change_auto_mode_airco(0,state);
	}
	void AircoTftPanel::change_run_mode_airco0(std::string state) {
		change_run_mode_airco(0,state);
	}
	void AircoTftPanel::change_request_temp_airco0(std::string state) {
		change_request_temp_airco(0,state);
	}
	void AircoTftPanel::change_request_median_temp_airco0(std::string state) {
		change_request_median_temp_airco(0,state);
	}

//------------------------------------------------------------------------------------------
// AIRCO1
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco1(std::string state) {
		change_auto_mode_airco(1,state);
	}
	void AircoTftPanel::change_run_mode_airco1(std::string state) {
		change_run_mode_airco(1,state);
	}
	void AircoTftPanel::change_request_temp_airco1(std::string state) {
		change_request_temp_airco(1,state);
	}
	void AircoTftPanel::change_request_median_temp_airco1(std::string state) {
		change_request_median_temp_airco(1,state);
	}

//------------------------------------------------------------------------------------------
// AIRCO2
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco2(std::string state) {
		change_auto_mode_airco(2,state);
	}
	void AircoTftPanel::change_run_mode_airco2(std::string state) {
		change_run_mode_airco(2,state);
	}
	void AircoTftPanel::change_request_temp_airco2(std::string state) {
		change_request_temp_airco(2,state);
	}
	void AircoTftPanel::change_request_median_temp_airco2(std::string state) {
		change_request_median_temp_airco(2,state);
	}

//------------------------------------------------------------------------------------------
// AIRCO3
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco3(std::string state) {
		change_auto_mode_airco(3,state);
	}
	void AircoTftPanel::change_run_mode_airco3(std::string state) {
		change_run_mode_airco(3,state);
	}
	void AircoTftPanel::change_request_temp_airco3(std::string state) {
		change_request_temp_airco(3,state);
	}
	void AircoTftPanel::change_request_median_temp_airco3(std::string state) {
		change_request_median_temp_airco(3,state);
	}

//------------------------------------------------------------------------------------------
// AIRCO4
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco4(std::string state) {
		change_auto_mode_airco(4,state);
	}
	void AircoTftPanel::change_run_mode_airco4(std::string state) {
		change_run_mode_airco(4,state);
	}
	void AircoTftPanel::change_request_temp_airco4(std::string state) {
		change_request_temp_airco(4,state);
	}
	void AircoTftPanel::change_request_median_temp_airco4(std::string state) {
		change_request_median_temp_airco(4,state);
	}

//------------------------------------------------------------------------------------------
// AIRCO5
//------------------------------------------------------------------------------------------

	void AircoTftPanel::change_auto_mode_airco5(std::string state) {
		change_auto_mode_airco(5,state);
	}
	void AircoTftPanel::change_run_mode_airco5(std::string state) {
		change_run_mode_airco(5,state);
	}
	void AircoTftPanel::change_request_temp_airco5(std::string state) {
		change_request_temp_airco(5,state);
	}
	void AircoTftPanel::change_request_median_temp_airco5(std::string state) {
		change_request_median_temp_airco(5,state);
	}

//------------------------------------------------------------------------------------------
// Touch control functions
//------------------------------------------------------------------------------------------

	void AircoTftPanel::makeTouchAction(uint16_t x, uint16_t y) {

		uint8_t targetAirco = 0;

		if (confirmScreen) {
			if (x > 50 and x < 150 and y > 230 and y < 330) {
				screenSaverActive = false;
				confirmScreen = false;
				redraw_panel();
			} else if (x > 150 and x < 280 and y > 230 and y < 330) {
				screenSaverActive = false;
				confirmScreen = false;
				call_homeassistant_service("input_boolean.turn_off", { {"entity_id", AircoData[confirmWhichAirco][1]} } );
				redraw_panel();
			}
		} else {
			if (y > init_draw_y) {
				targetAirco = floor((y-init_draw_y)/incr_draw_y);
				// touch in the column of auto buttons
				if (x < 85) {
					if (AircoAutoState[targetAirco]) {
						ConfirmAutoOff(targetAirco);
					} else {
						call_homeassistant_service("input_boolean.turn_on", { {"entity_id", AircoData[targetAirco][1]} } );
						InfoAutoOn();
					}
				}
				// touch in the column of the minus buttons
				if ((x > offset_x-5) and (x < offset_x+2*btn_radius+5)) {
					if (AircoAutoState[targetAirco]) {
						if (AircoTargetTemp[targetAirco] > AircoMedianTemp[targetAirco] - 2) {
							call_homeassistant_service("input_number.decrement", { {"entity_id", AircoData[targetAirco][4]} } );
						} else {
							flashRedTemp(targetAirco);
						}
					}
				}
				// touch in the column of the plus buttons
				if (x > offset_x+80) {
					if (AircoAutoState[targetAirco]) {
						if (AircoTargetTemp[targetAirco] < AircoMedianTemp[targetAirco] + 2) {
							call_homeassistant_service("input_number.increment", { {"entity_id", AircoData[targetAirco][4]} } );
						} else {
							flashRedTemp(targetAirco);
						}
					}
				}
			} else {
				// to be used for multi-pages display
				// ESP_LOGD("custom","touching the title bar");
			}
		}
	}

}  // namespace airco_tft_panel
}  // namespace esphome
