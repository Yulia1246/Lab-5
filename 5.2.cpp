#include <iostream>
#include <list>

class Plane
{
    int m_nCode;          
    int m_nFuel;          
    bool m_bInAir;        
    int m_nTimeToFinish;  
public:
    Plane(int nCode, int nFuel = 50) : m_nCode(nCode), m_nFuel(nFuel), m_bInAir(true), m_nTimeToFinish(-1) {}
    
    ~Plane(void) {}

    bool IsCrashed()
    {
        return m_bInAir && (m_nFuel <= 0);
    }

    bool IsFinished()
    {
        return m_nTimeToFinish <= 0;
    }

    void setAir(bool bAir)
    {
        m_bInAir = bAir;
    }

    int getCode()
    {
        return m_nCode;
    }

    void ProcessTick()
    {
        if (m_bInAir)
            m_nFuel--;
        if (m_nTimeToFinish > 0)
            m_nTimeToFinish--;
    }

    void Print()
    {
        std::cout << "#" << m_nCode << " (" << m_nFuel << ") ";
        if (m_bInAir)
            std::cout << "In air ";
        if (m_nTimeToFinish > 0)
            std::cout << "Processing: " << m_nTimeToFinish << "m left";
        std::cout << std::endl;
    }
};

class PlanesControl
{
protected:
    int m_nTimeForProcessing;          
    std::list<Plane*> m_Planes;        
    Plane* m_pProcessingPlane = nullptr; 
public:
    PlanesControl(int nTimeForProcessing = 10) : m_nTimeForProcessing(nTimeForProcessing) {}

    virtual ~PlanesControl(void)
    {
        if (m_pProcessingPlane)
            delete m_pProcessingPlane;
        while (!m_Planes.empty())
        {
            Plane* p = m_Planes.front();
            m_Planes.pop_front();
            delete p;
        }
    }

    Plane* PopReadyPlane()
    {
        if (m_pProcessingPlane && m_pProcessingPlane->IsFinished())
        {
            Plane* tmpPlane = m_pProcessingPlane;
            m_pProcessingPlane = nullptr;
            return tmpPlane;
        }
        return nullptr;
    }

    virtual void Add(Plane* p)
    {
        m_Planes.push_back(p);
    }

    virtual int ProcessTick()
    {
        if (!m_pProcessingPlane && !m_Planes.empty())
        {
            m_pProcessingPlane = m_Planes.front();
            m_Planes.pop_front();
            m_pProcessingPlane->ProcessTick();
        }

        for (Plane* p : m_Planes)
            p->ProcessTick();

        return 0;
    }

    virtual void Print()
    {
        if (m_pProcessingPlane)
            m_pProcessingPlane->Print();
        for (Plane* p : m_Planes)
            p->Print();
    }
};

class AirControl : public PlanesControl
{
public:
    AirControl(int nTimeForProcessing = 10) : PlanesControl(nTimeForProcessing) {}

    void Add(Plane* p) override
    {
        if (p)
        {
            p->setAir(true);
            m_Planes.push_back(p);
            std::cout << "#" << p->getCode() << " Asks landing" << std::endl;
        }
    }

    int ProcessTick() override
    {
        bool bProcessing = (m_pProcessingPlane != nullptr);
        bool bFinished = m_pProcessingPlane && m_pProcessingPlane->IsFinished();
        PlanesControl::ProcessTick();
        for (auto p = m_Planes.begin(); p != m_Planes.end();)
        {
            if ((*p)->IsCrashed())
            {
                std::cout << "#" << (*p)->getCode() << " Crashed!" << std::endl;
                delete (*p);
                p = m_Planes.erase(p);
            }
            else
                p++;
        }
        if (!bProcessing && m_pProcessingPlane)
            std::cout << "#" << m_pProcessingPlane->getCode() << " Start landing" << std::endl;
        if (bProcessing && bFinished)
        {
            m_pProcessingPlane->setAir(false);
            std::cout << "#" << m_pProcessingPlane->getCode() << " Landed" << std::endl;
        }
        return 0;
    }
};

class GroundControl : public PlanesControl
{
public:
    GroundControl(int nTimeForProcessing = 10) : PlanesControl(nTimeForProcessing) {}

    int ProcessTick() override
    {
        bool bProcessing = (m_pProcessingPlane != nullptr);
        bool bFinished = m_pProcessingPlane && m_pProcessingPlane->IsFinished();
        PlanesControl::ProcessTick();
        if (!bProcessing && m_pProcessingPlane)
            std::cout << "#" << m_pProcessingPlane->getCode() << " Ready to take off" << std::endl;
        if (bProcessing && bFinished)
        {
            m_pProcessingPlane->setAir(true);
            std::cout << "#" << m_pProcessingPlane->getCode() << " Took off." << std::endl;
        }
        return 0;
    }
};

int main()
{
    AirControl air(10);
    GroundControl land(10);

    air.Add(new Plane(12, 15));
    air.Add(new Plane(13, 25));
    air.Add(new Plane(14, 15));

    std::cout << "Running:" << std::endl;

    for (int i = 0; i < 25; i++)
    {
        air.ProcessTick();
        land.ProcessTick();
        Plane* p1 = air.PopReadyPlane();
        if (p1 != nullptr)
            land.Add(p1);
        Plane* p2 = land.PopReadyPlane();
        if (p2 != nullptr)
            delete p2;
    }

    std::cout << "Final state:" << std::endl;

    air.Print();
    land.Print();

    return 0;
}
