#include "dial_climates.h"

#include <cctype>

#include "esphome/core/log.h"

namespace esphome {
namespace dial_climates {

static const char *const TAG = "dial_climates";

namespace {

// Mirrors the values the AC page treats as "no reading": an attribute a climate does not
// expose arrives as one of these, so it must not overwrite a real mode.
bool is_invalid(const std::string &raw) {
  if (raw.empty())
    return true;
  std::string lower;
  lower.reserve(raw.size());
  for (char c : raw)
    lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return lower == "unknown" || lower == "unavailable" || lower == "none" || lower == "null";
}

}  // namespace

void DialClimates::add_climate(const std::string &entity_id, const std::string &name, sensor::Sensor *target_temp,
                               sensor::Sensor *current_temp, sensor::Sensor *humidity,
                               text_sensor::TextSensor *hvac_mode, text_sensor::TextSensor *hvac_modes,
                               text_sensor::TextSensor *fan_mode, text_sensor::TextSensor *fan_modes) {
  for (const auto &existing : this->climates_) {
    if (existing.entity_id == entity_id) {
      ESP_LOGW(TAG, "Duplicate climate entity_id ignored: %s", entity_id.c_str());
      return;
    }
  }
  ClimateEntry entry;
  entry.entity_id = entity_id;
  entry.name = name;
  entry.target_temp = target_temp;
  entry.current_temp = current_temp;
  entry.humidity = humidity;
  entry.hvac_mode = hvac_mode;
  entry.hvac_modes = hvac_modes;
  entry.fan_mode = fan_mode;
  entry.fan_modes = fan_modes;
  this->climates_.push_back(entry);
}

void DialClimates::setup() {
  for (size_t i = 0; i < this->climates_.size(); i++) {
    auto &climate = this->climates_[i];
    if (climate.target_temp != nullptr) {
      climate.target_temp->add_on_state_callback([this, i](float value) { this->on_target_temp_(i, value); });
      if (climate.target_temp->has_state())
        this->on_target_temp_(i, climate.target_temp->state);
    }
    if (climate.current_temp != nullptr) {
      climate.current_temp->add_on_state_callback([this, i](float value) { this->on_current_temp_(i, value); });
      if (climate.current_temp->has_state())
        this->on_current_temp_(i, climate.current_temp->state);
    }
    if (climate.humidity != nullptr) {
      climate.humidity->add_on_state_callback([this, i](float value) { this->on_humidity_(i, value); });
      if (climate.humidity->has_state())
        this->on_humidity_(i, climate.humidity->state);
    }
    if (climate.hvac_mode != nullptr) {
      climate.hvac_mode->add_on_state_callback([this, i](const std::string &value) { this->on_hvac_mode_(i, value); });
      if (climate.hvac_mode->has_state())
        this->on_hvac_mode_(i, climate.hvac_mode->state);
    }
    if (climate.hvac_modes != nullptr) {
      climate.hvac_modes->add_on_state_callback([this, i](const std::string &value) { this->on_hvac_modes_(i, value); });
      if (climate.hvac_modes->has_state())
        this->on_hvac_modes_(i, climate.hvac_modes->state);
    }
    if (climate.fan_mode != nullptr) {
      climate.fan_mode->add_on_state_callback([this, i](const std::string &value) { this->on_fan_mode_(i, value); });
      if (climate.fan_mode->has_state())
        this->on_fan_mode_(i, climate.fan_mode->state);
    }
    if (climate.fan_modes != nullptr) {
      climate.fan_modes->add_on_state_callback([this, i](const std::string &value) { this->on_fan_modes_(i, value); });
      if (climate.fan_modes->has_state())
        this->on_fan_modes_(i, climate.fan_modes->state);
    }
  }
}

const DialClimates::ClimateEntry &DialClimates::active_entry_() const {
  static const ClimateEntry empty{};
  if (this->climates_.empty() || this->active_index_ >= this->climates_.size())
    return empty;
  return this->climates_[this->active_index_];
}

void DialClimates::select_climate(size_t index) {
  if (this->climates_.empty())
    return;
  this->active_index_ = index % this->climates_.size();
}

void DialClimates::select_next() {
  if (!this->climates_.empty())
    this->active_index_ = (this->active_index_ + 1) % this->climates_.size();
}

void DialClimates::select_previous() {
  if (!this->climates_.empty())
    this->active_index_ = (this->active_index_ + this->climates_.size() - 1) % this->climates_.size();
}

const std::string &DialClimates::active_name() const { return this->active_entry_().name; }

const std::string &DialClimates::active_entity_id() const { return this->active_entry_().entity_id; }

const std::string &DialClimates::name_at(size_t index) const {
  static const std::string empty;
  if (index >= this->climates_.size())
    return empty;
  return this->climates_[index].name;
}

bool DialClimates::active_target_temp_valid() const { return this->active_entry_().target_temp_valid; }
float DialClimates::active_target_temp() const { return this->active_entry_().target_temp_value; }
bool DialClimates::active_current_temp_valid() const { return this->active_entry_().current_temp_valid; }
float DialClimates::active_current_temp() const { return this->active_entry_().current_temp_value; }
bool DialClimates::active_humidity_valid() const { return this->active_entry_().humidity_valid; }
float DialClimates::active_humidity() const { return this->active_entry_().humidity_value; }
bool DialClimates::active_hvac_mode_valid() const { return this->active_entry_().hvac_mode_valid; }
const std::string &DialClimates::active_hvac_mode() const { return this->active_entry_().hvac_mode_value; }
bool DialClimates::active_hvac_modes_valid() const { return this->active_entry_().hvac_modes_valid; }
const std::string &DialClimates::active_hvac_modes() const { return this->active_entry_().hvac_modes_value; }
bool DialClimates::active_fan_mode_valid() const { return this->active_entry_().fan_mode_valid; }
const std::string &DialClimates::active_fan_mode() const { return this->active_entry_().fan_mode_value; }
bool DialClimates::active_fan_modes_valid() const { return this->active_entry_().fan_modes_valid; }
const std::string &DialClimates::active_fan_modes() const { return this->active_entry_().fan_modes_value; }
bool DialClimates::active_is_on() const { return this->active_entry_().is_on; }

void DialClimates::on_target_temp_(size_t index, float value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  climate.target_temp_value = value;
  climate.target_temp_valid = !std::isnan(value);
}

void DialClimates::on_current_temp_(size_t index, float value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  climate.current_temp_value = value;
  climate.current_temp_valid = !std::isnan(value);
}

void DialClimates::on_humidity_(size_t index, float value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  climate.humidity_value = value;
  climate.humidity_valid = !std::isnan(value);
}

void DialClimates::on_hvac_mode_(size_t index, const std::string &value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  if (is_invalid(value)) {
    climate.hvac_mode_valid = false;
    climate.is_on = false;
    return;
  }
  climate.hvac_mode_value = value;
  climate.hvac_mode_valid = true;
  climate.is_on = value != "off";
}

void DialClimates::on_hvac_modes_(size_t index, const std::string &value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  climate.hvac_modes_valid = !is_invalid(value);
  climate.hvac_modes_value = climate.hvac_modes_valid ? value : std::string();
}

void DialClimates::on_fan_mode_(size_t index, const std::string &value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  if (is_invalid(value)) {
    climate.fan_mode_valid = false;
    return;
  }
  climate.fan_mode_value = value;
  climate.fan_mode_valid = true;
}

void DialClimates::on_fan_modes_(size_t index, const std::string &value) {
  if (index >= this->climates_.size())
    return;
  auto &climate = this->climates_[index];
  climate.fan_modes_valid = !is_invalid(value);
  climate.fan_modes_value = climate.fan_modes_valid ? value : std::string();
}

}  // namespace dial_climates
}  // namespace esphome
