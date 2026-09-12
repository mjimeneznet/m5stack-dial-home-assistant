#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace dial_climates {

class DialClimates : public Component {
 public:
  void add_climate(const std::string &entity_id, const std::string &name, sensor::Sensor *target_temp = nullptr,
                   sensor::Sensor *current_temp = nullptr, sensor::Sensor *humidity = nullptr,
                   text_sensor::TextSensor *hvac_mode = nullptr, text_sensor::TextSensor *hvac_modes = nullptr,
                   text_sensor::TextSensor *fan_mode = nullptr, text_sensor::TextSensor *fan_modes = nullptr);
  void setup() override;

  size_t climate_count() const { return this->climates_.size(); }
  size_t active_index() const { return this->active_index_; }
  void select_climate(size_t index);
  void select_next();
  void select_previous();
  const std::string &active_name() const;
  const std::string &active_entity_id() const;
  const std::string &name_at(size_t index) const;

  bool active_target_temp_valid() const;
  float active_target_temp() const;
  bool active_current_temp_valid() const;
  float active_current_temp() const;
  bool active_humidity_valid() const;
  float active_humidity() const;
  bool active_hvac_mode_valid() const;
  const std::string &active_hvac_mode() const;
  bool active_hvac_modes_valid() const;
  const std::string &active_hvac_modes() const;
  bool active_fan_mode_valid() const;
  const std::string &active_fan_mode() const;
  bool active_fan_modes_valid() const;
  const std::string &active_fan_modes() const;
  bool active_is_on() const;

 protected:
  struct ClimateEntry {
    std::string entity_id;
    std::string name;
    sensor::Sensor *target_temp{nullptr};
    bool target_temp_valid{false};
    float target_temp_value{0.0f};
    sensor::Sensor *current_temp{nullptr};
    bool current_temp_valid{false};
    float current_temp_value{0.0f};
    sensor::Sensor *humidity{nullptr};
    bool humidity_valid{false};
    float humidity_value{0.0f};
    text_sensor::TextSensor *hvac_mode{nullptr};
    bool hvac_mode_valid{false};
    bool is_on{false};
    std::string hvac_mode_value;
    text_sensor::TextSensor *hvac_modes{nullptr};
    bool hvac_modes_valid{false};
    std::string hvac_modes_value;
    text_sensor::TextSensor *fan_mode{nullptr};
    bool fan_mode_valid{false};
    std::string fan_mode_value;
    text_sensor::TextSensor *fan_modes{nullptr};
    bool fan_modes_valid{false};
    std::string fan_modes_value;
  };

  const ClimateEntry &active_entry_() const;
  void on_target_temp_(size_t index, float value);
  void on_current_temp_(size_t index, float value);
  void on_humidity_(size_t index, float value);
  void on_hvac_mode_(size_t index, const std::string &value);
  void on_hvac_modes_(size_t index, const std::string &value);
  void on_fan_mode_(size_t index, const std::string &value);
  void on_fan_modes_(size_t index, const std::string &value);

  std::vector<ClimateEntry> climates_;
  size_t active_index_{0};
};

}  // namespace dial_climates
}  // namespace esphome
