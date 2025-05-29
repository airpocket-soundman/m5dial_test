#include <M5Dial.h>
#include <lvgl.h>
#include "ui.h"

void my_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  M5Dial.Display.startWrite();
  M5Dial.Display.pushImage(
    area->x1, area->y1,
    area->x2 - area->x1 + 1,
    area->y2 - area->y1 + 1,
    (lgfx::rgb565_t *)color_p
  );
  M5Dial.Display.endWrite();
  lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  auto detail = M5Dial.Touch.getDetail();  // 戻り値で取得する
  Serial.printf("Touch: (%d, %d) Pressed: %d\n", detail.x, detail.y, detail.isPressed());

  if (detail.isPressed()) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = detail.x;
    data->point.y = detail.y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200);  // ← これを最初に追加！
  delay(1000);           // ← 起動直後の安定のため1秒待つ

  auto cfg = M5.config();
  M5Dial.begin(cfg);
  lv_init();

  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf[240 * 240 / 10];
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, sizeof(buf) / sizeof(lv_color_t));

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = my_flush_cb;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touch_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();
}

void loop() {
  M5Dial.update();
  lv_timer_handler();
  delay(5);
}
