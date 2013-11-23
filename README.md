c.navi.cc
=========

Реализация сервера обмена трекеров на C (прототип)

## Сборка

### Зависимости

    libevent-dev (2.0 or later )

Установка в Ubuntu:

    sudo apt-get install libevent-dev


1. compile and install libevhtp library first

cd libevhtp
cmake . # you need to install cmake program to compile it
make
make install
