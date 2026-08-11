# Project Structure and Naming

## Root files

- `dial.yaml`: ESPHome entry point.
- `secrets.example.yaml`: public template for credentials.
- `requirements.txt`: pinned ESPHome version for reproducible builds.
- `README.md`: community-facing overview and setup guide.
- `THIRD_PARTY_NOTICES.md`: notes for copied components, fonts, icons, and dependencies.

## Source layout

- `main/hardware.yaml`: M5Stack Dial hardware drivers and pins.
- `main/entities.yaml`: Home Assistant entity bindings.
- `pages/*.yaml`: one LVGL page per feature.
- `fonts/`: local fonts.
- `images/`: small embedded image assets.
- `components/`: local ESPHome external components.

## ID conventions

- `page_`: LVGL pages.
- `lbl_`: labels.
- `btn_`: buttons.
- `arc_`: arcs/progress rings.
- `img_`: images.
- `col_`: colors.
- `font_`: fonts.

## What not to publish

- `secrets.yaml`
- `.esphome/`
- `esphome-env/` or `.venv/`
- `.vscode/`
- `mini_code/`
- `__pycache__/`, `*.pyc`, `.DS_Store`
