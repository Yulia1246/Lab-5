#include <iostream>
#include <list>
#include <string>

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

class Airport
{
    std::string name;        
    AirControl landingPlanes;    
    GroundControl takingofPlanes;
    Plane* m_pOutputPlane = nullptr; 
public:
    Airport(std::string _name) : landingPlanes(10), takingofPlanes(10)
    {
        name = _name;
    }

    ~Airport()
    {
        if (m_pOutputPlane)
            delete m_pOutputPlane;
    }

    void Add(Plane* p, bool bInAir = true)
    {
        if (p)
        {
            p->setAir(bInAir);
            if (bInAir)
                landingPlanes.Add(p);
            else
                takingofPlanes.Add(p);
        }
    }

    Plane* PopOutputPlane()
    {
        if (m_pOutputPlane)
        {
            Plane* tmpPlane = m_pOutputPlane;
            m_pOutputPlane = nullptr;
            return tmpPlane;
        }
        return nullptr;
    }

    void ProcessTick()
    {
        landingPlanes.ProcessTick();
        takingofPlanes.ProcessTick();
        Plane* p1 = landingPlanes.PopReadyPlane();
        if (p1 != nullptr)
            takingofPlanes.Add(p1);
        m_pOutputPlane = takingofPlanes.PopReadyPlane();
    }

    void Print()
    {
        std::cout << "Airport: " << name << std::endl;
        std::cout << "Landing planes:" << std::endl;
        landingPlanes.Print();
        std::cout << "Taking off planes:" << std::endl;
        takingofPlanes.Print();
        if (m_pOutputPlane)
        {
            std::cout << "Output plane:" << std::endl;
            m_pOutputPlane->Print();
        }
    }
};

int main()
{
    Airport port("Kyiv");

    port.Add(new Plane(12, 15));
    port.Add(new Plane(13, 25));
    port.Add(new Plane(14, 15));

    std::cout << "Running:" << std::endl;

    for (int i = 0; i < 25; i++)
    {
        port.ProcessTick();
    }

    std::cout << "Final state:" << std::endl;

    port.Print();

    return 0;
}