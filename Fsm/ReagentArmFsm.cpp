#define FSM_BUILD_SAMPLE2
#include "fsm.hpp"

//#include "fsm.hpp"
//#include "ThreadPriority.h"
////#include "Actuator.h"
//#include<time.h> 
//#include <sstream>
//
//bool ReagentArmError()
//{
//
//    return true;
//}
//
//enum {
//    INIT = 0,           // ��ʼ��
//    ADD_DN,             // ��ϡ��Һ
//    ADD_R1,             // ��R1
//
//    ERROR_1,            // ������1
//    ERROR_2,            // ������2
//
//    DEBUG,              // ����
//};
//
//class ReagentArm {
//public:
//    fsm::stack m_fsm;
//    //std::shared_ptr<CActuator> m_AsyncExePool;    // ִ��������
//
//    bool TestTime()
//    {
//        //time_t now = time(NULL);
//        //tm* tm_t = localtime(&now);
//        //std::stringstream ss;
//        //ss << "year:" << tm_t->tm_year + 1900 << " month:" << tm_t->tm_mon + 1 << " day:" << tm_t->tm_mday
//        //    << " hour:" << tm_t->tm_hour << " minute:" << tm_t->tm_min << " second:" << tm_t->tm_sec;
//
//        //std::cout << ss.str() << std::endl;
//
//        //std::cout << "��:" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
//        std::cout << "����: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
//        //std::cout << "΢��:" << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
//        //std::cout << "����:" << std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
//
//        return true;
//    }
//
//    ReagentArm()
//    {
//        //m_AsyncExePool = std::make_shared<CActuator>(RT_BIND_RUN_CPU, RT_SCHED_STRATEGY, RT_MIN_PRI, 2);
//
//        m_fsm.on(INIT, 'init') = [&](const fsm::args& args) {
//            std::cout << "��ʼ����� ����״̬" << std::endl;
//        };
//
//        m_fsm.on(ADD_DN, 'init') = [&](const fsm::args& args) {
//            std::cout << "��ϡ��Һ" << std::endl;
//            TestTime();
//            std::this_thread::sleep_for(std::chrono::milliseconds(3600));
//            TestTime();
//
//            if (!args.empty()) {
//                if("true" == args.at(0)) {
//                    m_fsm.set(INIT);
//                }
//                else {
//                    m_fsm.set(ERROR_1);
//                }
//            }
//
//            m_fsm.set(INIT);
//        };
//
//        m_fsm.on(ADD_R1, 'init') = [&](const fsm::args& args) {
//            std::cout << "��R1" << std::endl;
//            TestTime();
//            std::this_thread::sleep_for(std::chrono::milliseconds(3600));
//            TestTime();
//
//            if (!args.empty()) {
//                if ("true" == args.at(0)) {
//                    m_fsm.set(INIT);
//                }
//                else {
//                    m_fsm.set(ERROR_1);
//                }
//            }
//
//            m_fsm.set(ERROR_1);
//        };
//
//        m_fsm.on(ERROR_1, 'init') = [&](const fsm::args& args) {
//            std::cout << "1-1" << std::endl;
//            TestTime();
//
//                std::cout << "������ 1" << std::endl;
//
//                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//
//                if (!args.empty()) {
//                    if ("true" == args.at(0)) {
//                        m_fsm.set(INIT);
//                    }
//                    else {
//                        m_fsm.set(ERROR_1);
//                    }
//                }
//
//                m_fsm.set(INIT);
//
//                //auto ret = m_AsyncExePool->emplace_back([&] {
//                //    std::cout << "������ 1-1" << std::endl;
//                //});
//
//            std::cout << "1-2" << std::endl;
//            TestTime();
//        };
//
//        m_fsm.on(ERROR_2, 'init') = [&](const fsm::args& args) {
//            std::cout << "2-1" << std::endl;
//            TestTime();
//
//                std::cout << "������ 2" << std::endl;
//
//                std::this_thread::sleep_for(std::chrono::milliseconds(6000));
//
//                if (!args.empty()) {
//                    if ("true" == args.at(0)) {
//                        m_fsm.set(INIT);
//                    }
//                    else {
//                        m_fsm.set(ERROR_1);
//                    }
//                }
//
//                m_fsm.set(INIT);
//
//                //auto ret = m_AsyncExePool->emplace_back([&] {
//                //    std::cout << "������ 2-1" << std::endl;
//                //    });
//
//            std::cout << "2-2" << std::endl;
//            TestTime();
//        };
//
//        m_fsm.set(INIT);
//    }
//
//
//};
//
//bool ReagentArm1Task()
//{
//    ReagentArm m_reagentTest;
//
//    while (true)
//    {
//        int conValue;
//        std::cin >> conValue;
//
//        if (INIT == m_reagentTest.m_fsm.get_state()) {
//            std::cout << "�������� " << std::endl;
//            m_reagentTest.m_fsm.set(conValue);
//
//            //m_reagentTest.m_fsm.command(DEBUG);
//            //m_reagentTest.m_fsm.command(DEBUG,1,2);
//        }
//        else {
//            std::cout << "�쳣���� �ϱ���Ŀʧ��" << std::endl;
//        }
//    }
//}
//
//int main()
//{
//    std::deque< int > deque = {1,2,3,4,5};
//    std::deque<  std::deque< int >::reverse_iterator > aborted;
//    for (auto it = deque.rbegin(); it != deque.rend(); ++it) {
//        int& self = *it;
//        aborted.push_back(it);
//    }
//
//    for (auto it = aborted.begin(), end = aborted.end(); it != end; ++it) {
//        std::cout << "1 "<<  **it << std::endl;
//        if (deque.size()) {
//            deque.pop_back();
//        }
//        //deque.erase(it->base());
//        //deque.erase(it);
//        //deque.erase(--(it->base()));
//    }
//
//    auto reagentTask = std::make_shared<ThreadPriority>(96 - 1, std::bind(&ReagentArm1Task));
//
//    reagentTask->join();
//}
//
