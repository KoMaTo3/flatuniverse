#pragma once

#include "klib.h"



class State {
public:
  State         ( const Dword setId, const float setLifeTime, const int setStacks = 1, const float setSpeed = 1.0f );
  virtual ~State();
  bool    Update( float dt );

private:
  Dword id;
  int   stacks;       //����� "������" ������
  float lifeTime,     //������� ����� �����, ��� �� maxLifeTime �� 0
        maxLifeTime;  //����� ����� ������. ���� ������� ����, ���� ���� �����
  float speed;        //�������� ������� ����� ������. 1 - �����, 0 - ����� ����������, >1 - �������, [0;1] - ��������
};