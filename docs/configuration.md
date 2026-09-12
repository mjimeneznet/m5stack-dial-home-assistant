# Home Assistant Controller for M5Stack Dial configuration reference

This is the detailed reference for the Home Assistant Controller for M5Stack Dial `dial.yaml` remote package. For an overview and a ready-to-use installation example, return to the [README](../README.md). A normal package installation is configured entirely in your local ESPHome YAML; it does not require editing files inside this repository.

## Device credentials

```yaml
substitutions:
  timezone: Europe/Madrid
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
```

The package defaults `device_name`, `device_friendly_name`, fallback hotspot values and OTA password for validation. Override those values, especially the OTA password, for a real installation. Keep credentials in `secrets.yaml` and never commit them.

## Optional Clock data

```yaml
substitutions:
  timezone: Europe/Madrid
  weather_entity: weather.your_location
  aqi_entity: sensor.your_aqi
```

`weather_entity` and `aqi_entity` are optional: the package defaults them to placeholder entity IDs, so they are not required to compile. If either entity is missing or unavailable, Clock shows incomplete data or `--`. `weather_entity` supplies weather information; `aqi_entity` should be numeric, with the weather entity's legacy `aqi` attribute used only as a fallback. Find entity IDs under **Developer Tools → States**.


## Optional menu features

Unconfigured optional features are hidden from the menu. Timer is hidden while `timer_entity` remains `timer.your_timer`; AC while `climate_entity` remains `climate.your_ac`; and Music while `music_player_entity` remains `media_player.your_player`. Lights is hidden when `dial_lights` has no entries.

| Field | Enables | Example |
| --- | --- | --- |
| `dial_lights` | Lights | A list of Home Assistant light entities. |
| `climate_entity` | AC | `climate.living_room` |
| `music_player_entity` | Music | `media_player.living_room` |
| `timer_entity` | Timer | `timer.dial_timer` |

### Lights

Every configured light is one entry of the package file list. Add one `main/dial_light.yaml` entry per light and pass that light through `vars:`:

```yaml
packages:
  smart_home_button:
    url: https://github.com/mjimeneznet/m5stack-dial-home-assistant
    ref: main
    refresh: 0s
    files:
      - dial.yaml
      - path: main/dial_light.yaml
        vars: {prefix: sofa, light: light.sofa, name: Sofá}
      - path: main/dial_light.yaml
        vars: {prefix: mesa, light: light.mesa, name: Mesa}
```

| Variable | Meaning |
| --- | --- |
| `prefix` | Short, unique id prefix for this light's sensors (`sofa`, `mesa`, ...). Lowercase, no spaces. |
| `light` | The Home Assistant light `entity_id`. |
| `name` | The label shown on the Dial. |

The template declares the Home Assistant sensors the page reads for live state — state, supported colour modes, colour, colour mode, brightness and the colour-temperature range — and hooks state and brightness into the menu subtitle. An attribute the light does not expose simply stays empty, and an unavailable entity does not synchronise. Add an entry to add a light; remove it to drop one; with none left, Lights disappears from the menu.

#### Manual light entries

If you prefer to own the sensors, or you already have them, declare a `dial_lights` entry per light and point its `*_sensor` fields at your own sensor ids:

```yaml
dial_lights:
  - entity_id: light.desk
    name: Escritorio
    state_sensor: dial_light_desk_state
    modes_sensor: dial_light_desk_modes
    brightness_sensor: dial_light_desk_brightness
    color_sensor: dial_light_desk_rgb
    color_mode_sensor: dial_light_desk_color_mode
    color_temp_kelvin_sensor: dial_light_desk_color_temp_kelvin
    min_color_temp_kelvin_sensor: dial_light_desk_min_color_temp_kelvin
    max_color_temp_kelvin_sensor: dial_light_desk_max_color_temp_kelvin
```

Each entry needs an `entity_id` and a display `name`; the `*_sensor` fields are optional. Without them the light can still be switched and dimmed, but the page has no state, colour or brightness to read back. The matching sensors look like this, one group per light:

