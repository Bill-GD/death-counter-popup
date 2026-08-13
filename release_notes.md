- Add decimal precision support for label:
  - The label can be integer (`34x8`, `56-100x2`...), or decimal (`34.1x4`, `56.99-100.00x2`).
  - The run count is grouped, `1-3`'s count includes both `1.2-3.4` and `1.56-3-99`.
  - The label precision level can be changed in setting,
    and the save data will always keep track of the most precise variant.
  - Changed the save data format & auto migrate the data on load
    Manual migrate button also added in the mod setting.
- Add setting to configure label opacity.
- Percentage in label will be properly clamped to 0-100 &
  any run that isn't completion but have progress bar already at 100 will be clamped to 99.999 (similar to vanilla).
- Fix saving on exit even if no level was loaded.