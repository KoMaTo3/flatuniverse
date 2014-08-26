return {
  changeInterval = 0.1,
  rangeMin = 100,
  rangeMax = 500,
  rangeChangeSpeed = 50,
  powerMin = 0.8,
  powerMax = 1,
  powerChangeSpeed = 0.05,

  OnCreate = function( self ) --(
    self.range = self.rangeMax
    self.power = self.powerMax
    Core.SetTimer( self.changeInterval, function() self:UpdateRange() end )
  end, --) OnCreate

  UpdateRange = function( self ) --(
    self.range = math.min( math.max( math.random( self.range - self.rangeChangeSpeed, self.range + self.rangeChangeSpeed ), self.rangeMin ), self.rangeMax )
    self.power = math.min( math.max( math.random( self.power - self.powerChangeSpeed, self.power + self.powerChangeSpeed ), self.powerMin ), self.powerMax )
    self.api:Attr({ lightPointSize = self.range, lightPointColor = '1 0.7 0.3 '..self.power })
    Core.SetTimer( self.changeInterval, function() self:UpdateRange() end )
  end, --) UpdateRange
}
