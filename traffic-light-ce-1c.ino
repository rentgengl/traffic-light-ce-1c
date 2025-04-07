#include <Adafruit_NeoPixel.h>
#include <RTClib.h>

#define PIN            2  // Пин ESP32, к которому подключена лента
#define NUMPIXELS      8  // Количество пикселей (сегментов) на ленте WS2812B

#define PIX_RED        4
#define PIX_YELLOW     2
#define PIX_TIME       1
#define PIX_GREEN      0
#define PIX_RED2       5
#define PIX_GREEN2     7

#define DELAY_BLINK    1000
#define BRIGHTNESS     128  // Яркость (0-255)

RTC_DS3231 rtc;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

DateTime startOfWorkDay(0, 0, 0, 8, 0, 0);  // 8:00:00
DateTime endOfWorkDay(0, 0, 0, 17, 0, 0);   // 17:00:00

DateTime startLanch(0, 0, 0, 11, 25, 0);
DateTime endLanch(0, 0, 0, 12, 29, 0);

void setup() {
  strip.begin();  // Инициализация ленты
  strip.setBrightness(BRIGHTNESS);  // Устанавливаем яркость
  strip.show();  // Обновление ленты (выключить все светодиоды)

  // Настройка модуля времени
  if (!rtc.begin()) {
    on_pix(PIX_RED2);
    delay(DELAY_BLINK);
  }

  on_pix(PIX_GREEN2);
  delay(DELAY_BLINK);
  off_all_pixels();

  // Если было отключение батарейки, то обновляю время
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// Отключение всех пикселей
void off_all_pixels() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));  // Отключить пиксель
  }
  strip.show();  // Обновить ленту
}

// Функция для получения времени без секунд
DateTime get_time(DateTime date_time) {
  return DateTime(0, 0, 0, date_time.hour(), date_time.minute());
}

// Функция для получения цвета пикселя
uint32_t get_pixel_color(int pix) {
  switch (pix) {
    case PIX_RED:
    case PIX_RED2:
      return strip.Color(255, 0, 0);  // Красный
    case PIX_YELLOW:
      return strip.Color(255, 255, 0);  // Желтый
    case PIX_GREEN:
    case PIX_GREEN2:
      return strip.Color(0, 255, 0);  // Зеленый
    default:
      return strip.Color(0, 0, 0);  // По умолчанию выключено
  }
}

// Функция для включения пикселя с заданным цветом
void on_pix(int pix) {
  uint32_t color = get_pixel_color(pix);
  strip.setPixelColor(pix, color);
  strip.show();  // Обновление ленты
}

// Функция для мигания пикселя
void blink(int pix, int repeat = 5) {
  for (int i = 0; i < repeat; i++) {
    on_pix(pix);           // Включить пиксель
    delay(DELAY_BLINK);    // Задержка
    off_all_pixels();      // Отключить все пиксели
    delay(DELAY_BLINK);    // Задержка
  }
}

void check_work_time(DateTime time_now) {
  off_all_pixels();
  if (time_now >= startOfWorkDay && time_now <= endOfWorkDay) {
    if (time_now == startLanch) {
      blink(PIX_YELLOW, 20);  // Мигаем желтым на старте ланча
    }

    if (time_now >= startLanch && time_now <= endLanch) {
      on_pix(PIX_RED);  // Время обеда — красный
    } else {
      on_pix(PIX_GREEN);  // Остальное время — зеленый
    }
  } else {
    on_pix(PIX_RED);  // Вне рабочего времени — красный
  }
}

void loop() {
  DateTime now_time = get_time(rtc.now());  // Получаем текущее время
  check_work_time(now_time);  // Проверяем рабочие часы и устанавливаем соответствующие пиксели
  delay(10000);  // Задержка 10 секунд перед следующим циклом
}
