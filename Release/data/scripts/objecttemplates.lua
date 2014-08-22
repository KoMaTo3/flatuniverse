function GetObjectsTabbedTemplates()  --(
  return {
    blocks = {
      --
      {
        name = 'Кирпич, бьётся головой',
        icon = 'data/textures/tiles/bricks/brick0.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/brick0.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          lightBlockByCollision = true,
          -- trigger = true, _triggerFunc = 'TestFunc', triggerPolygon = '-12 -12  12 -12  0 12'
          -- trigger = true, _triggerFunc = 'TestFunc', _triggerPolygon = { -1,-1, 1,-1, 0,1 }, _triggerScale = 1,
        },
        tags = {
          'brick-breakable',
        },
        templateScriptName = 'data/scripts/objects/brick-0.lua',
      },
      --
      {
        name = 'Блок',
        icon = 'data/textures/tiles/bricks/brick1.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/brick1.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      --
      {
        name = 'Блок',
        icon = 'data/temp/brick3.png',
        attr = {
          renderable = true, textureName = 'temp/brick3.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      -- Question-brick
      {
        name = 'Спавнит грибы',
        icon = 'data/textures/tiles/question0.png',
        attr = {
          -- renderable = true, textureName = 'textures/tiles/question0.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightPoint = true, lightPointSize = 200, lightPointColor = '1 1 1 1',
          z = 1,
        },
        tags = {
          'has-mushroom',
        },
        animation = { 'brick/question/0', 'default', 'repeat' },
        templateScriptName = 'data/scripts/objects/brick-1.lua',
      },
      -- Question-brick
      {
        name = 'Блок с монетками',
        icon = 'data/textures/tiles/question0.png',
        attr = {
          -- renderable = true, textureName = 'textures/tiles/question0.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightPoint = true, lightPointSize = 200, lightPointColor = '1 1 1 1',
          z = 1,
        },
        tags = {
          'has-coin',
        },
        animation = { 'brick/question/0', 'default', 'repeat' },
        templateScriptName = 'data/scripts/objects/brick-1.lua',
      },
      -- Tube0
      {
        name = 'Труба: верхушка левая',
        icon = 'data/textures/tiles/bricks/tube0-tl.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/tube0-tl.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      -- Tube0
      {
        name = 'Труба: верхушка правая',
        icon = 'data/textures/tiles/bricks/tube0-tr.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/tube0-tr.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      -- Tube0
      {
        name = 'Труба: левый тайл',
        icon = 'data/textures/tiles/bricks/tube0-left.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/tube0-left.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      -- Tube0
      {
        name = 'Труба: правый тайл',
        icon = 'data/textures/tiles/bricks/tube0-right.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/bricks/tube0-right.png',
          collision = true, _collisionScale = 1, collisionStatic = true,
          trigger = false,
          lightBlockByCollision = true,
        },
      },
      --
      {
        name = 'Триггер',
        icon = 'data/temp/null.png',
        attr = {
          renderable = true, textureName = 'temp/null.png',
          collision = false,
          -- trigger = true, _triggerFunc = 'TestFunc', triggerPolygon = '-12 -12  12 -12  0 12'
          trigger = true, _triggerScale = 1,
        },
        templateScriptName = 'data/scripts/objects/trigger-jump.lua',
      },
      --
    },  -- blocks
    grass = {
      -- Grass-0
      {
        name = 'Трава: фоновая светлая левая часть',
        icon = 'data/textures/tiles/grass/grass0-left.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass0-left.png',
        },
      },
      -- Grass-0
      {
        name = 'Трава: фоновая светлая средняя часть',
        icon = 'data/textures/tiles/grass/grass0-middle.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass0-middle.png',
        },
      },
      -- Grass-0
      {
        name = 'Трава: фоновая светлая правая часть',
        icon = 'data/textures/tiles/grass/grass0-right.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass0-right.png',
        },
      },
      -- Grass-1
      {
        name = 'Трава: фоновая тёмная левый угол',
        icon = 'data/textures/tiles/grass/grass1-left.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass1-left.png',
        },
      },
      -- Grass-1
      {
        name = 'Трава: фоновая тёмная центр 1/2',
        icon = 'data/textures/tiles/grass/grass1-middle.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass1-middle.png',
        },
      },
      -- Grass-1
      {
        name = 'Трава: фоновая тёмная центр 2/2',
        icon = 'data/textures/tiles/grass/grass1-center.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass1-center.png',
        },
      },
      -- Grass-1
      {
        name = 'Трава: фоновая тёмная правый угол',
        icon = 'data/textures/tiles/grass/grass1-right.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass1-right.png',
        },
      },
      -- Grass-1
      {
        name = 'Трава: фоновая тёмная верх',
        icon = 'data/textures/tiles/grass/grass1-top.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/grass/grass1-top.png',
        },
      },
      -- ShadowGrass-0
      {
        name = 'Тёмная трава',
        icon = 'data/textures/tiles/grass/blackgrass0.png',
        attr = {
          z = 1,
          -- renderable = true, textureName = 'textures/tiles/grass/blackgrass0.png', renderableSize = '64 32',
        },
        animation = { 'grass/0', 'default', 'repeat' },
      },
      -- ShadowGrass-1
      {
        name = 'Тёмная трава (сверху вниз)',
        icon = 'data/textures/tiles/grass/blackgrass0.png',
        attr = {
          z = 1,
          -- renderable = true, textureName = 'textures/tiles/grass/blackgrass0.png', renderableSize = '64 32',
        },
        animation = { 'grass/0', 'topdown', 'repeat' },
      },
      -- ShadowGrass-2
      {
        name = 'Тёмная трава с глазами',
        icon = 'data/textures/tiles/grass/shadowed/grass0.png',
        attr = {
          z = -1,
        },
        animation = { 'grass/shadowed/0', 'default', 'repeat' },
      },
      -- Tree-0
      {
        name = 'Тёмное дерево',
        icon = 'data/textures/ground/tree0.png',
        attr = {
          z = 1,
        },
        animation = { 'tree/0', 'default', 'repeat' },
      },
      --
    },  -- grass
    sky = {
      -- Clouds0
      {
        name = 'Облако: угол верхний левый',
        icon = 'data/textures/tiles/sky/clouds0-tl.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-tl.png',
        },
      },
      -- Clouds0
      {
        name = 'Облако: тайл верхний',
        icon = 'data/textures/tiles/sky/clouds0-top.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-top.png',
        },
      },
      -- Clouds0
      {
        name = 'Облако: угол верхний правый',
        icon = 'data/textures/tiles/sky/clouds0-tr.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-tr.png',
        },
      },
      -- Clouds0
      {
        name = 'Облако: угол нижний левый',
        icon = 'data/textures/tiles/sky/clouds0-bl.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-bl.png',
        },
      },
      -- Clouds0
      {
        name = 'Облако: тайл нижний',
        icon = 'data/textures/tiles/sky/clouds0-bottom.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-bottom.png',
        },
      },
      -- Clouds0
      {
        name = 'Облако: угол нижний правый',
        icon = 'data/textures/tiles/sky/clouds0-br.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/sky/clouds0-br.png',
        },
      },
    },  -- sky
    light = {
    },  -- light
    go = {
      -- Tile000a
      {
        name = 'Шип (верхний)',
        icon = 'data/textures/tiles/tile000a.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/tile000a.png',
          collision = false,
          -- trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { -0.8,-1, 0.8,-1, 0,1 }, _triggerScale = 1,
          lightBlockByCollision = true,
        },
        tags = {
          'kill',
        },
      },
      -- Tile000b
      {
        name = 'Шип (нижний)',
        icon = 'data/textures/tiles/tile000b.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/tile000b.png',
          -- trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { 0.8,1, -0.8,1, 0,-1 }, _triggerScale = 1,
          lightBlockByCollision = true,
        },
        tags = {
          'kill',
        },
      },
      -- Tile000c
      {
        name = 'Шип (правый)',
        icon = 'data/textures/tiles/tile000c.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/tile000c.png',
          -- trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { 1,-0.8, 1,0.8, -1,0 }, _triggerScale = 1,
          lightBlockByCollision = true,
        },
        tags = {
          'kill',
        },
      },
      -- Tile000d
      {
        name = 'Шип (левый)',
        icon = 'data/textures/tiles/tile000d.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/tile000d.png',
          -- trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { -1,-0.8, 1,0, -1,0.8 }, _triggerScale = 1,
          lightBlockByCollision = true,
        },
        tags = {
          'kill',
        },
      },
      -- Coin0
      {
        name = 'Собираемая монетка',
        icon = 'data/textures/tiles/coin0.png',
        attr = {
          renderable = true, textureName = 'textures/tiles/coin0.png',
          -- trigger = true, _triggerScale = 1, _triggerFunc = 'CoinCollect',
        },
		animation = { 'supermario/coin', 'default', 'repeat' },
      },
      -- Mushroom0
      {
        name = 'Гриб',
        icon = 'data/textures/items/mushroom.png',
        attr = {
          renderable = true, textureName = 'textures/items/mushroom.png',
          collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500',
          -- lightBlockByCollision = true,
        },
      },
      --
    },  -- go
    npc = {
      --
      {
        name = 'Моб: Goomba',
        icon = 'data/temp/enemy0.png',
        attr = {
          renderable = true, textureName = 'temp/enemy0.png',
          collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500', collisionVelocity = '-50 0',
          trigger = false,
        },
        creationScript = function( name )
        end,
        templateScriptName = 'data/scripts/objects/goomba.lua',
        animation = { 'goomba/0', 'default', 'repeat' },
      },
      --
      {
        name = 'Моб: ворона (прыгучая)',
        icon = 'data/temp/enemy0.png',
        attr = {
          renderable = true, textureName = 'temp/enemy0.png',
          collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500', collisionVelocity = '0 0',
          trigger = false,
        },
        creationScript = function( name )
        end,
        templateScriptName = 'data/scripts/objects/crow-jump.lua',
        animation = { 'crow/0', 'default', 'repeat' },
      },
      --
      {
        name = 'Моб: ворона (убегающая)',
        icon = 'data/temp/enemy0.png',
        attr = {
          renderable = true, textureName = 'temp/enemy0.png',
          collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500', collisionVelocity = '0 0',
          trigger = false,
        },
        creationScript = function( name )
        end,
        templateScriptName = 'data/scripts/objects/crow-flee.lua',
        animation = { 'crow/0', 'default', 'repeat' },
      },
      --
    },  -- npc
    other = {
      --
      {
        name = 'Точка спавна игрока',
        icon = 'data/textures/editor/respawn.png',
        attr = {
          lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1',
          trigger = true, _triggerScale = 1,
        },
        animation = { 'editor/respawn', 'default', 'repeat' },
        tags = {
          'respawn',
        },
        templateScriptName = 'data/scripts/objects/respawn.lua',
      },
      --
    },  -- other
    pc = {
      --
      {
        name = 'Super Mario',
        icon = 'data/textures/player/mario/mario-icon.png',
        attr = {
          -- lockToDelete = true,
          collision = true, collisionSize = '14 20', collisionStatic = false, collisionAcceleration = '0 1500', collisionVelocity = '0 0',
          lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1',
        },
        animation = { 'player/mario', 'stay-right', 'repeat' },
        tags = {
          'player',
        },
        templateScriptName = 'data/scripts/objects/player.lua',
        isActiveObject = true,
        creationScript = function( name )
        end,
      },
      --
    }, -- pc
  }
end --)
