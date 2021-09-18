/*
 * Project author : @vla-vi21 (inst);
 * 
 * Author of libraries : <GyverOS.h> - Alex Gyver ;
 * Chart - article : https://alexgyver.ru/lcd-plots-and-bars/ ; 
 */
 
 
#include <GyverOS.h> //подключаем библиотеку Гайвер ОС 
GyverOS<1> OS; //ставим сколько задач будет выполняться
#include <Wire.h> //подключаем библиотеку для подсветки дисплея
#include <LiquidCrystal_I2C.h> //подключаем библиотеку для управления дисплеем

#define SENSOR A0 //обьявляем константу переменной фоторезистора и пин 

LiquidCrystal_I2C lcd(0x27, 16, 2); //указываем параметры дисплея

unsigned long period_time = (long)5760000;// Нам нужно задать период таймера В МИЛЛИСЕКУНДАX (в примере  1,6 ЧАСА)
// (long) обязательно для больших чисел, иначе не посчитает
// ПРИМЕРЫ С МС -В- ЧАС/МИН \\
// 60000 МС - 1 МИНУТА
// 1800000 МС - 30 МИНУТ
// 3600000 МС - 1 ЧАС
// 5760000 МС - 1,6 ЧАСА
// 7200000 МС - 2 ЧАСА
// 10800000 МС - 3 ЧАСА

unsigned long my_timer;// переменная таймера, максимально большой целочисленный тип (он же uint32_t)

int plot_array[20]; //массив нашего графика

void setup() {
  lcd.init();  //инициализация подсветки дисплея (wire)
  lcd.backlight();
  lcd.clear();

  my_timer = millis();   // "сбросить" таймер

  OS.attach(0, task0, 1000); //пишем 1 задачу (0)

  initPlot(); //команда для посторения графиков
}
void task0() {                   //это 1 задача
  int val1 = analogRead(SENSOR); //с переменной фоторезистора извлекаем значения

  lcd.setCursor(10, 0);
  lcd.print("SENSOR");
  lcd.setCursor(12, 1);
  lcd.print(val1);
  delay(100);
  //выводим на дисплей
}


void loop() {                 //бесконечный цикл Arduino
  OS.tick();  //как можно чаще вызываем эту команду (чтобы 0 задача работала стабильно)
  if ((long)millis() - my_timer > period_time) {
    my_timer = millis();   // "сбросить" таймер
    int val = map(analogRead(0), 0, 1023, 0, 1000); //в переменную val записываем параметры
    drawPlot(0, 1, 10, 2, 0, 1000, val); //записываем параметры для графика
    delay(100);
  }
  delay(100);
}



void initPlot() {
  // необходимые символы для работы
  // создано в http://maxpromer.github.io/LCD-Character-Creator/
  byte row8[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row7[8] = {0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row6[8] = {0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row5[8] = {0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row4[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111,  0b11111};
  byte row3[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
  byte row2[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
  byte row1[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111};
  lcd.createChar(0, row8);
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
}
// построение графика
void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int fill_val) {
  for (byte i = 0; i < width; i++) {
    plot_array[i] = plot_array[i + 1];
  }
  fill_val = constrain(fill_val, min_val, max_val);
  plot_array[width] = fill_val;
  for (byte i = 0; i < width; i++) {                  // каждый столбец параметров
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    infill = floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10);
    fract = (infill % 10) * 8 / 10;                   // найти количество оставшихся полосок
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {     // для всех строк графика
      if (n < infill && infill > 0) {       // пока мы ниже уровня
        lcd.setCursor(i, (row - n));        // заполняем полными ячейками
        lcd.write(0);
      }
      if (n >= infill) {                    // если достигли уровня
        lcd.setCursor(i, (row - n));
        if (fract > 0) lcd.write(fract);          // заполняем дробные ячейки
        else lcd.write(16);                       // если дробные == 0, заливаем пустой
        for (byte k = n + 1; k < height; k++) {   // всё что сверху заливаем пустыми
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}
