--[[
  блок со знаком вопроса, можно выбивать монетки, грибы
  state:
    0 - default
    1 - смещение из-за удара
    2 - неподвижный после удара
]]

return {
  OnCreate = function( self ) --(
    if self.data.state == nil then
      self.data = {
      }
    end
    self.data.state = 0
    self.data.coinsCount = 3 -- вот этого здесь нельзя писать, только при инициализации
    self.api:SetAnimation( 'brick/question/0', 'default', 'repeat' )
    self.api:AddTag( 'has-coin' )
  end, --) OnCreate

  OnCollision = function( self, target, flags, vx, vy ) --(
    -- push brick
    if( self.data.state == 0 ) and ( bit32.band( flags, 4 ) == 4 ) and( vy < 10 ) then
      local _x, _y = self.api:GetPos()
      self.data.position = {
        x = _x,
        y = _y,
      }
      self.data.timer = 0
      local vx, vy = target.api:Attr({ 'collisionVelocity' })
      if( vy < 0 ) then
        vy = 50
        target.api:Attr({ collisionVelocity = vx..' '..vy })
      end
      self.data.state = 1

      -- coin
      if( self.api:HasTag( 'has-coin' ) ) then --(
        local coin = Object.New( 'coin', self.api:GetNameFull(), false )
        coin.api:SetPos( _x, _y )
        local parent = self
        coin.api:SetAnimation( 'supermario/coin', 'do', function( self )
            self.api:Destroy()
            parent:Reset()
          end
        )
        self.data.coinsCount = self.data.coinsCount - 1
        if( self.data.coinsCount == 0 ) then
          self.api:SetAnimation( 'brick/question/0', 'kick-and-freeze', function( self )
              self.api:SetAnimation( 'brick/question/0', 'freezed', 'repeat' )
            end
          )
          self.api:RemoveTag( 'has-coin' )
        else
          self.api:SetAnimation( 'brick/question/0', 'kick', function( self )
              self.api:SetAnimation( 'brick/question/0', 'default', 'repeat' )
            end
          )
        end
      end --) coin
    end
  end, --) OnCollision

  Reset = function( self ) --(
    self.data.state = 0
  end, --) Reset
}
