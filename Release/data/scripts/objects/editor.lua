return {
  -- mode:
  -- 0: none, default
  -- 1: нажал ЛКМ и смотрим, будет ли двигать мышью
  -- 2: выбран объект
  -- 3: перемещение выбранного объекта
  -- 10: mouseover на GUI.templates
  -- 11: перемещение GUI.templates
  -- 12: скролл GUI.templates
  -- 20: промежуточный режим вставки тайла: нажал мышь и ждём отпускания или движения
  -- 21: вставка блока тайлов: рисование прямоугольной области
  -- 22: мульти-селект (рисование рамки)
  -- 30: mouseover на GUI.tabbedTemplates
  -- 31: перемещение GUI.tabbedTemplates
  -- 32: скролл GUI.tabbedTemplates
  -- 50: GUIRenderer.guiFocused
  mode = {
    [0] = function( self, id, isPressed ) --(
      if isPressed then --(
        local doMultiSelect = false
        if settings.editorType == 0 then --(
          -- if GUI.templates.currentItem > 0 then --
          if GUI.tabbedTemplates.currentItem > 0 then --(
            settings.editorMode = 20
            settings.move.mouseStart.x = mousePos.x
            settings.move.mouseStart.y = mousePos.y
            Object.Select( nil )
            self:UpdateGuiBySelectedObject()
          else --)(
            doMultiSelect = true
          end --)
          --[[
        else
          local object = GetObjectUnderCursorByMode()
          if #object > 0 then
              SelectObject( object )
              self:UpdateGuiBySelectedObject()
              settings.editorMode = 1
          else
            settings.editorMode = 1
              -- multiselect?
          end
          ]]
        else --)(
          doMultiSelect = true
        end --)
        if doMultiSelect then
          settings.editorMode = 22
          local cx, cy = Camera.GetPos()
          settings.multiSelectStartPoint.x = mousePos.x + cx
          settings.multiSelectStartPoint.y = mousePos.y + cy
        end
      else --)( released
      end --)
      return false
    end, --)
    [1] = function( self, id, isPressed ) --(
      if isPressed then
      else
        local objects = Object.GetSelected()
        if TableSize( objects ) > 0 then
          settings.editorMode = 2
        else
          settings.editorMode = 0
        end
      end
      return false
    end, --)
    [2] = function( self, id, isPressed ) --(
      if isPressed then
        if settings.keys.isCtrl --[[ or settings.keys.isAlt ]] then --(
          settings.editorMode = 22
          local cx, cy = Camera.GetPos()
          settings.multiSelectStartPoint.x = mousePos.x + cx
          settings.multiSelectStartPoint.y = mousePos.y + cy
        else --)(
          settings.editorMode = 3
          settings.objectMode3Moved = false
        end --)
      else
        local objects = GetObjectUnderCursorByMode()
        if TableSize( objects ) > 0 then
          Object.Select( objects )
          self:UpdateGuiBySelectedObject()
        end
      end
      return false
    end, --)
    [3] = function( self, id, isPressed ) --(
      if isPressed then
      else
        if settings.objectMode3Moved then
          settings.editorMode = 2
        else
          local objects = GetObjectUnderCursorByMode()
          Object.Select( objects )
          self:HideSelectedInterface()
          self:UpdateGuiBySelectedObject()
          if TableSize( objects ) > 0 then
            settings.editorMode = 2
          else
            settings.editorMode = 0
          end
        end
      end
      return false
    end, --)
    [10] = function( self, id, isPressed ) --(
      if isPressed then
        if mousePos.x >= GUI.templates.x and
           mousePos.x <= GUI.templates.x + GUI.templates.width and
           mousePos.y >= GUI.templates.y and
           mousePos.y <= GUI.templates.y + 15
           then
          settings.editorMode = 11
          GUI.templates.moving.x = mousePos.x
          GUI.templates.moving.y = mousePos.y
        elseif mousePos.x >= GUI.templates.x + GUI.templates.width - 15 and
          mousePos.x <= GUI.templates.x + GUI.templates.width and
          mousePos.y >= GUI.templates.y + 15 and
          mousePos.y <= GUI.templates.y + GUI.templates.height then
            settings.editorMode = 12
            TestMouseOnGUI( mousePos.x, mousePos.y )
        else
          local num = math.floor( ( mousePos.y - GUI.templates.y - 15 + GUI.templates.scroll * GUI.templates.maxScroll ) / ( GUI.templates.itemSize + 5 ) ) + 1
          if num > 0 and num <= #GUI.templates.items then
            self:DoPause( true )
            GUI.templates.currentItem = num
            -- Debug.RenderState( 0 )
            settings.editorType = 0
            -- Object.Select( nil )
            -- self:UpdateGuiBySelectedObject()
          end
        end
      else
      end
      return false
    end, --)
    [11] = function( self, id, isPressed ) --(
      if isPressed then
      else
        settings.editorMode = 10
      end
      return false
    end, --)
    [12] = function( self, id, isPressed ) --(
      if isPressed then
      else
        if settings.editorMode == 12 then
          settings.editorMode = 10
        end
      end
      return false
    end, --)
    [20] = function( self, id, isPressed ) --(
      settings.editorMode = 0
      local objects = {}
      table.insert( objects, EditorInsertItemByTemplate( mousePos.x, mousePos.y ) )
      Object.Select( objects )
      self:UpdateGuiBySelectedObject()
      PushToBuffer( function()
        end, function()
          for _,object in pairs( objects ) do
            object.api:Destroy()
          end
        end
      )
      return false
    end, --)
    [21] = function( self, id, isPressed ) --(
      if isPressed then
      else
        settings.editorMode = 0
        local cx, cy = Camera.GetPos()
        local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
        local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
        local newX = mousePos.x
        local newY = mousePos.y
        local left   = x < newX and x or newX
        local right  = x < newX and newX or x
        local top    = y < newY and y or newY
        local bottom = y < newY and newY or y
        left, top = GetTilePosByPixel( left, top )
        right, bottom = GetTilePosByPixel( right, bottom )
        -- local texture = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].icon
        local names = {}
        local objects = {}
        for tx = left, right do
        for ty = top, bottom do
          x, y = GetPixelByTile( tx, ty )
          local obj = EditorInsertItemByTemplate( x, y )
          table.insert( objects, obj )
          table.insert( names, obj.api:GetNameFull() )
        end
        end
        Object.Select( objects )
        self:UpdateGuiBySelectedObject()
        PushToBuffer( function()
          end, function()
            for _,name in pairs( names ) do
              Object.Get( name ).api:Destroy()
            end
          end
        )
      end
      return false
    end, --) 21
    [22] = function( self, id, isPressed ) --(
      if isPressed then --(
      else --)(
        local cx, cy = Camera.GetPos()
        local newX = mousePos.x + cx
        local newY = mousePos.y + cy
        if settings.multiSelectStartPoint.x == newX and settings.multiSelectStartPoint.y == newY then --( one-select
          local objects = GetObjectUnderCursorByMode()
          if TableSize( objects ) > 0 then --(
            if settings.keys.isCtrl then  --( add
              local oldObjects = Object.GetSelected()
              if( TableSize( oldObjects ) > 0 ) then
                objects = AddObjectsToList( oldObjects, objects )
              end
            end --)
            if settings.keys.isAlt then --( exclude
              local oldObjects = Object.GetSelected()
              if( TableSize( oldObjects ) > 0 ) then
                objects = RemoveObjectsFromList( oldObjects, objects )
              end
            end --)
            Object.Select( objects )
            settings.editorMode = 2
          else --)(
            settings.editorMode = 0
          end --)
        else --)( multi-select
          local objects = SelectObjectsInRectangle( settings.multiSelectStartPoint.x, settings.multiSelectStartPoint.y, newX, newY, settings.editorType )
          if TableSize( objects ) > 0 then
            settings.editorMode = 2
          else
            settings.editorMode = 0
          end
        end --)
        self:UpdateGuiBySelectedObject()
      end --)
      return false
    end, --) 22
    [30] = function( self, id, isPressed ) --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
      return false
    end,  --) 30
    [31] = function( self, id, isPressed ) --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
      return false
    end,  --) 31
    [32] = function( self, id, isPressed ) --(
      GUI.tabbedTemplates.OnMouseClick( id, isPressed )
      return false
    end,  --) 32
  }, -- mode


  OnCreate = function( self ) --(
    self.selectedInterface = false
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
      Core.GameExit()
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
    if id == 0x2E then --( Del
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
          object.api:Attr({
            'position'
            , 'renderable', 'collision', 'trigger', 'lightBlockByCollision', 'lightPoint'
            , 'textureName', 'renderableSize', 'renderablePosition', 'color', 'renderableScale', 'renderableRotation'
            , 'collisionSize', 'collisionAcceleration'
          })
        local
          triggerType
          , setCollisionStatic, collisionForceX, collisionForceY, collisionVelocityX, collisionVelocityY
          =
          object.api:Attr({
            'triggerType'
            , 'collisionStatic', 'collisionForce', 'collisionVelocity'
          })
        local setTriggerPolygon = ''
        if triggerType == "polygon" then
          setTriggerPolygon = object.api:Attr({ 'triggerPolygon' })
        end
        PushToBuffer( function()
          end, function()
            -- ObjectCreate( name, x, y, z )
            local newObject = Object.New( name )
            newObject.api:SetPos( x, y, z )
            newObject.api:Attr({ renderable = isRenderable, collision = isCollision, trigger = isTrigger, lightBlockByCollision = isLightBlockByCollision, lightPoint = isLightPoint })
            if isRenderable then
              newObject.api:Attr({
                textureName = setTextureName, renderableSize = renderableSizeX..' '..renderableSizeY, renderablePosition = renderablePositionX..' '..renderablePositionY
                , color = colorR..' '..colorG..' '..colorB..' '..colorA, renderableScale = renderableScaleX..' '..renderableScaleY, renderableRotation = setRenderableRotation
              })
            end
            if isCollision then
              newObject.api:Attr({
                collisionSize = collisionSizeX..' '..collisionSizeY, collisionAcceleration = collisionAccelerationX..' '..collisionAccelerationY
                -- , collisionVelocity = collisionVelocityX..' '..collisionVelocityY --, collisionStatic = setCollisionStatic, collisionForce = collisionForceX..' '..collisionForceY
              })
            end
            if isTrigger then
              if triggerType == "polygon" then
                newObject.api:Attr({ triggerPolygon = setTriggerPolygon })
              end
            end
          end
        )
        --
        Debug.Log( 'Del object '..( object.api:GetNameFull() ) )
        object.api:Destroy()
      end --)
      Object.Select( nil )
      if settings.editorMode == 2 then
        settings.editorMode = 0
      end
      self:UpdateGuiBySelectedObject()
    end --) Del
    if id == 0x20 then    -- Space
      Object.Select( nil )
      self:UpdateGuiBySelectedObject()
      if settings.editorMode == 2 then
        settings.editorMode = 0
      end
      GUI.tabbedTemplates.currentItem = 0
    end
    if id == 0x47 then    -- G: toggle showing grid
      ToggleGrid()
    end
    if id == 0x0D then    -- Enter
      self:DoPause( not settings.gamePaused )
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


  OnMouseKeyPress = function( self, id ) --(
    return self:_OnMouseKey( id, true )
  end, --) OnMouseKeyPress


  OnMouseKeyRelease = function( self, id ) --(
    return self:_OnMouseKey( id, false )
  end, --) OnMouseKeyPress


  _OnMouseKey = function( self, id, isPressed ) --(
    local result = false
    if self.mode[ settings.editorMode ] ~= nil then
      result = self.mode[ settings.editorMode ]( self, id, isPressed )
    end
    GUIRenderer.OnMouseKey( id, isPressed )
    return result
  end, --) _OnMouseKey


  OnMouseMove = function( self, x, y ) --(
    if GUI.tabbedTemplates.isHovered or GUIRenderer.guiFocused then --(
      if GUIRenderer.guiFocused then
        if settings.editorMode ~= 50 then
          self.lastEditorMode = settings.editorMode
          settings.editorMode = 50
        end
      end
    else  --)(
      GUI.tooltip:SetPosition( 0, settings.windowSize.y )
      local mode = {
        [0] = function()
        end,
        [1] = function()
          settings.editorMode = 3
        end,
        [2] = function()
        end,
        [3] = function()
          local objectList = Object.GetSelected()
          if not settings.objectMode3Moved then
            -- settings.move.objectStart.x = oldX
            -- settings.move.objectStart.y = oldY
            settings.move.mouseStart.x = mousePos.x
            settings.move.mouseStart.y = mousePos.y
            settings.move.lastOffset.x = 0
            settings.move.lastOffset.y = 0
          end
          -- local newX = settings.move.objectStart.x + x - settings.move.mouseStart.x
          -- local newY = settings.move.objectStart.y + y - settings.move.mouseStart.y
          local tileSize = tonumber( GetTileSize() )
          local offsetX, offsetY = GetTileOffset()
          local dx, dy = math.floor( ( mousePos.x - settings.move.mouseStart.x ) / tileSize ) * tileSize, math.floor( ( mousePos.y - settings.move.mouseStart.y ) / tileSize ) * tileSize
          if settings.keys.isAlt then
            dx, dy = mousePos.x - settings.move.mouseStart.x, mousePos.y - settings.move.mouseStart.y
          end
          if settings.move.blockAxis.x then
            dx = 0
          end
          if settings.move.blockAxis.y then
            dy = 0
          end
          local moveByX, moveByY = -settings.move.lastOffset.x + dx, -settings.move.lastOffset.y + dy
          settings.move.lastOffset.x = settings.move.lastOffset.x + moveByX
          settings.move.lastOffset.y = settings.move.lastOffset.y + moveByY
          for _,objectByName in pairs( objectList ) do
            -- local objectByName = Object.Get( name )
            local x, y = objectByName.api:GetPos()
            objectByName.api:SetPos( x + moveByX, y + moveByY )
            -- newX = math.floor( newX / tileSize ) * tileSize + offsetX
            -- newY = math.floor( newY / tileSize ) * tileSize + offsetY
          end

          self:UpdateSelectedInterfacePosition( objectList )

          -- buffer
          if moveByX ~= 0 or moveByY ~= 0 then
            PushToBuffer( function()
              end, function()
                for _,objectByName in pairs( objectList ) do
                  -- local objectByName = Object.Get( name )
                  local x, y = objectByName.api:GetPos()
                  objectByName.api:SetPos( x - moveByX, y - moveByY )
                end
              end
            )
          end
          --
          settings.objectMode3Moved = true
        end,
        [20] = function()
          settings.editorMode = 21
          local cx, cy = Camera.GetPos()
          settings.move.mouseStart.x = mousePos.x - settings.windowSize.x * 0.5 + cx
          settings.move.mouseStart.y = mousePos.y - settings.windowSize.y * 0.5 + cy
        end,
        [50] = function() --(
          settings.editorMode = self.lastEditorMode
          self.lastEditorMode = 0
        end, --) 50
      }
      if mode[ settings.editorMode ] ~= nil then
        mode[ settings.editorMode ]()
      end
    end --)
    GUI.tabbedTemplates.OnMouseMove( x, y )
    GUIRenderer.OnMouseMove( x, y )
    mousePos.x = x
    mousePos.y = y

  end, --) OnMouseMove


  ShowSelectedInterface = function( self ) --(
    self:HideSelectedInterface()
    local objectsList = Object.GetSelected()
    if( TableSize( objectsList ) < 1 ) then
      while true do return end
    end

    local x, y, count = 0, 0, 0
    for _,object in pairs( objectsList ) do
      local x0, y0 = object.api:GetPos()
      x = x + x0
      y = y + y0
      count = count + 1
    end --for
    x = x / count
    y = y / count
    self.selectedInterface = {
      x = x,
      y = y,
      arrowUp = Object.New( 'editor/selected-interface/arrow-up', '', false ),
      arrowRight = Object.New( 'editor/selected-interface/arrow-right', '', false ),
    }

    self.selectedInterface.arrowUp = self.selectedInterface.arrowUp.api:SetScript( 'data/scripts/objects/editor-selected-object-arrow-up.lua' )
    self.selectedInterface.arrowUp:Init( self.selectedInterface.x, self.selectedInterface.y )
    self.selectedInterface.arrowRight = self.selectedInterface.arrowRight.api:SetScript( 'data/scripts/objects/editor-selected-object-arrow-right.lua' )
    self.selectedInterface.arrowRight:Init( self.selectedInterface.x, self.selectedInterface.y )
  end, --)


  HideSelectedInterface = function( self ) --(
    if self.selectedInterface ~= false then
      self.selectedInterface.arrowUp.api:Destroy()
      self.selectedInterface.arrowRight.api:Destroy()
      self.selectedInterface = false
    end
  end, --) HideSelectedInterface


  Render = function( self ) --(
    settings.timer = settings.timer + 0.01
    Debug.Render( 'clrscr' )
    if settings.showGrid then
      GUI.grid.Render()
    end
    --( вставка блока тайлов
    if settings.editorMode == 21 then
      local cx, cy = Camera.GetPos()
      local x = settings.move.mouseStart.x - cx + settings.windowSize.x * 0.5
      local y = settings.move.mouseStart.y - cy + settings.windowSize.y * 0.5
      local newX = mousePos.x
      local newY = mousePos.y
      Debug.Render( 'rect', x, y, 8.0, newX, newY, 8.0, '00ff00ff' )
      local left   = x < newX and x or newX
      local right  = x < newX and newX or x
      local top    = y < newY and y or newY
      local bottom = y < newY and newY or y
      local tileSize = GetTileSize()
      local offsetX, offsetY = GetTileOffset()
      left, top = GetTilePosByPixel( left, top )
      right, bottom = GetTilePosByPixel( right, bottom )
      local texture = GUI.tabbedTemplates.tabsList[ GUI.tabbedTemplates.currentTab ].items[ GUI.tabbedTemplates.currentItem ].icon
      for tx = left, right do
      for ty = top, bottom do
        x, y = GetPixelByTile( tx, ty )
        Debug.Render( 'sprite', x, y, 8.0, x + tileSize, y + tileSize, 8.0, texture, 'ffffff44' )
      end
      end
    end --)
    --( рамка тайла под курсором
    if settings.editorMode == 0 and settings.gamePaused and GUI.tabbedTemplates.currentItem > 0 then
      x, y = GetTilePosByPixel( mousePos.x, mousePos.y )
      left, top = GetPixelByTile( x, y )
      right, bottom = GetPixelByTile( x + 1, y + 1 )
      Debug.Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
    end --)

    --( рамка мультиселекта
    if settings.editorMode == 22 then
      local cx, cy = Camera.GetPos()
      left = settings.multiSelectStartPoint.x - cx
      top = settings.multiSelectStartPoint.y - cy
      right = mousePos.x
      bottom = mousePos.y
      if left > right then
        left, right = right, left
      end
      if top > bottom then
        top, bottom = bottom, top
      end
      Debug.Render( 'rect', left, top, 8.0, right, bottom, 8.0, 'ff0000ff' )
    end --)

    if settings.guiVisibility then
      -- GUI.templates.Draw()
      GUI.tabbedTemplates.Draw()
      GUIRendererRender( 0 ) -- additional GUI from gui.lua
    end

    if self.selectedInterface ~= false then --(
      local x, y = GetWorldToScreen( self.selectedInterface.x, self.selectedInterface.y )
      Debug.Render( 'rect', x - 1, y, 8.0, x + 1, y - 50, 8.0, '00ff00ff' )
      Debug.Render( 'rect', x, y - 1, 8.0, x + 50, y + 1, 8.0, 'ff0000ff' )
      Debug.Render( 'sprite', x + 50, y - 5, 8.0, x + 60, y + 4, 8.0, 'data/textures/tiles/tile000d.png', 'ff0000ff' )
      Debug.Render( 'sprite', x - 5, y - 60, 8.0, x + 4, y - 50, 8.0, 'data/textures/tiles/tile000b.png', '00ff00ff' )
    end --) self.selectedInterface
  end, --) Render


  OnUpdateCore = function( self ) --(
    self:Render()

    if self.selectedInterface ~= false then
      local objectsList = Object.GetSelected()
      self:UpdateSelectedInterfacePosition( objectsList )
    end

    if settings.gamePaused then
      Camera.Set( 'defaults/camera' )
    end
    -- local x, y = ObjectAttr( 'defaults/camera', { 'position' } )
    local camera = Object.Get( 'defaults/camera' )
    local x, y = camera.api:Attr({ 'position' })
    if bit32.band( settings.cameraDirection, 8 ) == 8 then
      y = y - 5
    end
    if bit32.band( settings.cameraDirection, 2 ) == 2 then
      y = y + 5
    end
    if bit32.band( settings.cameraDirection, 4 ) == 4 then
      x = x - 5
    end
    if bit32.band( settings.cameraDirection, 1 ) == 1 then
      x = x + 5
    end
    camera.api:Attr({ position = x..' '..y  })
  end, --) OnUpdateCore


  UpdateGuiBySelectedObject = function( self ) --(
    -- local object = GetSelectedObject()
    -- local objectList = Explode( object, '//' )
    local objectList = Object.GetSelected()
    local doReset = false
    local selectedObjectsCount = TableSize( objectList )
    if selectedObjectsCount < 1 then --(
      self:HideSelectedInterface()
      doReset = true
    elseif( selectedObjectsCount == 1 ) then --)( one object
      local object = nil
      for _,obj in pairs( objectList ) do
        object = obj
        break
      end
      -- object = object.api:GetNameFull()
      local isRenderable
        , isCollision
        , isTrigger
        , isLightBlockByCollision
        , isLightPoint
        , textureName
        , renderableSizeX
        , renderableSizeY
        , objectZ
        = object.api:Attr({
          'renderable',
          'collision',
          'trigger',
          'lightBlockByCollision',
          'lightPoint',
          'textureName',
          'renderableSize',
          'z',
        })
      GUI.elements.objectName:SetEnabled( true )
      GUI.elements.objectName:SetText( object.api:GetNameFull() )
      GUI.elements.objectZ:SetEnabled( true )
      GUI.elements.objectZ:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.objectZslider:SetEnabled( true )
      GUI.elements.objectZslider:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.isRenderable:SetEnabled( true )
      GUI.elements.isRenderable:SetIsChecked( isRenderable )
      GUI.elements.isCollision:SetEnabled( true )
      GUI.elements.isCollision:SetIsChecked( isCollision )
      GUI.elements.isTrigger:SetEnabled( true )
      GUI.elements.isTrigger:SetIsChecked( isTrigger )
      GUI.elements.isLightBlockByCollision:SetEnabled( true )
      GUI.elements.isLightBlockByCollision:SetIsChecked( isLightBlockByCollision )
      GUI.elements.isLightPoint:SetEnabled( true )
      GUI.elements.isLightPoint:SetIsChecked( isLightPoint )
      self:ShowSelectedInterface()
    else --)( multi-object
      local isRenderable, isCollision, isTrigger, isLightBlockByCollision, isLightPoint, objectZ = false, false, false, false, false, 0.0
      for _,object in pairs( objectList ) do
        local name = object.api:GetNameFull()
        isRenderable = isRenderable or object.api:Attr({ 'renderable' })
        isCollision = isCollision or object.api:Attr({ 'collision' })
        isTrigger = isTrigger or object.api:Attr({ 'trigger' })
        isLightBlockByCollision = isLightBlockByCollision or object.api:Attr({ 'lightBlockByCollision' })
        isLightPoint = isLightPoint or object.api:Attr({ 'lightPoint' })
        objectZ = object.api:Attr({ 'z' })
      end
      GUI.elements.objectName:SetEnabled( false )
      GUI.elements.objectName:SetText( '' )
      GUI.elements.objectZ:SetEnabled( true )
      GUI.elements.objectZ:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.objectZslider:SetEnabled( true )
      GUI.elements.objectZslider:SetText( string.format( '%.3f', objectZ ) )
      GUI.elements.isRenderable:SetEnabled( true )
      GUI.elements.isRenderable:SetIsChecked( isRenderable )
      GUI.elements.isCollision:SetEnabled( true )
      GUI.elements.isCollision:SetIsChecked( isCollision )
      GUI.elements.isTrigger:SetEnabled( true )
      GUI.elements.isTrigger:SetIsChecked( isTrigger )
      GUI.elements.isLightBlockByCollision:SetEnabled( true )
      GUI.elements.isLightBlockByCollision:SetIsChecked( isLightBlockByCollision )
      GUI.elements.isLightPoint:SetEnabled( true )
      GUI.elements.isLightPoint:SetIsChecked( isLightPoint )
      self:ShowSelectedInterface()
    end --)
    if doReset then --(
      GUI.elements.objectName:SetEnabled( false )
      GUI.elements.objectName:SetText( '' )
      GUI.elements.objectZ:SetEnabled( false )
      GUI.elements.objectZ:SetText( '' )
      GUI.elements.objectZslider:SetEnabled( false )
      GUI.elements.objectZslider:SetText( '' )
      GUI.elements.isRenderable:SetEnabled( false )
      GUI.elements.isRenderable:SetIsChecked( false )
      GUI.elements.isCollision:SetEnabled( false )
      GUI.elements.isCollision:SetIsChecked( false )
      GUI.elements.isTrigger:SetEnabled( false )
      GUI.elements.isTrigger:SetIsChecked( false )
      GUI.elements.isLightBlockByCollision:SetEnabled( false )
      GUI.elements.isLightBlockByCollision:SetIsChecked( false )
      GUI.elements.isLightPoint:SetEnabled( false )
      GUI.elements.isLightPoint:SetIsChecked( false )
    end --)
  end, --) UpdateGuiBySelectedObject


  UpdateSelectedInterfacePosition = function( self, objectList ) --(
    if self.selectedInterface == false then
      do return end
    end
    local x, y, count = 0, 0, 0
    for _,object in pairs( objectList ) do
      local x0, y0 = object.api:GetPos()
      x = x + x0
      y = y + y0
      count = count + 1
    end --for
    x = x / count
    y = y / count
    self.selectedInterface.x = x
    self.selectedInterface.y = y
    self.selectedInterface.arrowUp:Move( x, y )
    self.selectedInterface.arrowRight:Move( x, y )
  end, --) UpdateSelectedInterfacePosition


  FindPlayer = function( self ) --(
    local objectsList = Object.GetRoot()
    for _,object in pairs( objectsList ) do
      if object.api:HasTag( 'player' ) then
        self.player = object
        break
      end
    end
    return self.player
  end, --) FindPlayer


  -- Переключение режима паузы (редактора) и игры
  DoPause = function( self, setPause ) --(
    local player = Object.Get( 'editor' ):FindPlayer()
    settings.gamePaused = setPause
    Scene.SetPause( not settings.gamePaused )
    if settings.gamePaused then
      if Camera.Get().api:GetNameFull() ~= 'defaults/camera' then
        local x, y = player.api:Attr({ 'position' })
        Object.Get( 'defaults/camera' ).api:Attr({ position = x..' '..y })
      end
      Camera.Set( 'defaults/camera' )
    else
      if player ~= nil then
        Camera.Set( player )
      end
      Debug.RenderState( 0 )
      settings.editorType = 0
      GUI.elements.layer:SetText( 'default' )
      -- OnChangeLayer( GUI.elements.layer )
    end
  end --) DoPause
}
