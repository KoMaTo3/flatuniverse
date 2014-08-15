GUIWindow = {}

GUIRenderer = {
  GUIElements = {},
  activeItem = nil,
  focusedItem = nil,
  OnClickDefault = nil,
  OnMouseMoveDefault = nil,
  OnKeyboardDefault = nil,
  guiFocused = false,

  RenderEnable = function()
    Core.SetTimer( 0.1, GUIRendererRender )
  end,
}



--
-- Other
--
labelDebug = false
function GUIInit()
  Core.LoadScript( 'data/scripts/gui/button.lua' )
  Core.LoadScript( 'data/scripts/gui/checkbox.lua' )
  Core.LoadScript( 'data/scripts/gui/label.lua' )
  Core.LoadScript( 'data/scripts/gui/select.lua' )
  Core.LoadScript( 'data/scripts/gui/edit.lua' )
  Core.LoadScript( 'data/scripts/gui/slider-vertical.lua' )
  Core.LoadScript( 'data/scripts/gui/tree.lua' )
  Keyboard.Listen( 'GUIKeyboard' )
  Mouse.ListenKey( 'GUIMouseKey' )
  Mouse.ListenMove( 'GUIMouseMove' )
end

--[[ GUIRendererRender ]]
function GUIRendererRender( timerId )
  for id, item in pairs( GUIRenderer.GUIElements ) do
    item:Render()
  end
  --Core.SetTimer( 0.1, 'GUIRendererRender' )
end -- GUIRendererRender

--[[ GUIMouseKey ]]
function GUIMouseKey( id, isPressed ) --(
  local doDefaultHandler = false
  if not settings.guiVisibility then  --(
    doDefaultHandler = true
  else  --)(
    if GUIRenderer.focusedItem ~= nil then --(
      if GUIRenderer.focusedItem.OnClick ~= nil then
        GUIRenderer.focusedItem:OnClick( id, isPressed )
      end
    else --) (
      local inRect = false
      GUIRenderer.activeItem = nil
      for elementId, item in pairs( GUIRenderer.GUIElements ) do --(
        if item:TestInRect( mousePos.x, mousePos.y ) then --(
          inRect = true
          if GUIRenderer.activeItem ~= nil then
            --labelDebug:SetText( GUIRenderer.activeItem:GetType() )
            if GUIRenderer.activeItem.OnClick ~= nil then
              GUIRenderer.activeItem:OnClick( id, isPressed )
            end
          end
          do break end
        end --) if
      end --) for elementId,item
      if not inRect then
        doDefaultHandler = true
      end
    end --) ~GUIRenderer.focusedItem
  end --)
  if doDefaultHandler and GUIRenderer.OnClickDefault ~= nil then
    GUIRenderer.OnClickDefault( id, isPressed )
  end
end --) GUIMouseKey

--[[ GUIMouseMove ]]
function GUIMouseMove( x, y )
local doDefaultHandler = false
  if not settings.guiVisibility then
    doDefaultHandler = true
  end
  mousePos.x = x
  mousePos.y = y
  GUIRenderer.activeItem = nil
  local inRect = false
  for id, item in pairs( GUIRenderer.GUIElements ) do --(
    if item:TestInRect( x, y ) then --(
      inRect = true
      do break end
    end --) if
  end --) for id,item
  if not inRect then
    doDefaultHandler = true
  end
  if doDefaultHandler and GUIRenderer.OnMouseMoveDefault ~= nil then
    GUIRenderer.OnMouseMoveDefault( x, y )
  end
  GUIRenderer.guiFocused = not doDefaultHandler

  -- labelDebug:SetText( GUIRenderer.activeItem == nil and '-' or GUIRenderer.activeItem:GetType() )
end -- GUIMouseMove

--[[ GUIKeyboard ]]
function GUIKeyboard( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then
    if GUIRenderer.focusedItem.OnKeyboard ~= nil then
      if GUIRenderer.focusedItem:OnKeyboard( id, isPressed ) == false and GUIRenderer.OnKeyboardDefault ~= nil then
        GUIRenderer.OnKeyboardDefault( id, isPressed )
      end
    end
  else
    if GUIRenderer.OnKeyboardDefault ~= nil then
      GUIRenderer.OnKeyboardDefault( id, isPressed )
    end
  end
end -- GUIKeyboard

GUIInit()
