// структура для хранения данных
struct Data {
  char val = 'H';
  char str[15] = "ello kitty!";
};
Data data;  // переменная, с которой мы работаем в программе

#include <EEManager.h>  // подключаем либу
EEManager memory(data); // передаём нашу переменную (фактически её адрес)

// можно задать таймаут обновления, по умолч. 5 секунд (5000мс)
//EEManager memory(data, 1000);

void setup() {
  Serial.begin(9600);
  // для esp8266 не забудь вызвать EEPROM.begin(размер)!
  // EEPROM.begin(memory.blockSize());
  /*
    Запускаем менеджер, передаём:
    - Стартовый адрес в памяти для записи даты
    - Ключ хранения (0.. 255) или символ
  */
  EEManager::Status status = memory.begin(0, 'b');

  /*
    Коды возврата:
    EEManager::OK - данные прочитаны из EEPROM
    EEManager::FIRST_RUN - стандартные данные записаны в EEPROM
    EEManager::NO_SPACE - в EEPROM не хватает места
  */
  Serial.println(status);

  // выведем в порт всё содержимое блока памяти, включая ключ
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();
  
  data.val = 'X';     // изменим данные в переменной
  memory.update();    // отложим обновление

  // примечание: если нужно создать ещё один блок следующим в памяти -
  // запускаем его на адресе memory.endAddr() + 1, это первый
  // свободный байт после первого блока
}

void loop() {
  // в лупе вызываем tick(), в нём по таймеру произойдёт обновление
  // функция вернёт true, когда это случится
  if (memory.tick()) Serial.println("Updated!");

  // выведем в порт всё содержимое блока памяти, включая ключ
  for (uint16_t i = memory.startAddr(); i < memory.endAddr() + 1; i++)
    Serial.write(EEPROM.read(i));
  Serial.println();

  // таймер сброса на 10 секунд: через 10 секунд
  // произойдёт сброс ключа данных, и при следующем запуске
  // в епром будут записаны данные по умолчанию (то что в ярлыке структуры)
  static uint32_t tmr;
  if (millis() - tmr >= 10000) {
    memory.reset();
    Serial.println("reset");
    for (;;);
  }
  
  delay(1000);    // чилим
}
