#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

const int DELAY_MS = 100;  

mutex printMutex;

class Entity {
public:
    string   name;
    double   x, y;          
    double   endX, endY;    
    double   V;             
    double   ux, uy;        

    Entity(string name,
           double startX, double startY,
           double targetX, double targetY,
           double speed)
      : name(std::move(name)),
        x(startX), y(startY),
        endX(targetX), endY(targetY),
        V(speed)
    {
        double dx = endX - x;
        double dy = endY - y;
        double dist = sqrt(dx*dx + dy*dy);
        if (dist > 0) {
            ux = dx/dist;
            uy = dy/dist;
        } else {
            ux = uy = 0;
        }
    }

    void move() {
        while (true) {
            {
                lock_guard<mutex> lock(printMutex);
                cout << name << " @ (" 
                     << x << ", " << y << ")\n";
            }

            double dx = endX - x;
            double dy = endY - y;
            if (sqrt(dx*dx + dy*dy) < V) {
                x = endX;
                y = endY;
                lock_guard<mutex> lock(printMutex);
                cout << name << " arrived @ (" 
                     << x << ", " << y << ")\n\n";
                break;
            }

            x += ux * V;
            y += uy * V;

            this_thread::sleep_for(
                chrono::milliseconds(DELAY_MS));
        }
    }
};

int main() {
    srand(unsigned(time(nullptr)));

    double W, H, V;
    int nLegal, nPhysical;

    cout << "Enter W H V nLegal nPhysical: ";
    cin  >> W >> H >> V >> nLegal >> nPhysical;

    vector<thread> threads;
    threads.reserve(nLegal + nPhysical);

    for (int i = 0; i < nLegal; ++i) {
        double sx = (rand()/double(RAND_MAX)) * (W/2);
        double sy = (rand()/double(RAND_MAX)) * (H/2);
        double ex = (rand()/double(RAND_MAX)) * (W/2);
        double ey = (rand()/double(RAND_MAX)) * (H/2);
        threads.emplace_back(
            &Entity::move,
            Entity("Legal_" + to_string(i+1), sx, sy, ex, ey, V)
        );
    }

    for (int i = 0; i < nPhysical; ++i) {
        double sx = (W/2) + (rand()/double(RAND_MAX))*(W/2);
        double sy = (H/2) + (rand()/double(RAND_MAX))*(H/2);
        double ex = (W/2) + (rand()/double(RAND_MAX))*(W/2);
        double ey = (H/2) + (rand()/double(RAND_MAX))*(H/2);
        threads.emplace_back(
            &Entity::move,
            Entity("Physical_" + to_string(i+1), sx, sy, ex, ey, V)
        );
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
