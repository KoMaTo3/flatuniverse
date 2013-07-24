flatuniverse
============

OpenGL 2D engine

Latest release 2013.07.24: https://dl.dropboxusercontent.com/u/71135358/flatuniverse-20130724.tar.gz

Последние изменения:
- изменена физика - теперь скорость при падении не растёт до бесконечности
- полигональные триггеры (шипы)
- добавлен дабл-джамп и подкорректирована физика движения игрока
- хоткеи загрузки и сохранения мира: F2 - сохранить, F3 - загрузить, F9 - очистить
- BackSpace - откат позиции и скорости персонажа в 0:0
- Enter - пауза
- реализация анимации объектов. Связка с Lua.
- Пример анимации персонажа-Марио - установка текстуры, текстурных координат, цвета, поворота, позиции, а так же пример составной анимации - помимо самого Марио за ним летает "квадрат"


Управление:
- игрок: стрелки, A-W-D
- G: вкл/выкл сетку
- ~, 1, 2, 3: переключение режима редактора
- Q, E: переключение текущего шаблона
- Space: отмена выделения
- Ctrl+Z: отмена последнего действия (очень ограничено, в процессе доработки)
- BackSpace: откат позиции и скорости персонажа в 0:0
- Enter: переключение активности игры (пока только вкл/выкл гравитации игрока)
- F2 - сохранить мир
- F3 - загрузить мир
- F9 - очистить мир

Известные проблемы:
- не сохраняется состояние скриптов (после перезапуска "живой" объект становится болванкой)
