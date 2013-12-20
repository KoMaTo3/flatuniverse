flatuniverse
============

OpenGL 2D engine

Особенности движка:
- динамично-подгружаемый мир. Весь мир поделён на сетку (grid), по мере необходимости подгружаются/выгружаются соответствующие области;
- подгрузка ресурсов игры в отдельном потоке (на данный момент только текстуры);
- вся игровая логика и редактор вынесены в Lua-скрипты.

Latest release 2013.12.19: https://dl.dropboxusercontent.com/u/71135358/flatuniverse-20131219.tar.gz

Версия мира: 0x000B

Последние изменения:
- !!!изменен файл конфигурации, со старым работать не будет
- переделано окно выбора шаблонов объектов: теперь оно побито на закладки, есть возможность его двигать и сворачивать
- зарелизена система освещения, марио стал в стиле "нуар", ура;
- тестовая стрельба светящимися монетками, кнопка X;
- в старых сохранениях необходимо вручную пометить блоки как "LightBlockByCollision", тобишь, отбрасывающие тень на основе формы коллизии;
- галочка "LightBlock" делает объект источником света радиуса 400 рандомного цвета (в скриптах всё поправимо);
- уровень глобального освещения пока никак не сохраняется, только в реалтайме настраивается в редакторе;
- в редакторе появились элементы-слайдеры для выбора значений из диапазона (на примере ползунков R-G-B-A глобального освещения);
- фоновая выгрузка и подгрузка гридов. Факт этих действий можно наблюдать по изменяющемуся числу квадов, отображаемое в заголовке окна;
- добавлена работа с потоками, конкретно - вынесена загрузка текстур; приложение теперь запускается быстрее, но первое время видны пустые "текстуры"-болванки (для теста).


Управление:
- игрок: стрелки, A-W-D, X - стрельба
- G: вкл/выкл сетку
- ~, 1, 2, 3: переключение режима редактора
- Space: отмена выделения
- Ctrl+Z: отмена последнего действия (очень ограничено, в процессе доработки)
- Z: вкл/выкл интерфейс редактора
- BackSpace: откат позиции и скорости персонажа в 0:0
- Enter: переключение активности игры (пока только вкл/выкл гравитации игрока)
- F2 - сохранить мир
- F3 - загрузить мир
- F9 - очистить мир

Известные проблемы:
- не сохраняется состояние скриптов (после перезапуска "живой" объект становится болванкой)
- если упасть, могут быть загружены нижние гриды, в которых в огромных количествах накопился всякий трэш, падающий с уровня, вплоть до краша если версия грида крайне старая или сохранённая с возможными на то время ошибками... такие вот дела.
