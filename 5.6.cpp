#include <iostream>
#include <vector>
#include <string>
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
    void setTimeToFinish(int time) { m_nTimeToFinish = time; }
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
            m_pProcessingPlane->setTimeToFinish(m_nTimeForProcessing);
        }
        if (m_pProcessingPlane)
            m_pProcessingPlane->ProcessTick();
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
    bool Remove(int nCode)
    {
        for (auto p = m_Planes.begin(); p != m_Planes.end(); p++)
        {
            if ((*p)->getCode() == nCode)
            {
                delete (*p);
                m_Planes.erase(p);
                return true;
            }
        }
        return false;
    }
};

class Stat
{
    int m_nTimer = 0;
    int m_nCrashes = 0;
    int m_nLandings = 0;
    int m_nTakeOffs = 0;
public:
    int getTimer() { return m_nTimer; }
    void IncrementTimer(int interval = 1)
    {
        m_nTimer += interval;
    }
    void IncrementCrashes() { m_nCrashes++; }
    void IncrementLandings() { m_nLandings++; }
    void IncrementTakeOffs() { m_nTakeOffs++; }
    void Print()
    {
        std::cout << "Time: " << m_nTimer << "m, Crashes: " << m_nCrashes << ", Landings: " << m_nLandings << ", Take offs: " << m_nTakeOffs << std::endl;
    }
};

class AirControl : public PlanesControl
{
    Stat& m_rStat;
public:
    AirControl(int nTimeForProcessing, Stat& rStat) : PlanesControl(nTimeForProcessing), m_rStat(rStat) {}

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
                m_rStat.IncrementCrashes(); 
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
            m_rStat.IncrementLandings(); 
        }
        return 0;
    }
};

class GroundControl : public PlanesControl
{
    Stat& m_rStat;
public:
    GroundControl(int nTimeForProcessing, Stat& rStat) : PlanesControl(nTimeForProcessing), m_rStat(rStat) {}

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
            m_rStat.IncrementTakeOffs(); 
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
    Stat& m_rStat; 
public:
    Airport(std::string _name, Stat& rStat) : landingPlanes(10, rStat), takingofPlanes(10, rStat), m_rStat(rStat)
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
        m_rStat.IncrementTimer(); 
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
    bool Remove(int nCode)
    {
        bool result = false;
        if (m_pOutputPlane && (nCode == m_pOutputPlane->getCode()))
        {
            delete m_pOutputPlane;
            m_pOutputPlane = nullptr;
            result = true;
        }
        if (result || landingPlanes.Remove(nCode) || takingofPlanes.Remove(nCode))
        {
            std::cout << "#" << nCode << " Was removed from Airport " << name << std::endl;
            return true;
        }
        return false;
    }
    Stat& getStat() { return m_rStat; } 
};

typedef void (*ptDlgCommandFunc)(Airport& port);

class DlgCommand
{
    std::string m_sName;
    ptDlgCommandFunc m_pFunc;
public:
    DlgCommand(const char* sName, ptDlgCommandFunc pFunc = NULL) : m_sName(sName), m_pFunc(pFunc) {}
    ~DlgCommand(void) {}
    const char* GetName() { return m_sName.c_str(); }
    void Run(Airport& port) { if (m_pFunc) m_pFunc(port); }
};

class DialogManager
{
    std::vector<DlgCommand*> m_aCommands;
    Airport& m_refAirport;
public:
    DialogManager(Airport& port) : m_refAirport(port)
    {
        m_aCommands.push_back(new DlgCommand("Quit"));
    }
    ~DialogManager(void)
    {
        for (DlgCommand* p : m_aCommands)
            delete p;
    }
    void RegisterCommand(const char* sName, ptDlgCommandFunc pFunc)
    {
        m_aCommands.push_back(new DlgCommand(sName, pFunc));
    }
    void Run()
    {
        int nCommand = 1;
        while(nCommand)
        {
            std::cout << std::endl;
            for(size_t i = 0; i < m_aCommands.size();i++)
                std::cout << i << ". " << m_aCommands[i]->GetName() << std::endl;
            std::cout << "Enter command:";
            std::cin >> nCommand;
            if((nCommand > 0) && (nCommand < (int)m_aCommands.size()))
                m_aCommands[nCommand]->Run(m_refAirport);
        }
    }
};


void Init(Airport& port)
{
    port.Add(new Plane(12, 15));
    port.Add(new Plane(13, 25));
    port.Add(new Plane(14, 15));
}

void Process(Airport& port)
{
    int nTicks = 0;
    std::cout << "Input ticks count:";
    std::cin >> nTicks;
    for(int i = 0; i < nTicks; i++)
    {
        port.ProcessTick();
        port.Print();
    }
}

void Show(Airport& port)
{
    port.Print();
}

void Add(Airport& port)
{
    int nCode = 0;
    std::cout << "Input plane code:";
    std::cin >> nCode;
    port.Add(new Plane(nCode));
}

void Remove(Airport& port)
{
    int nCode = 0;
    std::cout << "Input plane code:";
    std::cin >> nCode;
    if (!port.Remove(nCode))
        std::cout << "#" << nCode << " Was not found in model" << std::endl;
}

void ShowStat(Airport& port)
{
    port.getStat().Print();
}

int main()
{   
    Stat stat;
    Airport port("Kyiv", stat);
    DialogManager mgr(port);
    mgr.RegisterCommand("Init", Init);
    mgr.RegisterCommand("Process", Process);
    mgr.RegisterCommand("Show", Show);
    mgr.RegisterCommand("Add", Add);
    mgr.RegisterCommand("Remove", Remove);
    mgr.RegisterCommand("Stat", ShowStat);
    mgr.Run();
    return 0;
}