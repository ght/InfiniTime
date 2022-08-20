#include "components/motor/MotorController.h"
#include "displayapp/screens/settings/SettingChimes.h"
#include <lvgl/lvgl.h>
#include "displayapp/DisplayApp.h"
#include "displayapp/InfiniTimeTheme.h"
#include "displayapp/screens/Styles.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

namespace {
  void handleFrequencyDecrease(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      auto* screen = static_cast<SettingChimes*>(obj->user_data);
      screen->FrequencyDecrease();
    }
  }
  void handleFrequencyIncrease(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      auto* screen = static_cast<SettingChimes*>(obj->user_data);
      screen->FrequencyIncrease();
    }
  }
  void handleDurationDecrease(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      auto* screen = static_cast<SettingChimes*>(obj->user_data);
      screen->DurationDecrease();
    }
  }
  void handleDurationIncrease(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      auto* screen = static_cast<SettingChimes*>(obj->user_data);
      screen->DurationIncrease();
    }
  }
}

constexpr std::array<SettingChimes::Option, 8> SettingChimes::frequencyOptions;
constexpr std::array<uint8_t, 7> SettingChimes::durationOptions;

SettingChimes::SettingChimes(Pinetime::Applications::DisplayApp* app,
                             Pinetime::Controllers::MotorController& motorController,
                             Pinetime::Controllers::Settings& settingsController)
  : Screen(app),
    motorController {motorController},
    settingsController {settingsController} {

  // Find current chime frequency, rounding up if no match
  uint8_t currentFrequency = settingsController.GetChimesFrequency();
  for (size_t i = 0; i < frequencyOptions.size(); i++) {
    if (frequencyOptions[i].value <= currentFrequency) {
      frequencySelectedIndex = i;
      break;
    }
  }

  // Find current chime duration, rounding up if no match
  uint8_t currentDuration = settingsController.GetChimesDuration();
  for (size_t i = 0; i < durationOptions.size(); i++) {
    if (durationOptions[i] >= currentDuration) {
      durationSelectedIndex = i;
      break;
    }
  }

  // Styling
  static constexpr uint8_t uiContainerMargin = 10;
  static constexpr uint8_t uiContainerTop = 50;
  static const uint8_t uiContainerWidth = LV_HOR_RES - 2 * uiContainerMargin;
  static const uint8_t uiContainerHeight = LV_VER_RES - uiContainerTop - uiContainerMargin;
  static constexpr uint8_t uiButtonWidth = 50;
  static constexpr uint8_t uiButtonHeight = 80;

  // Styles
  lv_style_t containerStyle;
  lv_style_init(&containerStyle);
  lv_style_set_text_color(&containerStyle, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_bg_opa(&containerStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_style_set_margin_all(&containerStyle, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_all(&containerStyle, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_inner(&containerStyle, LV_STATE_DEFAULT, 0);
  lv_style_set_border_width(&containerStyle, LV_STATE_DEFAULT, 0);

  // Container
  wrapper = lv_cont_create(lv_scr_act(), nullptr);
  lv_obj_add_style(wrapper, LV_CONT_PART_MAIN, &containerStyle);
  lv_obj_set_style_local_pad_inner(wrapper, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_pos(wrapper, uiContainerMargin, uiContainerTop);
  lv_obj_set_size(wrapper, uiContainerWidth, uiContainerHeight);
  lv_cont_set_layout(wrapper, LV_LAYOUT_COLUMN_MID);

  // Title
  title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Chimes");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);

  // Icon
  icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, Symbols::clock);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  // Frequency input
  frequencyContainer = lv_cont_create(wrapper, nullptr);
  lv_obj_set_size(frequencyContainer, uiContainerWidth, uiButtonHeight);
  lv_obj_add_style(frequencyContainer, LV_CONT_PART_MAIN, &containerStyle);

  frequencyDecreaseButton = lv_btn_create(frequencyContainer, nullptr);
  frequencyDecreaseButton->user_data = this;
  lv_obj_set_size(frequencyDecreaseButton, uiButtonWidth, uiButtonHeight);
  lv_obj_align(frequencyDecreaseButton, nullptr, LV_ALIGN_IN_LEFT_MID, 0, 0);
  lv_obj_set_event_cb(frequencyDecreaseButton, handleFrequencyDecrease);
  frequencyDecreaseLabel = lv_label_create(frequencyDecreaseButton, nullptr);
  lv_obj_align(frequencyDecreaseLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text_static(frequencyDecreaseLabel, "-");

  frequencyIncreaseButton = lv_btn_create(frequencyContainer, nullptr);
  frequencyIncreaseButton->user_data = this;
  lv_obj_set_size(frequencyIncreaseButton, uiButtonWidth, uiButtonHeight);
  lv_obj_align(frequencyIncreaseButton, nullptr, LV_ALIGN_IN_RIGHT_MID, 0, 0);
  lv_obj_set_event_cb(frequencyIncreaseButton, handleFrequencyIncrease);
  frequencyIncreaseLabel = lv_label_create(frequencyIncreaseButton, nullptr);
  lv_obj_align(frequencyIncreaseLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text_static(frequencyIncreaseLabel, "+");

  frequencyLabel = lv_label_create(frequencyContainer, nullptr);
  FrequencyUpdate();
  lv_obj_align(frequencyLabel, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, uiButtonWidth + 6, 0);

  // Duration input
  durationContainer = lv_cont_create(wrapper, nullptr);
  lv_obj_set_size(durationContainer, uiContainerWidth, uiButtonHeight);
  lv_obj_add_style(durationContainer, LV_CONT_PART_MAIN, &containerStyle);

  durationDecreaseButton = lv_btn_create(durationContainer, nullptr);
  durationDecreaseButton->user_data = this;
  lv_obj_set_size(durationDecreaseButton, uiButtonWidth, uiButtonHeight);
  lv_obj_align(durationDecreaseButton, nullptr, LV_ALIGN_IN_LEFT_MID, 0, 0);
  lv_obj_set_event_cb(durationDecreaseButton, handleDurationDecrease);
  durationDecreaseLabel = lv_label_create(durationDecreaseButton, nullptr);
  lv_obj_align(durationDecreaseLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text_static(durationDecreaseLabel, "-");

  durationIncreaseButton = lv_btn_create(durationContainer, nullptr);
  durationIncreaseButton->user_data = this;
  lv_obj_set_size(durationIncreaseButton, uiButtonWidth, uiButtonHeight);
  lv_obj_align(durationIncreaseButton, nullptr, LV_ALIGN_IN_RIGHT_MID, 0, 0);
  lv_obj_set_event_cb(durationIncreaseButton, handleDurationIncrease);
  durationIncreaseLabel = lv_label_create(durationIncreaseButton, nullptr);
  lv_obj_align(durationIncreaseLabel, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text_static(durationIncreaseLabel, "+");

  durationLabel = lv_label_create(durationContainer, nullptr);
  DurationUpdate();
  lv_obj_align(durationLabel, nullptr, LV_ALIGN_IN_TOP_LEFT, uiButtonWidth + 6, 0);

  finishedLoading = true;
}

SettingChimes::~SettingChimes() {
  lv_obj_clean(lv_scr_act());
  settingsController.SaveSettings();
}

void SettingChimes::FrequencyUpdate() {
  bool canDecrease = frequencySelectedIndex > 0;
  lv_obj_set_state(frequencyDecreaseButton, canDecrease ? LV_STATE_DEFAULT : LV_STATE_DISABLED);
  bool canIncrease = frequencySelectedIndex < frequencyOptions.size() - 1;
  lv_obj_set_state(frequencyIncreaseButton, canIncrease ? LV_STATE_DEFAULT : LV_STATE_DISABLED);
  lv_label_set_text_fmt(frequencyLabel, "Every\n%s", GetFrequencySelection().label);

  if (finishedLoading) {
    settingsController.SetChimesFrequency(GetFrequencySelection().value);
  }
}

void SettingChimes::FrequencyDecrease() {
  if (frequencySelectedIndex > 0) {
    frequencySelectedIndex--;
    FrequencyUpdate();
  }
}

void SettingChimes::FrequencyIncrease() {
  if (frequencySelectedIndex < frequencyOptions.size() - 1) {
    frequencySelectedIndex++;
    FrequencyUpdate();
  }
}

void SettingChimes::DurationUpdate() {
  bool canDecrease = durationSelectedIndex > 0;
  lv_obj_set_state(durationDecreaseButton, canDecrease ? LV_STATE_DEFAULT : LV_STATE_DISABLED);
  bool canIncrease = durationSelectedIndex < durationOptions.size() - 1;
  lv_obj_set_state(durationIncreaseButton, canIncrease ? LV_STATE_DEFAULT : LV_STATE_DISABLED);
  lv_label_set_text_fmt(durationLabel, "buzz for\n%d ms.", GetDurationSelection());

  if (finishedLoading) {
    uint8_t duration = GetDurationSelection();
    motorController.RunForDuration(duration);
    settingsController.SetChimesDuration(duration);
  }
}

void SettingChimes::DurationIncrease() {
  if (durationSelectedIndex < durationOptions.size() - 1) {
    durationSelectedIndex++;
    DurationUpdate();
  }
}

void SettingChimes::DurationDecrease() {
  if (durationSelectedIndex > 0) {
    durationSelectedIndex--;
    DurationUpdate();
  }
}
