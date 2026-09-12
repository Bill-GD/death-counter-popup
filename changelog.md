# Changelog

## 1.1.2

- Bump Geode to 5.10.1
- Add setting to configure label popup style (3 styles).
- Minor code changes

## 1.1.1

- Rework how the mod get percentage.
- A minor fix to run start percentage.

## 1.1.0

- Add decimal precision support for label:
  - The label can be integer (`34x8`, `56-100x2`...), or decimal (`34.1x4`, `56.99-100.00x2`).
  - The run count is grouped, `1-3`'s count includes both `1.2-3.4` and `1.56-3-99`.
  - The label precision level can be changed in setting,
    and the save data will always keep track of the most precise variant.
  - This update also <cr>changed the save data format</cr>, the mod will update the data, but if it fails,
    try the migrate button in the mod setting.
- Add setting to configure label opacity.
- Percentage in label will be properly clamped to 0-100,
  any run that isn't completion but have progress bar already at 100 will be clamped to 99.999 (similar to vanilla).
- Fix saving on exit even if no level was loaded.

## 1.0.3

- Improve label for percentages between 0 and 1 (decimals).
- Update description & README.

## 1.0.2

- Add explicit checks for JSON key.

## 1.0.1

- Add some guards to file reading & parsing.

## 1.0.0

- Initial version.
