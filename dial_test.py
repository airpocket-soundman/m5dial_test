#include <M5Dial.h>

volatile int pulseCount = 0;
unsigned long lastUpdate = 0;
int rpm = 0;

const int sensorPin = 1;  // GPIO1（入力）

void IRAM_ATTR onPulse() {
  pulseCount++;  // 割り込みでパルスを加算
}

void setup() {
  M5Dial.begin();
  M5Dial.Display.setTextSize(3);
  M5Dial.Display.setTextColor(WHITE);
  M5Dial.Display.fillScreen(BLACK);

  // GPIO1を内部プルアップ＋割り込み設定（立下り）
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), onPulse, FALLING);

  lastUpdate = millis();
}

void loop() {
  M5Dial.update();

  // 1秒ごとに更新
  if (millis() - lastUpdate >= 1000) {
    noInterrupts();
    int count = pulseCount;
    pulseCount = 0;
    interrupts();

    // タコメーター式計算（1回転2パルス → rpm = count / 2 * 60）
    rpm = (count / 2) * 60;

    // 表示更新
    M5Dial.Display.fillScreen(BLACK);
    M5Dial.Display.setCursor(10, 40);
    M5Dial.Display.printf("RPM: %d", rpm);

    lastUpdate = millis();
  }

  delay(10);  // 少し休ませる
}