```yaml
text_sensor:
  - platform: homeassistant
    id: dial_light_desk_state
    entity_id: light.desk
    internal: true
    on_value:
      - script.execute: menu_on_ha_update

  - platform: homeassistant
    id: dial_light_desk_modes
    entity_id: light.desk
    attribute: supported_color_modes
    internal: true

  - platform: homeassistant
    id: dial_light_desk_rgb
    entity_id: light.desk
    attribute: rgb_color
    internal: true

  - platform: homeassistant
    id: dial_light_desk_color_mode
    entity_id: light.desk
    attribute: color_mode
    internal: true

sensor:
  - platform: homeassistant
    id: dial_light_desk_brightness
    entity_id: light.desk
    attribute: brightness
    internal: true
    on_value:
      - script.execute: menu_on_ha_update

  - platform: homeassistant
    id: dial_light_desk_color_temp_kelvin
    entity_id: light.desk
    attribute: color_temp_kelvin
    internal: true

  - platform: homeassistant
    id: dial_light_desk_min_color_temp_kelvin
    entity_id: light.desk
    attribute: min_color_temp_kelvin
    internal: true

  - platform: homeassistant
    id: dial_light_desk_max_color_temp_kelvin
    entity_id: light.desk
    attribute: max_color_temp_kelvin
    internal: true
```

`menu_on_ha_update` is a script provided by the package and keeps the menu subtitle current; every sensor must be `internal: true` so it does not appear in Home Assistant. Use `main/dial_light.yaml` as the reference for the ids and attributes. With no light entries at all, from either path, Lights disappears from the menu.

### Climate

Every configured air conditioner or thermostat is one entry of the package file list, the same way lights work. Add one `main/dial_climate.yaml` entry per climate and pass it through `vars:`:

```yaml
packages:
  smart_home_button:
    url: https://github.com/mjimeneznet/m5stack-dial-home-assistant
    ref: main
    refresh: 0s
    files:
      - dial.yaml
      - path: main/dial_climate.yaml
        vars: {prefix: buhardilla, climate: climate.ac_buhardilla, name: Buhardilla}
      - path: main/dial_climate.yaml
        vars: {prefix: salon, climate: climate.ac_salon, name: Salón}
```

| Variable | Meaning |
| --- | --- |
| `prefix` | Short, unique id prefix for this climate's sensors (`buhardilla`, `salon`, ...). Lowercase, no spaces. |
| `climate` | The Home Assistant climate `entity_id`. |
| `name` | The label shown on the Dial and in the selector. |

The template declares the sensors the page reads — target temperature, current temperature, humidity, HVAC mode, HVAC modes, fan mode and fan modes — and drives the page only for the climate you have selected. An attribute a climate does not expose simply stays empty.

With **one** climate the menu opens the AC page directly. With **more than one** it opens a selector first: rotate to move between climates, swipe left (or a short press) to open the selected one, swipe right to go back. From the AC page, back returns to the selector.

The page supports target temperature and uses the modes advertised by the entity. HVAC mode, fan mode and swing-related capabilities vary between Home Assistant integrations, so only controls supported by the entity should be expected.

Without any `main/dial_climate.yaml` entry, the legacy single-climate mode still works: set the substitution and the page uses that entity, hidden when it keeps its placeholder value.

```yaml
substitutions:
  climate_entity: climate.living_room
```

### Music

```yaml
substitutions:
  music_player_entity: media_player.living_room
```

Select the entity that actually plays the audio. Home Assistant supplies player state, transport actions, volume and available metadata, including `media_title`, `media_artist`, `media_duration` and `media_position`. SendSpin is optional and currently supplies only 100 × 100 album artwork when a compatible SendSpin source is available.

### Timer

```yaml
substitutions:
  timer_entity: timer.dial_timer
```

Create the referenced Home Assistant Timer helper. Home Assistant remains the source of truth, so dashboards and automations can control it too. For YAML-defined timers, `restore: true` is optional but can be useful after a Home Assistant restart.

