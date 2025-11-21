void updateParameters(byte paramNumberNavigation, int32_t paramValueNavigation) {
  switch (paramNumberNavigation) {
    // MENU / UI navigation actions sent via 'y' from the input controller.
    //
    // We keep this lightweight and focused on state that isn't already
    // handled by the mainboard's param -> screen path ('p'/'w'/'x').

    case (uint8_t)ParamId::PARAM_MANUAL_CALIBRATION_STAGE:
      // Update the current manual calibration stage and derived oscillator N
      // so the manual calibration screen tracks encoder-driven stage changes.
      manualCalibrationStage = (uint8_t)paramValueNavigation;
      manualCalibrationOSCN  = manualCalibrationStage / 2;
      break;

    case (uint8_t)ParamId::PARAM_MANUAL_CALIBRATION_OFFSET:
      // Directly set the displayed offset when the input controller sends
      // the per-oscillator value (e.g. after a stage change or knob tweak),
      // avoiding any "carry over" from the previous oscillator.
      offset = (int8_t)paramValueNavigation;
      break;

    default:
      break;
  }
}