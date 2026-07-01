/*
    Менеджер EEPROM - библиотека для уменьшения износа памяти
    Документация:
    GitHub: https://github.com/GyverLibs/EEManager
    Возможности:
    - Отложенная запись (обновление) по таймеру
    - Работает на базе стандартной EEPROM.h
    - Встроенный механизм ключа первой записи

    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License

    Версии:
    v1.0 - релиз
    v1.1 - изменены коды возврата begin
    v1.2 - добавлена nextAddr()
    v1.2.1 - поддержка esp32
    v1.2.2 - пофиксил варнинг
    v1.3 - исправлен критический баг с адресацией, добавлен макрос EEBlock
    v1.4 - ещё больше поддержки esp8266/32
    v2.0 - ключ запуска перенесён в начало блока для удобства. Обновление библиотеки сбросит данные в EEPROM!
*/

#ifndef _EEManager_h
#define _EEManager_h
#include <Arduino.h>
#include <EEPROM.h>

#define EEBlock(x) (sizeof(x) + 1)

class EEManager {
   public:
    enum Status : uint8_t {
        OK,
        FIRST_RUN,
        NO_SPACE,
    };

    // передать данные любого типа, опционально таймаут обновления в мс
    template <typename T>
    EEManager(T& data, uint16_t tout = 5000) {
        _data = (uint8_t*)&data;
        _size = sizeof(T);
        _tout = tout;
    }

    // передать данные любого типа и их размер, опционально таймаут обновления в мс
    EEManager(void* data, uint16_t size, uint16_t tout = 5000) {
        _data = (uint8_t*)data;
        _size = size;
        _tout = tout;
    }

    // сменить таймаут
    void setTimeout(uint16_t tout = 5000) {
        _tout = tout;
    }

    // начать работу, прочитать данные в переменную. Принимает адрес начала хранения даты и ключ
    Status begin(uint16_t addr, uint8_t key) {
        _ready = false;
        _update = false;
        uint32_t next = (uint32_t)addr + 1 + _size;
        if (next > (uint32_t)EEPROM.length() || next > UINT16_MAX) return NO_SPACE;

        _addr = addr + 1;  // данные начнутся со следующего адреса
        _ready = true;                                         // EEPROM запущен и размера хватит
        if (EEPROM.read(keyAddr()) != key) {                   // ключ не совпал
            EEPROM.write(keyAddr(), key);                      // пишем ключ
            updateNow();                                       // пишем стандартные значения
            return FIRST_RUN;
        }
        for (uint16_t i = 0; i < _size; i++) _data[i] = EEPROM.read(_addr + i);
        return OK;
    }

    // обновить данные в еепром сейчас
    void updateNow() {
        _update = false;
        if (_ready) {
#if defined(ESP8266) || defined(ESP32)
            for (uint16_t i = 0; i < _size; i++) EEPROM.write(_addr + i, _data[i]);
            EEPROM.commit();
#else
            for (uint16_t i = 0; i < _size; i++) EEPROM.update(_addr + i, _data[i]);
#endif
        }
    }

    // отложить обновление и сбросить таймер
    void update() {
        if (_ready) {
            _tmr = millis();
            _update = true;
        }
    }

    // отменить отложенное обновление
    void stop() {
        _update = false;
    }

    // тикер обновления
    bool tick() {
        if (_ready && _update && (uint16_t)((uint16_t)millis() - _tmr) >= _tout) {
            updateNow();
            return true;
        }
        return false;
    }

    // сбросить ключ запуска. При перезагрузке (или вызове begin) запишутся стандартные данные
    void reset() {
        if (!_ready) return;
        EEPROM.write(keyAddr(), EEPROM.read(keyAddr()) + 1);  // меняем ключ на +1, при перезапуске будет дефолт
#if defined(ESP8266) || defined(ESP32)
        EEPROM.commit();
#endif
    }

    // получить размер данных
    uint16_t dataSize() {
        return _size;
    }

    // получить размер всего блока (данные + ключ)
    uint16_t blockSize() {
        return _size + 1;
    }

    // получить адрес ключа
    uint16_t keyAddr() {
        return _addr - 1;
    }

    // получить адрес первого байта данных в блоке
    uint16_t startAddr() {
        return _addr;
    }

    // получить адрес последнего байта в блоке
    uint16_t endAddr() {
        return _addr + _size - 1;
    }

    // получить первый свободный адрес для следующего блока
    uint16_t nextAddr() {
        return _addr + _size;
    }

   private:
    uint8_t* _data;
    uint16_t _size, _addr = 0;
    uint16_t _tmr = 0;
    uint16_t _tout;
    bool _ready = false, _update = false;
};

#endif
