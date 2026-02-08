#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <zmq.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct SwarmConfig{
    float visual_range = 2.0f;
    float protected_range = 0.5f;
    
    float separation_factor = 0.05f;
    float alignment_factor = 0.05f;
    float cohesion_factor = 0.01f;

    float max_speed = 0.2f;
    float min_speed = 0.05f;
};

const SwarmConfig cfg;

struct Boid
{
    float x, y;
    float vx, vy;
};

float random_float(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
};

float distance(const Boid &b1, const Boid &b2)
{
    float dx = b1.x - b2.x;
    float dy = b1.y - b2.y;
    return std::sqrt(dx * dx + dy * dy);
};

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Engine simulation - ZMQ setup
    zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::pub);
    socket.bind("tcp://127.0.0.1:5555");

    // Creating the boids
    std::vector<Boid> swarm;
    int num_boids = 50;

    for (int i = 0; i < num_boids; ++i){

        Boid boid = {
            random_float(-9.0f, 9.0f), random_float(-9.0f, 9.0f),
            random_float(-0.1f, 0.1f), random_float(-0.1f, 0.1f)
        };
        swarm.push_back(boid);
    }

    std::cout << "Engine started with: " << swarm.size() << " boids." << std::endl;

    while (true){

        std::string data = "";

        // Reynolds algorithm: Separation, Alignment, Cohesion
        for (auto &current_boid : swarm){

            float align_avg_vx = 0.0f;
            float align_avg_vy = 0.0f;
            float cohesion_avg_x = 0.0f;
            float cohesion_avg_y = 0.0f;
            float separation_vx = 0.0f;
            float separation_vy = 0.0f;

            int neighbors_count = 0;

            for (auto &other_boid : swarm){

                if (&current_boid == &other_boid){
                    continue;
                }

                float dist = distance(current_boid, other_boid);

                if (dist < cfg.visual_range){
                    // Alignment
                    align_avg_vx += other_boid.vx;
                    align_avg_vy += other_boid.vy;
                    
                    // Cohesion
                    cohesion_avg_x += other_boid.x;
                    cohesion_avg_y += other_boid.y;

                   
                    // Separation acts if very close
                    if(dist < cfg.protected_range){
                        float close_dx = current_boid.x - other_boid.x;
                        float close_dy = current_boid.y - other_boid.y;

                        separation_vx += close_dx / (dist + 0.0001f);
                        separation_vy += close_dy / (dist + 0.0001f);
                    }
                    
                    neighbors_count++;
                    
                }
                
            }
            if (neighbors_count > 0)
            {
                //Alignment
                align_avg_vx /= neighbors_count;
                align_avg_vy /= neighbors_count;
                
                current_boid.vx += (align_avg_vx - current_boid.vx) * cfg.alignment_factor;
                current_boid.vy += (align_avg_vy - current_boid.vy) * cfg.alignment_factor;

                //Cohesion
                cohesion_avg_x /= neighbors_count;
                cohesion_avg_y /= neighbors_count;
                
                float vec_to_center_x = cohesion_avg_x - current_boid.x;
                float vec_to_center_y = cohesion_avg_y - current_boid.y;
                current_boid.vx += vec_to_center_x * cfg.cohesion_factor;
                current_boid.vy += vec_to_center_y * cfg.cohesion_factor;
            }

            //Separation
            current_boid.vx += separation_vx*cfg.separation_factor;
            current_boid.vy += separation_vy*cfg.separation_factor;
            
            float speed = std::sqrt(current_boid.vx * current_boid.vx + current_boid.vy * current_boid.vy);

            if (speed > cfg.max_speed){
                current_boid.vx = (current_boid.vx / speed) * cfg.max_speed;
                current_boid.vy = (current_boid.vy / speed) * cfg.max_speed;
            }
            else if (speed < cfg.min_speed && speed > 0){
                 current_boid.vx = (current_boid.vx / speed) * cfg.min_speed;
                 current_boid.vy = (current_boid.vy / speed) * cfg.min_speed;
            }

            current_boid.x += current_boid.vx;
            current_boid.y += current_boid.vy;


            // Sticky walls problem fix

            float margin = 10.0f; // world limit

            if (current_boid.x > margin){
                current_boid.x = margin;
                current_boid.vx *= -1.0f;
            }
            else if (current_boid.x < -margin){
                current_boid.x = -margin;
                current_boid.vx *= -1.0f;
            }
            if (current_boid.y > margin){
                current_boid.y = margin;
                current_boid.vy *= -1.0f;
            }
            else if (current_boid.y < -margin){
                current_boid.y = -margin; //
                current_boid.vy *= -1.0f;
            }

            std::string boid_data = std::to_string(current_boid.x) + ";" + std::to_string(current_boid.y) + ";" + std::to_string(current_boid.vx) + ";" + std::to_string(current_boid.vy) + ";";

            data += boid_data;
        }

        // What python will be catching
        zmq::message_t message(data.size());
        memcpy(message.data(), data.data(), data.size());
        socket.send(message, zmq::send_flags::none);

        // Waiting one second (overload simulation)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    return 0;
}