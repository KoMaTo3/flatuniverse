function GetObjectsTemplates()
  return {
    {
      name = 'test',
      icon = 'data/temp/brick2.png',
      attr = {
        renderable = true, textureName = 'temp/brick2.png',
        collision = true, _collisionScale = 1, collisionStatic = isStatic,
        trigger = false,
      },
    },
    --
    {
      name = 'brick0',
      icon = 'data/temp/brick0.png',
      attr = {
        renderable = true, textureName = 'temp/brick0.png',
        collision = true, _collisionScale = 1, collisionStatic = isStatic,
        trigger = false,
      },
    },
    --
  }
end