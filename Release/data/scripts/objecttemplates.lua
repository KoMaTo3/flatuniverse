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
    --
  }
end