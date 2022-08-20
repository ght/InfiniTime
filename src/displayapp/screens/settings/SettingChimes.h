#pragma once

#include <cstdint>
#include <lvgl/lvgl.h>
#include "components/motor/MotorController.h"
#include "components/settings/Settings.h"
#include "displayapp/screens/Screen.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingChimes : public Screen {
      public:
        SettingChimes(DisplayApp* app,
                      Pinetime::Controllers::MotorController& motorController,
                      Pinetime::Controllers::Settings& settingsController);
        ~SettingChimes() override;

        void FrequencyDecrease();
        void FrequencyIncrease();
        void DurationDecrease();
        void DurationIncrease();

      private:
        Controllers::MotorController& motorController;
        Controllers::Settings& settingsController;

        bool finishedLoading = false;

        lv_obj_t* wrapper;
        lv_obj_t* title;
        lv_obj_t* icon;

        // Frequency
        struct Option {
          uint8_t value;
          const char* label;
        };

        static constexpr std::array<Option, 8> frequencyOptions = {{
          {60, "full hour"},
          {30, "half hour"},
          {20, "20 mins"},
          {15, "15 mins"},
          {10, "ten mins"},
          {5, "five mins"},
          {2, "two mins"},
          {1, "minute"}
        }};
        uint8_t frequencySelectedIndex = 0;

        Option GetFrequencySelection() {
          return frequencyOptions[frequencySelectedIndex];
        }

        void FrequencyUpdate();

        lv_obj_t* frequencyContainer;
        lv_obj_t* frequencyLabel;
        lv_obj_t* frequencyDecreaseButton;
        lv_obj_t* frequencyDecreaseLabel;
        lv_obj_t* frequencyIncreaseButton;
        lv_obj_t* frequencyIncreaseLabel;

        // Duration
        static constexpr std::array<uint8_t, 7> durationOptions = { 10, 35, 50, 80, 120, 180, 250 };
        uint8_t durationSelectedIndex = 0;

        uint8_t GetDurationSelection() {
          return durationOptions[durationSelectedIndex];
        }

        void DurationUpdate();

        lv_obj_t* durationContainer;
        lv_obj_t* durationLabel;
        lv_obj_t* durationDecreaseButton;
        lv_obj_t* durationDecreaseLabel;
        lv_obj_t* durationIncreaseButton;
        lv_obj_t* durationIncreaseLabel;
      };
    }
  }
}
