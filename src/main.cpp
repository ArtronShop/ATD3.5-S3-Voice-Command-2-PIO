#include <Arduino.h>
#include <lvgl.h>
#include <ATD3.5-S3.h>
#include "gui/ui.h"

#define RX_PIN (2) // กำหนดขาต่อ RX VC-02-Kit
#define TX_PIN (1) // กำหนดขาต่อ TX VC-02-Kit

#define LAMP_FRONT_PIN (42)
#define LAMP_MIDDLE_PIN (41)
#define LAMP_BACK_PIN (40)

void switch_button_update(lv_event_t * e) {
  if (lv_obj_has_state(ui_front, LV_STATE_CHECKED)) { // if switch on
    digitalWrite(LAMP_FRONT_PIN, HIGH); // ON LED
  } else {
    digitalWrite(LAMP_FRONT_PIN, LOW); // OFF LED
  }

  if (lv_obj_has_state(ui_middle, LV_STATE_CHECKED)) { // if switch on
    digitalWrite(LAMP_MIDDLE_PIN, HIGH); // ON LED
  } else {
    digitalWrite(LAMP_MIDDLE_PIN, LOW); // OFF LED
  }

  if (lv_obj_has_state(ui_back, LV_STATE_CHECKED)) { // if switch on
    digitalWrite(LAMP_BACK_PIN, HIGH); // ON LED
  } else {
    digitalWrite(LAMP_BACK_PIN, LOW); // OFF LED
  }
}

void setup() {
  Serial.begin(115200);
  
  // Setup peripherals
  Display.begin(0); // rotation number 0
  Touch.begin();
  Sound.begin();
  // Card.begin(); // uncomment if you want to Read/Write/Play/Load file in MicroSD Card
  pinMode(LAMP_FRONT_PIN, OUTPUT);
  pinMode(LAMP_MIDDLE_PIN, OUTPUT);
  pinMode(LAMP_BACK_PIN, OUTPUT);

  Serial.begin(115200); // ใช้งาน Serial Monitor
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // ใช้งาน Serial2
  Serial2.setTimeout(50); // กำหนดเวลา Timeout เป็น 50 mS
  
  // Map peripheral to LVGL
  Display.useLVGL(); // Map display to LVGL
  Touch.useLVGL(); // Map touch screen to LVGL
  Sound.useLVGL(); // Map speaker to LVGL
  // Card.useLVGL(); // Map MicroSD Card to LVGL File System

  Display.enableAutoSleep(120); // Auto off display if not touch in 2 min
  
  // Add load your UI function
  ui_init();

  // Add event handle
  lv_obj_add_event_cb(ui_front, switch_button_update, LV_EVENT_CLICKED, (void *) LAMP_FRONT_PIN);
  lv_obj_add_event_cb(ui_middle, switch_button_update, LV_EVENT_CLICKED, (void *) LAMP_MIDDLE_PIN);
  lv_obj_add_event_cb(ui_back, switch_button_update, LV_EVENT_CLICKED, (void *) LAMP_BACK_PIN);
}

void loop() {
  Display.loop(); // Keep GUI work

  if (Serial2.available()) { // ถ้ามีข้อมูลเข้ามาที่ Serial2
    String command_code = Serial2.readString(); // อ่านค่าจาก Serial2
    if (command_code == "\x5A\x00\x00\x00\x5A") { // Wake Up
      Serial.println("Wake Up");
      lv_obj_clear_flag(ui_voice_panel, LV_OBJ_FLAG_HIDDEN);
    } else if (command_code == "\x5A\x01\x00\x00\x5B") { // Standby
      Serial.println("Standby");
      lv_obj_add_flag(ui_voice_panel, LV_OBJ_FLAG_HIDDEN);
    } else if (command_code == "\x5A\x27\x00\x00\x81") { // all light turn on
      Serial.println("ALL LED ON");
      lv_obj_add_state(ui_front, LV_STATE_CHECKED);
      lv_obj_add_state(ui_middle, LV_STATE_CHECKED);
      lv_obj_add_state(ui_back, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x28\x00\x00\x82") { // all light turn off
      Serial.println("ALL LED OFF");
      lv_obj_clear_state(ui_front, LV_STATE_CHECKED);
      lv_obj_clear_state(ui_middle, LV_STATE_CHECKED);
      lv_obj_clear_state(ui_back, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x29\x00\x00\x83") { // turn on the light front
      Serial.println("LED FRONT ON");
      lv_obj_add_state(ui_front, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2A\x00\x00\x84") { // turn off the light front
      Serial.println("LED FRONT OFF");
      lv_obj_clear_state(ui_front, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2B\x00\x00\x85") { // turn on the light middle
      Serial.println("LED MIDDLE ON");
      lv_obj_add_state(ui_middle, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2C\x00\x00\x86") { // turn off the light middle
      Serial.println("LED MIDDLE OFF");
      lv_obj_clear_state(ui_middle, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2D\x00\x00\x87") { // turn on the light back
      Serial.println("LED BACK ON");
      lv_obj_add_state(ui_back, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2E\x00\x00\x88") { // turn off the light back
      Serial.println("LED BACK OFF");
      lv_obj_clear_state(ui_back, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x2F\x00\x00\x89") { // all light turn on
      Serial.println("LED ON");
      lv_obj_add_state(ui_front, LV_STATE_CHECKED);
      lv_obj_add_state(ui_middle, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else if (command_code == "\x5A\x30\x00\x00\x8A") { // all light turn off
      Serial.println("LED OFF");
      lv_obj_clear_state(ui_front, LV_STATE_CHECKED);
      lv_obj_clear_state(ui_middle, LV_STATE_CHECKED);
      switch_button_update(NULL);
    } else {
      Serial.print("Unknow command : ");
      for (uint8_t i=0;i<command_code.length();i++) {
        Serial.print("0x");
        Serial.print((int) command_code.charAt(i), HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
