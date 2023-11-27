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
        if (m_pProcessingPlane->IsFinished())
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

int main()
{
    PlanesControl dkp(10);
    
    dkp.Add(new Plane(12, 15));
    dkp.Add(new Plane(13, 25));
    dkp.Add(new Plane(14, 15));
    
    dkp.Print();
    
    for (int i = 0; i < 5; i++)
        dkp.ProcessTick();
    
    dkp.Print();
    
    return 0;
}
