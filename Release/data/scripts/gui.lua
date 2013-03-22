GUIWindow = {}

GUIRenderer = {
  GUIElements = {},
  activeItem = nil,
  focusedItem = nil,
  OnClickDefault = nil,
  OnMouseMoveDefault = nil,

  RenderEnable = function()
    SetTimer( 0.1, 'GUIRendererRender' )
  end,
}



--
-- Other
--
labelDebug = false
function GUIInit()
  LoadScript( 'data/scripts/gui/button.lua' )
  LoadScript( 'data/scripts/gui/checkbox.lua' )
  LoadScript( 'data/scripts/gui/label.lua' )
  LoadScript( 'data/scripts/gui/select.lua' )
  ListenMouseKey( 'GUIMouseKey' )
  ListenMouseMove( 'GUIMouseMove' )

  labelDebug = GUILabel:Create( 0, 0, 1000, 20, 'debug' )

  local label0 = GUILabel:Create( 80, 10, 100, 100, 'Подложка №1' )
  local but0 = GUIButton:Create( 5, 20, 85, 75, 'Кнопко №1', function() Alert(1) end, label0 )
  local cb0 = GUICheckbox:Create( 5, 15, 'cb0', false, but0 )
  local cb1 = GUICheckbox:Create( 5, 30, 'cb1', false, but0 )
  local cb2 = GUICheckbox:Create( 5, 45, 'cb2', true, but0 )

  local list = {}
  table.insert( list, 'default' )
  table.insert( list, 'renderable' )
  table.insert( list, 'collision' )
  table.insert( list, 'trigger' )
  local sel0 = GUISelect:Create( 200, 10, 90, 1, list, function( obj ) Alert( obj:GetValue()..':'..obj:GetText() ) end, labelDebug )
end

--[[ GUIRendererRender ]]
function GUIRendererRender()
  for id, item in pairs( GUIRenderer.GUIElements ) do
    --Alert( item.rect.right )
    item:Render()
  end
  --SetTimer( 0.1, 'GUIRendererRender' )
end -- GUIRendererRender

--[[ GUIMouseKey ]]
function GUIMouseKey( id, isPressed )
  if GUIRenderer.focusedItem ~= nil then --(
    GUIRenderer.focusedItem:OnClick( id, isPressed )
  else --) (
    local inRect = false
    GUIRenderer.activeItem = nil
    for id, item in pairs( GUIRenderer.GUIElements ) do --(
      if item:TestInRect( mousePos.x, mousePos.y ) then --(
        inRect = true
        if GUIRenderer.activeItem ~= nil then
          -- Alert( GUIRenderer.activeItem:GetType() )
          --labelDebug:SetText( GUIRenderer.activeItem:GetType() )
          GUIRenderer.activeItem:OnClick( id, isPressed )
          -- Alert( GUIRenderer.activeItem.rect.left )
        end
        do break end
      end --) if
    end --) for id,item
    if not inRect and GUIRenderer.OnClickDefault ~= nil then
      GUIRenderer.OnClickDefault( id, isPressed )
    end
  end --) ~GUIRenderer.focusedItem
end -- GUIMouseKey

--[[ GUIMouseMove ]]
function GUIMouseMove( x, y )
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
  if not inRect and GUIRenderer.OnMouseMoveDefault ~= nil then
    GUIRenderer.OnMouseMoveDefault( x, y )
  end

  -- labelDebug:SetText( GUIRenderer.activeItem == nil and '-' or GUIRenderer.activeItem:GetType() )
end -- GUIMouseMove

GUIInit()
