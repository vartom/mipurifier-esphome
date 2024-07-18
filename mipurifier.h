
#include "esphome.h"

class MiPurifier : public Component, public UARTDevice, public CustomAPIDevice {
public:
  static const int max_line_length = 200;
  char recv_buffer[max_line_length];
  char send_buffer[max_line_length];
  int last_heartbeat, last_query;
  
  Sensor *airquality_sensor = new Sensor();
  Sensor *carbon_dioxide_sensor = new Sensor();
  Sensor *temperature_sensor = new Sensor();
  Sensor *filter_intermediate_sensor = new Sensor();
  Sensor *filter_efficient_sensor = new Sensor();
  Sensor *filter_inter_day_sensor = new Sensor();
  Sensor *filter_effi_day_sensor = new Sensor();
  Sensor *control_speed_sensor = new Sensor();

  MiPurifier(UARTComponent *uart) : UARTDevice(uart) {}

  int readline(int readch, char *buffer, int len) {
    static int pos = 0;
    int rpos;
    
    if (readch > 0) {
      switch (readch) {
        case '\r': // Return on CR
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  // only run setup() after a Wi-Fi connection has been established successfully
  float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; } 

  void turn_on() {
    strcpy(send_buffer, "down set_power true\r");
  }

  void turn_off() {
    strcpy(send_buffer, "down set_power false\r");
  }

  void enable_beeper() {
    strcpy(send_buffer, "down set_sound true\r");
  }

  void disable_beeper() {
    strcpy(send_buffer, "down set_sound false\r");
  }

  void display_on() {
    strcpy(send_buffer, "down set_display true\r");
  }

  void display_off() {
    strcpy(send_buffer, "down set_display false\r");
  }

  void ptc_on() {
    strcpy(send_buffer, "down set_ptc_on true\r");
  }

  void ptc_off() {
    strcpy(send_buffer, "down set_ptc_on false\r");
  }

  void child_lock_on() {
    strcpy(send_buffer, "down set_child_lock true\r");
  }

  void child_lock_off() {
    strcpy(send_buffer, "down set_child_lock false\r");
  }

  void reset_upper_filter() {
    strcpy(send_buffer, "down set_filter_reset \"efficient\"\r");
  }

  void reset_dust_filter() {
    strcpy(send_buffer, "down set_filter_reset \"intermediate\"\r");
  }

  void set_mode(std::string mode) {
    // 0: auto, 1: sleep, 2: favorite
    if (mode == "auto") {
      strcpy(send_buffer, "down set_mode \"auto\"\r");
    } else if (mode == "sleep") {
      strcpy(send_buffer, "down set_mode \"sleep\"\r");
    } else if (mode == "favourite") {
      strcpy(send_buffer, "down set_mode \"favourite\"\r");
    }
  }

  void set_ptc_level(std::string ptc_level) {
    if (ptc_level == "low") {
      strcpy(send_buffer, "down set_ptc_level \"low\"\r");
    } else if (ptc_level == "medium") {
      strcpy(send_buffer, "down set_ptc_level \"medium\"\r");
    } else if (ptc_level == "high") {
      strcpy(send_buffer, "down set_ptc_level \"high\"\r");
    }
  }

  void set_screen_direction(std::string screen_direction) {
    if (screen_direction == "forward") {
      strcpy(send_buffer, "down set_screen_direction \"forward\"\r");
    } else if (screen_direction == "left") {
      strcpy(send_buffer, "down set_screen_direction \"left\"\r");
    } else if (screen_direction == "right") {
      strcpy(send_buffer, "down set_screen_direction \"right\"\r");
    }
  }

  void set_favouritespeed(int speed) {
    snprintf(send_buffer, max_line_length, "down set_favourite_speed %i\r", speed);
  }

  void send_command(std::string s) {
    strcpy(send_buffer, s.c_str());
  }

  void update_property(char* id, char* val) {
    if (strcmp(id, "pm25") == 0) {
      airquality_sensor->publish_state(atof(val));
    } else if (strcmp(id, "co2") == 0) {
      carbon_dioxide_sensor->publish_state(atof(val));
    } else if (strcmp(id, "temperature_outside") == 0) {
      temperature_sensor->publish_state(atof(val));
    } else if (strcmp(id, "filter_intermediate") == 0) {
      filter_intermediate_sensor->publish_state(atof(val));
    } else if (strcmp(id, "filter_efficient") == 0) {
      filter_efficient_sensor->publish_state(atof(val));
    } else if (strcmp(id, "filter_inter_day") == 0) {
      filter_inter_day_sensor->publish_state(atof(val));
    } else if (strcmp(id, "filter_effi_day") == 0) {
      filter_effi_day_sensor->publish_state(atof(val));
    } else if (strcmp(id, "control_speed") == 0) {
      control_speed_sensor->publish_state(atof(val));
    } else if (strcmp(id, "power") == 0) {
      // power (on, off)
      power_switch->publish_state(strcmp(val, "true") == 0);
    } else if (strcmp(id, "mode") == 0) {
         if (strcmp(val, "auto") == 0) {
             mode_select->publish_state("auto");
            }
         else if (strcmp(val, "sleep") == 0) {
             mode_select->publish_state("sleep");
            }
         else if (strcmp(val, "favourite") == 0) {
             mode_select->publish_state("favourite");
            }
    } else if (strcmp(id, "ptc_level") == 0) {
         if (strcmp(val, "low") == 0) {
             ptc_level_select->publish_state("low");
            }
         else if (strcmp(val, "medium") == 0) {
             ptc_level_select->publish_state("medium");
            }
         else if (strcmp(val, "high") == 0) {
             ptc_level_select->publish_state("high");
            }
    } else if (strcmp(id, "screen_direction") == 0) {
         if (strcmp(val, "forward") == 0) {
             screen_direction_select->publish_state("forward");
            }
         else if (strcmp(val, "left") == 0) {
             screen_direction_select->publish_state("left");
            }
         else if (strcmp(val, "right") == 0) {
             screen_direction_select->publish_state("right");
            }
    } else if (strcmp(id, "sound") == 0) {
      // beeper (on, off)
      beeper_switch->publish_state(strcmp(val, "true") == 0);
    } else if (strcmp(id, "ptc_on") == 0) {
      // ptc (on, off)
      ptc_switch->publish_state(strcmp(val, "true") == 0);
    } else if (strcmp(id, "display") == 0) {
      // display (on, off)
      display_switch->publish_state(strcmp(val, "true") == 0);
    } else if (strcmp(id, "child_lock") == 0) {
      // child_lock (on, off)
      child_lock_switch->publish_state(strcmp(val, "true") == 0);
    } else if (strcmp(id, "favourite_speed") == 0) {
      // favourite speed
      favouritespeed->publish_state(atof(val));
    }
  }

  void update_property2(int id, char* val) {
      switch (id) {
        case 1:
      // power (on, off) fix some other result answer( rtc level result == temperature )
         if (strcmp(val, "true") == 0) {
             power_switch->publish_state(true);
            }
         else if (strcmp(val, "false") == 0) {
             power_switch->publish_state(false);
            }
          break;
        case 2:
         if (strcmp(val, "auto") == 0) {
             mode_select->publish_state("auto");
            }
         else if (strcmp(val, "sleep") == 0) {
             mode_select->publish_state("sleep");
            }
         else if (strcmp(val, "favourite") == 0) {
             mode_select->publish_state("favourite");
            }
         break;
        case 3:
          airquality_sensor->publish_state(atof(val));
            break;
        case 4:
          carbon_dioxide_sensor->publish_state(atof(val));
            break;
        case 5:
          temperature_sensor->publish_state(atof(val));
            break;
        case 6:
      // favourite speed
          favouritespeed->publish_state(atof(val));
            break;
        case 7:
      // ptc (on, off)
            ptc_switch->publish_state(strcmp(val, "true") == 0);
            break;
        case 8:
         if (strcmp(val, "low") == 0) {
             ptc_level_select->publish_state("low");
            }
         else if (strcmp(val, "medium") == 0) {
             ptc_level_select->publish_state("medium");
            }
         else if (strcmp(val, "high") == 0) {
             ptc_level_select->publish_state("high");
            }
            break;
        case 9:
      // display (on, off)
            display_switch->publish_state(strcmp(val, "true") == 0);
            break;
        case 10:
      // sound (on, off)
            beeper_switch->publish_state(strcmp(val, "true") == 0);
            break;
        case 11:
      // child_lock (on, off)
            child_lock_switch->publish_state(strcmp(val, "true") == 0);
            break;
        case 12:
          control_speed_sensor->publish_state(atof(val));
          break;
    }
  }

  void setup() override {
    register_service(&MiPurifier::send_command, "send_command", {"command"});
    // get initial state & settings
    strcpy(send_buffer, "down get_prop power, mode, pm25, co2, temperature_outside, favourite_speed, control_speed, ptc_on, ptc_level, display, sound, child_lock, control_speed\r");
  }
  
  void loop() override {
    while (available()) {
      if(readline(read(), recv_buffer, max_line_length) > 0) {
        char *cmd = strtok(recv_buffer, " ");
        if (strcmp(cmd, "net") == 0) {
            write_str("cloud\r");
        } else if (strcmp(cmd, "time") == 0) {
          write_str("ok\r");
        } else if (strcmp(cmd, "get_down") == 0) {
          // send command from send_buffer
          if (strlen(send_buffer) > 0) {
            write_str(send_buffer);
            send_buffer[0] = '\0';
            ESP_LOGD("mipurifier", "sent send_buffer");
          } else if (millis() - last_heartbeat > 30000) {
            // send mysterious heartbeat message
            write_str("down none\r");
         //   write_str("down get_prop power, mode, pm25, co2, temperature_outside, favourite_speed, ptc_on, ptc_level, display, sound, child_lock, control_speed\r");
            last_heartbeat = millis();
            ESP_LOGD("mipurifier", "sent heartbeat");
          } else if (millis() - last_query > 15000) {
            // force sensor update
            write_str("down get_prop power, mode, pm25, co2, temperature_outside, favourite_speed, ptc_on, ptc_level, display, sound, child_lock, control_speed\r");
            //    id                   1     2      3    4            5                6              7       8          9       10       11           12           
            last_query = millis();
            ESP_LOGD("mipurifier", "sent query string");
          } else {
            write_str("down none\r");
          }
        } else if (strcmp(cmd, "props") == 0) {
          ESP_LOGD("mipurifier", "parsing properties_changed message");
          char *id, *val;
          while (true) {
            if (!(id = strtok(NULL, " "))) break;
            if (!(val = strtok(NULL, " \""))) break;
            update_property(id, val);
          }
          write_str("ok\r");
        } else if (strcmp(cmd, "result") == 0) {         
          // loop over all properties and update
          ESP_LOGD("mipurifier", "parsing result message");
          char *val;
          int id = 1;
          while (true) {
            if (!(val = strtok(NULL, " ,\""))) break;
        //    ESP_LOGD("mipurifier","Str id \"%i\" into:\n",id);
        //    ESP_LOGD("mipurifier","Str val \"%s\" into:\n",val);
            if (strcmp(val, "ok") == 0) {
              write_str("ok\r");
              break;
            }
            update_property2(id, val);
            id = id + 1;
          }
          write_str("ok\r");
        } else {
          // just acknowledge any other message
          write_str("ok\r");
        }
      }
    }
  }
};
