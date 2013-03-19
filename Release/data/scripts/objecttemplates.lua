function GetObjectsTemplates()
  return {
    {
      name = 'brick0',
      icon = 'data/temp/brick0.png',
      attr = {
        renderable = true, textureName = 'temp/brick0.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    --
    {
      name = 'brick2',
      icon = 'data/temp/brick2.png',
      attr = {
        renderable = true, textureName = 'temp/brick2.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
      tags = {
        'brick-breakable',
      },
    },
    --
    {
      name = 'brick3',
      icon = 'data/temp/brick3.png',
      attr = {
        renderable = true, textureName = 'temp/brick3.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    --
    {
      name = 'enemy0',
      icon = 'data/temp/enemy0.png',
      attr = {
        renderable = true, textureName = 'temp/enemy0.png',
        collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 300',
        trigger = false,
      },
      creationScript = function( name )
      end,
    },
    -- Tile000
    {
      name = 'tile0/a',
      icon = 'data/textures/tiles/tile000a.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000a.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = 'tile0/b',
      icon = 'data/textures/tiles/tile000b.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000b.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = 'tile0/c',
      icon = 'data/textures/tiles/tile000c.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000c.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = 'tile0/d',
      icon = 'data/textures/tiles/tile000d.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000d.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    --
  }
end