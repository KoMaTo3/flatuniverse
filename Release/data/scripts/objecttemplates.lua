function GetObjectsTemplates()
  return {
    --
    {
      name = '������, ������ �������',
      icon = 'data/textures/tiles/bricks/brick0.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/bricks/brick0.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
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
      },
    },
    --
    {
      name = '���: ������',
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
      name = '����: �������� ������� 1/4',
      icon = 'data/textures/tiles/tile000a.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000a.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = '����: �������� ������� 2/4',
      icon = 'data/textures/tiles/tile000b.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000b.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = '����: �������� ������� 3/4',
      icon = 'data/textures/tiles/tile000c.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000c.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Tile000
    {
      name = '����: �������� ������� 4/4',
      icon = 'data/textures/tiles/tile000d.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/tile000d.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
    },
    -- Question-brick
    {
      name = '����-������� ������� ����� ��� ����� �������',
      icon = 'data/textures/tiles/question0.png',
      attr = {
        renderable = true, textureName = 'textures/tiles/question0.png',
        collision = true, _collisionScale = 1, collisionStatic = true,
        trigger = false,
      },
      tags = {
        'has-mushroom',
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
      },
    },
    --
  }
end
