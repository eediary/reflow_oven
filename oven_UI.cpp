#include "oven_UI.h"
//#define DEBUG

static state UI_state = SELECT;
static byte SELECT_var = 0;
static byte MANUAL_var = 0;
static byte PROFILE_var = 0;
static float set_temp = 20, temp_start = 0, temp_end = 20;
static unsigned long t_start = 0, t_end = 1;
static int hysteresis = 0;
static bool relay_enable = false;

#ifdef DEBUG
static unsigned long last_time = 0;
static const unsigned long debug_period = 100;
#endif

// uses byte buttons (LEFT, MIDDLE, RIGHT, FLAG) to choose next state
// drives what str1 and str2 should be based on state + data
// returns whether or not relay should be on, based on thermocouple readings
bool UI_state_machine(PMODTC1_data data, char *str1, char *str2, byte buttons){
  bool to_return = false;
  char str_temp[4];
  
  // check error flags
  if(data.error_flag > 0){
    // Error has occured; check it
    strcpy(str1, "TC MOD ERROR");
    strcpy(str2, "");
    if(data.error_flag & SCV_MASK){
      strcat(str2, "SCV ");
    }
    if(data.error_flag & SCG_MASK){
      strcat(str2, "SCG ");
    }
    if(data.error_flag & OC_MASK){
      strcat(str2, "OC");
    }
    UI_state = SELECT;
    relay_enable = false;
    to_return = false;
  } else{
    // state machine
    switch(UI_state){
      
      // SELECT state
      case SELECT:
        switch(SELECT_var){
          case 0:
            strcpy(str1, "PROFILE 1");
            break;
          case 1:
            strcpy(str1, "PROFILE 2");
            break;
          case 2:
            strcpy(str1, "MANUAL");
            break;
        }
        strcpy(str2, nav_string);

        // check buttons
        if((buttons & F_MASK) > 0){
          // clear flag
          buttons &= ~F_MASK;
          if((buttons & L_MASK) == 0){
            SELECT_var = max(0, SELECT_var-1);
          } else if((buttons & R_MASK) == 0){
            SELECT_var = min(2, SELECT_var+1);
          } else if((buttons & M_MASK) == 0){
            // Hit enter, so change state to PROFILE or MANUAL
            if(SELECT_var == 0 || SELECT_var == 1){
              UI_state = PROFILE;
              reset_timer();
            } else if(SELECT_var == 2){
              UI_state = MANUAL;
            }
          }
        }
        to_return = false;
        break;

      // PROFILE state
      case PROFILE:
        // feed interpolator profiles to get set temp
        set_temp = m_lin_interp(ALENGTH, &time_array[SELECT_var][0], &temp_array[SELECT_var][0]);

        // turns relay on or off w/ hysteresis
        if(data.oven_temp < (set_temp + (float)hysteresis)){
          to_return = true;
          hysteresis = 0;
        } else {
          hysteresis = HYST_VAL;
          to_return = false;
        }
        
        // only enable relay if within profile
        if(get_time() > time_array[ADEPTH-1][ALENGTH-1]){
          relay_enable = false;
        } else{
          relay_enable = true;
        }

        // update LCD text
        // shows run time, end time and current temp
        if(relay_enable){
          strcpy(str1, "PROF ");
          sprintf(str_temp, "%1d", (int) SELECT_var + 1);
          strcat(str1, str_temp);
          strcat(str1, ": ");
          sprintf(str_temp, "%3d", (int)(get_time()/1000));
          strcat(str1, str_temp);
          strcat(str1, "/");
          sprintf(str_temp, "%3d", (int)(time_array[SELECT_var][ALENGTH-1]/1000));
          strcat(str1, str_temp);
        } else{
          strcpy(str1, "Done!");
        }
        strcpy(str2, "TMP: ");
        sprintf(str_temp, "%3d", (int)data.oven_temp);
        strcat(str2, str_temp);

        // check buttons
        if((buttons & F_MASK) > 0){
          // clear flag
          buttons &= ~F_MASK;
          if((buttons & M_MASK) == 0){
            // Hit back, so crevert to SELECT
            UI_state = SELECT;
            relay_enable = false;
          }
        }
        break;

      // Manual state
      case MANUAL:
        // feed interpolator current temp and target temp
        set_temp = lin_interp(t_start, t_end, temp_start, temp_end);

        // turns relay on or off w/ hysteresis
        if(data.oven_temp < (set_temp + (float)hysteresis)){
          to_return = true;
          hysteresis = 0;
        } else {
          hysteresis = HYST_VAL;
          to_return = false;
        }

        // lets user go back to SELECT or change target temp
        switch(MANUAL_var){
          case 0:
            strcpy(str1, "BACK");
            strcpy(str2, nav_string);
            break;
          default:
            strcpy(str1, "TMP: ");
            sprintf(str_temp, "%3d", (int)data.oven_temp);
            strcat(str1, str_temp);
            strcat(str1," -> ");
            sprintf(str_temp, "%3d", (int)temp_end);
            strcat(str1, str_temp);
            strcpy(str2, "ADJ: ");
            sprintf(str_temp, "%3d", (MANUAL_var+1) * 10);
            strcat(str2, str_temp);
            break;
        }
        
        // check buttons
        if((buttons & F_MASK) > 0){
          // clear flag
          buttons &= ~F_MASK;
          if((buttons & L_MASK) == 0){
            MANUAL_var = max(0, MANUAL_var-1);
          } else if((buttons & R_MASK) == 0){
            MANUAL_var = min(29, MANUAL_var+1);
          } else if((buttons & M_MASK) == 0){
            // Hit enter, so change state
            if(MANUAL_var == 0){
              // user wants to go back to SELECT state
              UI_state = SELECT;
              relay_enable = false;
              to_return = false;
              reset_timer();
            } else{
              // user has changed target temp
              // update times and temps for interpolation
              temp_start = data.oven_temp;
              temp_end = (float) (MANUAL_var+1) * 10;
              t_start = get_time();
              t_end = t_start + abs(temp_end - temp_start) * MANUAL_SLOPE;
              relay_enable = true;
            }
          }
        }
        break;
    }
  }
  #ifdef DEBUG
  // only shows debug info when relay is enabled since not much happens without the relay
  if(relay_enable){
    if(get_time() > (last_time + debug_period)){
      Serial.print(
        String(get_time()) + ", " +
        String(data.oven_temp) + ", " +
        String(set_temp) + ", " +
        String(data.ref_temp) + ", " +
        String(to_return) + "\n"
      ); 
      last_time = get_time();
    }
  }
  #endif
  
  to_return = relay_enable && to_return;
  return to_return;
}
