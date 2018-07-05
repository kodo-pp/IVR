# ModBox [![Build Status](https://travis-ci.com/kodo-pp/ModBox.svg?branch=master)](https://travis-ci.com/kodo-pp/ModBox)

Моя ИВР

![ModBox](/github/images/logo.png)

## Примечание по объектным и классовым диаграммам
Эти диаграммы хранятся в каталоге `diagrams.dir`. Файл `Корзун_диаграммы.zip` содержит те же самые файлы, но запакованные в ZIP-архив. Эти диаграммы также собраны в файл `Корзун_диаграммы.pdf`.

## Использованные библиотеки
- Графический движок [Irrlicht](https://sourceforge.net/projects/irrlicht)

- Библиотеки, использыемые Irrlicht (код Independent JPEG Group (jpeglib), libpng и zlib)

- Для модуля использовал NetCat-класс [отсюда](https://gist.github.com/leonjza/f35a7252babdf77c8421), немного модифицировал его

## Примечание по первому показу кода
Всё в каталоге Project/, там же и файл
README.md, более подробно посвящённый
этому.

## Сборка и запуск
- Сборка: `./build.sh`

- Сборка для последующей отладки: `MODBOX_DEBUG=yes ./build.sh`

- Запуск: `./run.sh`

- Сборка и запуск: `./run.sh -f`

- Удаление собранных файлов: `./clean.sh`
