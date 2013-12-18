function GetObjectsTemplates()
  return {
    --
    {
      name = '����� ������ ������',
      icon = 'data/textures/editor/respawn.png',
      attr = {
        lightPoint = true, lightPointSize = 400, lightPointColor = '1 1 1 1',
      },
      animation = { 'editor/respawn', 'default', 'repeat' },
      tags = {
        'respawn',
      },
    },
    --
    {
      name = '������, ������ �������',
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
    },
    --
    {
      name = '����',
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
      name = '����',
      icon = 'data/temp/brick3.png',
      attr = {
        renderable = true, textureName = 'temp/brick3.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
        lightBlockByCollision = true,
      },
    },
    --
    {
      name = '���: ������',
      icon = 'data/temp/enemy0.png',
      attr = {
        renderable = true, textureName = 'temp/enemy0.png',
        collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500', collisionVelocity = '-200 0',
        trigger = false,
        lightBlockByCollision = true,
      },
      creationScript = function( name )
      end,
    },
    -- Tile000a
    {
      name = '��� (�������)',
      icon = 'data/textures/tiles/tile000a.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000a.png',
        collision = false,
        trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { -0.8,-1, 0.8,-1, 0,1 }, _triggerScale = 1,
        lightBlockByCollision = true,
      },
    },
    -- Tile000b
    {
      name = '��� (������)',
      icon = 'data/textures/tiles/tile000b.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000b.png',
        trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { 0.8,1, -0.8,1, 0,-1 }, _triggerScale = 1,
        lightBlockByCollision = true,
      },
    },
    -- Tile000c
    {
      name = '��� (������)',
      icon = 'data/textures/tiles/tile000c.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000c.png',
        trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { 1,-0.8, 1,0.8, -1,0 }, _triggerScale = 1,
        lightBlockByCollision = true,
      },
    },
    -- Tile000d
    {
      name = '��� (�����)',
      icon = 'data/textures/tiles/tile000d.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000d.png',
        trigger = true, _triggerFunc = 'DoKill', _triggerPolygon = { -1,-0.8, 1,0, -1,0.8 }, _triggerScale = 1,
        lightBlockByCollision = true,
      },
    },
    -- Question-brick
    {
      name = '������� �����',
      icon = 'data/textures/tiles/question0.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/question0.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
        lightBlockByCollision = true,
      },
      tags = {
        'has-mushroom',
      },
    },
    -- Question-brick
    {
      name = '���� � ���������',
      icon = 'data/textures/tiles/question0.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/question0.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
        lightBlockByCollision = true,
      },
      tags = {
        'has-coin',
      },
    },
    -- Grass-0
    {
      name = '�����: ������� ������� ����� �����',
      icon = 'data/textures/tiles/grass/grass0-left.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass0-left.png',
      },
    },
    -- Grass-0
    {
      name = '�����: ������� ������� ������� �����',
      icon = 'data/textures/tiles/grass/grass0-middle.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass0-middle.png',
      },
    },
    -- Grass-0
    {
      name = '�����: ������� ������� ������ �����',
      icon = 'data/textures/tiles/grass/grass0-right.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass0-right.png',
      },
    },
    -- Grass-1
    {
      name = '�����: ������� ����� ����� ����',
      icon = 'data/textures/tiles/grass/grass1-left.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass1-left.png',
      },
    },
    -- Grass-1
    {
      name = '�����: ������� ����� ����� 1/2',
      icon = 'data/textures/tiles/grass/grass1-middle.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass1-middle.png',
      },
    },
    -- Grass-1
    {
      name = '�����: ������� ����� ����� 2/2',
      icon = 'data/textures/tiles/grass/grass1-center.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass1-center.png',
      },
    },
    -- Grass-1
    {
      name = '�����: ������� ����� ������ ����',
      icon = 'data/textures/tiles/grass/grass1-right.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass1-right.png',
      },
    },
    -- Grass-1
    {
      name = '�����: ������� ����� ����',
      icon = 'data/textures/tiles/grass/grass1-top.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/grass/grass1-top.png',
      },
    },
    -- Tube0
    {
      name = '�����: �������� �����',
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
      name = '�����: �������� ������',
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
      name = '�����: ����� ����',
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
      name = '�����: ������ ����',
      icon = 'data/textures/tiles/bricks/tube0-right.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/bricks/tube0-right.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
        lightBlockByCollision = true,
      },
    },
    -- Clouds0
    {
      name = '������: ���� ������� �����',
      icon = 'data/textures/tiles/sky/clouds0-tl.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-tl.png',
      },
    },
    -- Clouds0
    {
      name = '������: ���� �������',
      icon = 'data/textures/tiles/sky/clouds0-top.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-top.png',
      },
    },
    -- Clouds0
    {
      name = '������: ���� ������� ������',
      icon = 'data/textures/tiles/sky/clouds0-tr.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-tr.png',
      },
    },
    -- Clouds0
    {
      name = '������: ���� ������ �����',
      icon = 'data/textures/tiles/sky/clouds0-bl.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-bl.png',
      },
    },
    -- Clouds0
    {
      name = '������: ���� ������',
      icon = 'data/textures/tiles/sky/clouds0-bottom.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-bottom.png',
      },
    },
    -- Clouds0
    {
      name = '������: ���� ������ ������',
      icon = 'data/textures/tiles/sky/clouds0-br.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/sky/clouds0-br.png',
      },
    },
    -- Coin0
    {
      name = '���������� �������',
      icon = 'data/textures/tiles/coin0.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/coin0.png',
        trigger = true, _triggerScale = 1, _triggerFunc = 'CoinCollect',
      },
    },
    -- Mushroom0
    {
      name = '����',
      icon = 'data/textures/items/mushroom.png',
      attr = {
        renderable = true, textureName = 'textures/items/mushroom.png',
        collision = true, _collisionScale = 1, collisionStatic = false, collisionAcceleration = '0 1500',
        lightBlockByCollision = true,
      },
    },
    -- ShadowGrass-0
    {
      name = 'Ҹ���� �����',
      icon = 'data/textures/tiles/grass/blackgrass0.png',
      attr = {
        z = 1,
        -- renderable = true, textureName = 'textures/tiles/grass/blackgrass0.png', renderableSize = '64 32',
      },
      animation = { 'grass/0', 'default', 'repeat' },
    },
    -- ShadowGrass-1
    {
      name = 'Ҹ���� ����� (������ ����)',
      icon = 'data/textures/tiles/grass/blackgrass0.png',
      attr = {
        z = 1,
        -- renderable = true, textureName = 'textures/tiles/grass/blackgrass0.png', renderableSize = '64 32',
      },
      animation = { 'grass/0', 'topdown', 'repeat' },
    },
    -- ShadowGrass-2
    {
      name = 'Ҹ���� ����� � �������',
      icon = 'data/textures/tiles/grass/shadowed/grass0.png',
      attr = {
        z = -1,
      },
      animation = { 'grass/shadowed/0', 'default', 'repeat' },
    },
    -- Tree-0
    {
      name = 'Ҹ���� ������',
      icon = 'data/textures/ground/tree0.png',
      attr = {
        z = 1,
      },
      animation = { 'tree/0', 'default', 'repeat' },
    },
    --
  }
end
