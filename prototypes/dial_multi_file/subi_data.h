#ifndef SUBI_DATA_H
#define SUBI_DATA_H

struct SubiData {
  int FL[4] ={0, 64, 128, 192};
  int FR[4] ={0, 16, 32, 48};
  int RL[4] ={0, 32, 64, 96};
  int RR[4] ={64, 128, 192, 255};

  int OilTemp = 0;
  float OilPress = 0;
  int WaterTemp = 0;
  int RPM = 0;
};

class SubiDataManager {
    private:
    SubiData m_data;

    public:
    SubiDataManager();

    void Update();   
    int* GetTireData(int index);
    SubiData GetData();
};

#endif