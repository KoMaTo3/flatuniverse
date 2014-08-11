return {
  OnCreate = function( self ) --(
    Debug.Log( 'editor initialized' )
  end, --) OnCreate

  OnKeyPress = function( self, id ) --(
    if settings.gamePaused then
      if id == 0x26 or id == 0x57 then  -- Up
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 8 )
      end
      if id == 0x25 or id == 0x41 then  -- Left
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 4 )
      end
      if id == 0x27 or id == 0x44 then  -- Right
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 1 )
      end
      if id == 0x28 or id == 0x53 then  -- Down
        settings.cameraDirection = bit32.bor( settings.cameraDirection, 2 )
      end
    end
    if id == 0x1B then    -- Esc
      GameExit()
    end
    if id == 0x5A then    -- Z
      if settings.keys.isCtrl then  -- Ctrl+Z
        PopFromBuffer()
      else
        settings.guiVisibility = not settings.guiVisibility
        if not settings.guiVisibility then
          settings.editorMode = 0
        end
        --SetGuiVisibility( settings.guiVisibility )
      end
    end
    if id == 0xC0 then    -- ~
      SetEditorMode( 0 )
    end
    if id == 0x31 then    -- 1
      SetEditorMode( 1 )
    end
    if id == 0x32 then    -- 2
      SetEditorMode( 2 )
    end
    if id == 0x33 then    -- 3
      SetEditorMode( 3 )
    end
    if id == 0x2E then    -- Del
      local objectList = Object.GetSelected() -- GuiGetText( 'editor/object.object_name' )
      for _,object in pairs( objectList ) do --(
        -- buffer
        local name = object.api:GetNameFull()
        local
          x, y, z
          , isRenderable, isCollision, isTrigger, isLightBlockByCollision, isLightPoint
          , setTextureName, renderableSizeX, renderableSizeY, renderablePositionX, renderablePositionY, colorR, colorG, colorB, colorA, renderableScaleX, renderableScaleY, setRenderableRotation
          , collisionSizeX, collisionSizeY, collisionAccelerationX, collisionAccelerationY
          =
          ObjectAttr( name, {
            'position'
            , 'renderable', 'collision', 'trigger', 'lightBlockByCollision', 'lightPoint'
            , 'textureName', 'renderableSize', 'renderablePosition', 'color', 'renderableScale', 'renderableRotation'
            , 'collisionSize', 'collisionAcceleration'
          } )
        local
          triggerType
          , setCollisionStatic, collisionForceX, collisionForceY, collisionVelocityX, collisionVelocityY
          =
          ObjectAttr( name, {
            'triggerType'
            , 'collisionStatic', 'collisionForce', 'collisionVelocity'
          } )
        local setTriggerPolygon = ''
        if triggerType == "polygon" then
          setTriggerPolygon = ObjectAttr( name, { 'triggerPolygon' } )
        end
        PushToBuffer( function()
          end, function()
            ObjectCreate( name, x, y, z )
            ObjectAttr( name, { renderable = isRenderable, collision = isCollision, trigger = isTrigger, lightBlockByCollision = isLightBlockByCollision, lightPoint = isLightPoint } )
            if isRenderable then
              ObjectAttr( name, {
                textureName = setTextureName, renderableSize = renderableSizeX..' '..renderableSizeY, renderablePosition = renderablePositionX..' '..renderablePositionY
                , color = colorR..' '..colorG..' '..colorB..' '..colorA, renderableScale = renderableScaleX..' '..renderableScaleY, renderableRotation = setRenderableRotation
              } )
            end
            if isCollision then
              ObjectAttr( name, {
                collisionSize = collisionSizeX..' '..collisionSizeY, collisionAcceleration = collisionAccelerationX..' '..collisionAccelerationY
                -- , collisionVelocity = collisionVelocityX..' '..collisionVelocityY --, collisionStatic = setCollisionStatic, collisionForce = collisionForceX..' '..collisionForceY
              } )
            end
            if isTrigger then
              if triggerType == "polygon" then
                ObjectAttr( name, { triggerPolygon = setTriggerPolygon } )
              end
            end
          end
        )
        --
        ObjectRemove( name )
      end --)
      Object.Select( nil )
      if settings.editorMode == 2 then
        settings.editorMode = 0
      end
      UpdateGuiBySelectedObject()
    end
    if id == 0x20 then    -- Space
      Object.Select( nil )
      UpdateGuiBySelectedObject()
      if settings.editorMode == 2 then
        settings.editorMode = 0
      end
      GUI.tabbedTemplates.currentItem = 0
    end
    if id == 0x47 then    -- G: toggle showing grid
      ToggleGrid()
    end
    if id == 0x0D then    -- Enter
      DoPause( not settings.gamePaused )
    end
    if id == 0x72 then    -- F3
      settings.gamePaused = true
      settings.editorMode = 0
      settings.buffer = {}
      settings.keys.isShift = false
      settings.keys.isCtrl = false
      settings.keys.isAlt = false
    end
    if id == 0x10 then    -- Shift
      settings.keys.isShift = true
    end
    if id == 0x11 then    -- Control
      settings.keys.isCtrl = true
    end
    if id == 0x12 then    -- Alt
      settings.keys.isAlt = true
    end
  end, --) OnKeyPress

  OnKeyRelease = function( self, id ) --(
    if settings.gamePaused then
      if id == 0x26 or id == 0x57 then  -- Up
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 8 ) )
      end
      if id == 0x25 or id == 0x41 then  -- Left
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 4 ) )
      end
      if id == 0x27 or id == 0x44 then  -- Right
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 1 ) )
      end
      if id == 0x28 or id == 0x53 then  -- Down
        settings.cameraDirection = bit32.band( settings.cameraDirection, bit32.bnot( 2 ) )
      end
    end
    if id == 0x10 then    -- Shift
      settings.keys.isShift = false
    end
    if id == 0x11 then    -- Control
      settings.keys.isCtrl = false
    end
    if id == 0x12 then    -- Alt
      settings.keys.isAlt = false
    end
  end, --) OnKeyRelease
}
