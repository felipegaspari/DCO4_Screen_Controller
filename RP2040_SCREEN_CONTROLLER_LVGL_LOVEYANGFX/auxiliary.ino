// Toggle blinker1Flag ~70 ms. Currently unused (no live callers in loop).
void blinker1() {
  if (millis() - blinkTimer1 > 70) {
    blinkTimer1 = millis();
    blinker1Flag  = !blinker1Flag;
  }
}

// Toggle blinker2Flag ~150 ms. Currently unused (no live callers in loop).
void blinker2() {
  if (millis() - blinkTimer1 > 150) {
    blinkTimer2 = millis();
    blinker2Flag  = !blinker1Flag;
  }
}