```yaml
timer:
  dial_timer:
    name: Dial timer
    duration: "00:05:00"
    restore: true
```

## Screen-management reference

```yaml
substitutions:
  screen_dim_timeout: 45s
  screen_return_timeout: 5min
  screen_off_timeout: 30min
  screen_dim_brightness: "20%"
```

| Setting | Default | Behaviour |
| --- | --- | --- |
| `screen_dim_timeout` | `45s` | Dims the current page's backlight. |
| `screen_return_timeout` | `5min` | Returns to Clock. |
| `screen_off_timeout` | `30min` | Turns off only the display backlight. |
| `screen_dim_brightness` | `"20%"` | Backlight level during DIM. |

Set a timeout to `0s` to disable it. DIM and RETURN are independent. With OFF enabled, the package raises an OFF timeout that is shorter than an enabled DIM or RETURN timeout to the later timeout. DIM and OFF retain the active page; RETURN intentionally navigates to Clock. The first encoder turn, front-button press or touch gesture from DIM or OFF wakes the display and is consumed.

Only an **active** Home Assistant timer blocks RETURN. A paused timer does not. Timer state does not block DIM or OFF. A finished timer wakes the display, opens Timer and triggers its visual blink and buzzer feedback. Remote timer starts do not wake the display.

## Package refresh and validation

`ref: main` follows the version currently published on the repository's `main` branch. `refresh: 0s` makes ESPHome check the remote package on every configuration or build, which is useful while tracking it, but depends on GitHub being reachable and can add download time. It is optional; pin a tag or commit in `ref` for reproducible builds. Recompile after package updates.

### Package assets

ESPHome resolves relative paths inside a remote package against **your** configuration directory, so the package cannot ship its images and fonts as local files. It downloads them from `asset_base_url` instead, which defaults to the repository's `main` branch:

```yaml
substitutions:
  asset_base_url: https://raw.githubusercontent.com/mjimeneznet/m5stack-dial-home-assistant/main
```

Override it with the same ref you pin when you want YAML and assets to move together:

```yaml
packages:
  smart_home_button:
    url: https://github.com/mjimeneznet/m5stack-dial-home-assistant
    ref: v1.0.0
    files: [dial.yaml]

substitutions:
  asset_base_url: https://raw.githubusercontent.com/mjimeneznet/m5stack-dial-home-assistant/v1.0.0
```

The first validation or build needs network access; ESPHome then caches the downloaded files locally.

Validate before flashing:

```bash
esphome config your-dial.yaml
esphome compile your-dial.yaml
```

Use USB for the initial installation if the device is not on Wi-Fi; later updates can use ESPHome OTA.

## Technical troubleshooting

- **Invalid API encryption key:** generate a valid ESPHome API key and put it in `secrets.yaml`.
- **Entity not found or unavailable:** check its exact ID and availability in Home Assistant; a configured unavailable feature stays in the menu but cannot synchronise.
- **Lights missing:** add a `main/dial_light.yaml` entry to your `packages:` block, or declare `dial_lights` yourself; Lights is hidden while no light is configured.
- **Lights listed without state, brightness or colour:** use the `main/dial_light.yaml` template, or point the entry's `*_sensor` fields at your own Home Assistant sensors, as shown in the Lights section.
- **AQI empty:** use a numeric sensor, not a textual state.
- **Fonts or glyphs fail during build:** allow the initial build to download Google Fonts, the package images and fonts, and dependencies; use the version pinned in `requirements.txt`.
- **Compilation error after an update:** validate the complete local YAML and refresh the package before retrying.

## Development-only customisation

Clone the repository when you need to change the firmware itself. `pages/` contains the LVGL pages, while `main/` contains hardware, default entities, idle logic and light sensors. Install `requirements.txt`, copy `secrets.example.yaml` to a local `secrets.yaml`, and run `esphome config dial.yaml` before compiling. These internal files are not part of the normal remote-package workflow.
