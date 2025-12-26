    #include "subi_data.h"

    SubiDataManager::SubiDataManager() 
    {
        m_data = SubiData();
    }

    void SubiDataManager::Update() {
        m_data.FL[0]++;
        if (m_data.FL[0] > 255)
        {
            m_data.FL[0] = 0;
        }
        m_data.FR[1] = m_data.FL[0];
        m_data.RL[2] = m_data.FL[0];
        m_data.RR[3] = m_data.FL[0];

        m_data.OilPress += 0.05;
        if(m_data.OilPress > 11)
        {
          m_data.OilPress = 0;
        }

        m_data.OilTemp += 2;
        if (m_data.OilTemp > 120)
        {
          m_data.OilTemp = 0;
        }

        m_data.WaterTemp++;
        if (m_data.WaterTemp > 120)
        {
          m_data.WaterTemp = 0;
        }  

        m_data.RPM += 10;
        if (m_data.RPM > 7100)
        {
          m_data.RPM = 0;
        }      
    }

    int* SubiDataManager::GetTireData(int index)
    {
        switch (index) 
        {
            case 0:
              return m_data.FL;
            case 1:
              return m_data.FR;
            case 2:
              return m_data.RL;
            default:
              return m_data.RR;
        }
    }

    SubiData SubiDataManager::GetData()
    {
      return m_data;
    }