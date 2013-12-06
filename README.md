flatuniverse
============

OpenGL 2D engine

Latest release 2013.12.06: https://dl.dropboxusercontent.com/u/71135358/flatuniverse-20131206.tar.gz
Версия мира: 0x000B

Последние изменения:
- зарелизена система освещения, марио стал в стиле "нуар", ура
- тестовая стрельба светящимися монетками, кнопка X
- в старых сохранениях необходимо вручную пометить блоки как "LightBlockByCollision", тобишь, отбрасывающие тень на основе формы коллизии
- галочка "LightBlock" делает объект источником света радиуса 400 рандомного цвета (в скриптах всё поправимо)
- уровень глобального освещения пока никак не сохраняется, только в реалтайме настраивается в редакторе


Управление:
- игрок: стрелки, A-W-D, X - стрельба
- G: вкл/выкл сетку
- ~, 1, 2, 3: переключение режима редактора
- Q, E: переключение текущего шаблона
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
- если упасть, могут быть загружены нижние гриды, в которых в огромных количествах накопился всякий трэш, падающий с уровня, вплоть до краша если версия грида крайне старая... такие вот дела
