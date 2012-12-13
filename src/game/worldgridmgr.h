/*
  ����� � ����.
  ���� ��� - ����� ������ (WorldGrid)
  ���������� ��������� - ������� �������� ������.
  ��� ����� ������ ����������� ��� ������ �����������.
  � ����� ���� ������ �������� ��������.
  - ������ ���-� �������� ����������� - ���������� ���
  ���� ���������, ��� �� � ����� �������� �������� �������� �� �������:
  - ���� ���� - ����������� ������ � ������ ����. ���� ����� ��� - ���������
  ����, ������, ��� ������ ��������� ���������, � ��� ����� � ���������-���������
*/

#pragma once

#include "worldgrid.h"


#define WORLD_GRID_UPDATE_INTERVAL    ( 1.0f / 10.0f )    //������ ���������� ��������� ������



class WorldGridManager
{
private:
  Object              *rootObject;    //�������� ������. � �� �������� gridc#/gridr#...
  float               currentTime;    //�����, ��������� � ������� ���������� ����������
  WorldGridObjectList activeObjects;  //�������� �������� ��������

public:
  WorldGridManager( Object *newRootObject );
  virtual ~WorldGridManager();

  void Update();
  void AddActiveObject( Object *obj );
};