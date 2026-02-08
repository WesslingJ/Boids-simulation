import zmq
import subprocess
import os
import sys
import time
import pygame
import math

def draw_arrow(screen, color, start, end, arrow_size=10):
    # 1. Main line
    pygame.draw.line(screen, color, start, end, 2)
    
    # 2. Mathematics
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    angle = math.atan2(dy, dx)

    x1 = end[0] - arrow_size * math.cos(angle - math.pi / 6)
    y1 = end[1] - arrow_size * math.sin(angle - math.pi / 6)

    x2 = end[0] - arrow_size * math.cos(angle + math.pi / 6)
    y2 = end[1] - arrow_size * math.sin(angle + math.pi / 6)
    
    # 4. Drawing the triangle
    pygame.draw.polygon(screen, color, (end, (x1, y1), (x2, y2)))

EXE_PATH = os.path.join("..","build","Debug","SwarmEngine.exe")

def run_engine():
    #PyGame setup
    pygame.init()
    screen = pygame.display.set_mode((800, 800))
    pygame.display.set_caption("Swarm Viewer")

    screen_x = 400
    screen_y = 300
    current_vx, current_vy = 0.1, 0.1
    vector_scale = 100
    running = True

    #ZMQ setup
    engine_process = None
    context = None
    socket_listener = None

    if not os.path.exists(EXE_PATH):
        print(f"No engine file: {EXE_PATH}")
        return 

    try:
        engine_process = subprocess.Popen(EXE_PATH)
        context = zmq.Context()
        socket_listener = context.socket(zmq.SUB)
        socket_listener.setsockopt(zmq.LINGER, 0)
        socket_listener.connect("tcp://127.0.0.1:5555")
        socket_listener.subscribe("")
        
        while running:
            #ZMQ LOGIC AND PROCESS CLEANING
            if socket_listener.poll(100):
                try: 
                    data = socket_listener.recv_string()

                    screen.fill((0,0,0))
                    
                    parts = data.split(';')[:-1]

                    for i in range(0, len(parts), 4):
                        #Getting boids parameters
                        raw_x, raw_y, raw_vx, raw_vy = parts[i], parts[i+1], parts[i+2], parts[i+3]

                        screen_x = int((float(raw_x)+10)*40)
                        screen_y = int((float(raw_y)+10)*40)

                        vx = float(raw_vx)
                        vy = float(raw_vy)

                        #Drawing the boid
                        pygame.draw.circle(screen, (255, 255, 200), (screen_x, screen_y), 5)

                        #Drawing the boid velocity vector
                        end_x = screen_x + int(vx*vector_scale)
                        end_y = screen_y + int(vy*vector_scale)
                        draw_arrow(screen, (255, 100, 100), (screen_x, screen_y), (end_x, end_y), arrow_size=7)
                    
                except (zmq.ZMQError, ValueError, IndexError):
                    pass
                    
            #PYGAME LOGIC
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

            #Displaying everything
            pygame.display.flip()
            pygame.time.Clock().tick(60)

            if engine_process.poll() is not None:
                print("[App] Engine stopped unexpectedly!")
                running = False

    except KeyboardInterrupt:
        
        print("\n[App] Stopping on request")
    finally:
        print("[App] Killing C++ process")
        if engine_process:
            engine_process.terminate()
            try:
                engine_process.wait(timeout=1)
            except subprocess.TimeoutExpired:
                engine_process.kill() 
            print("[App] C++ closed")

        if socket_listener:
            socket_listener.close()

        if context:
            context.term()
            print("[App] ZMQ closed")

        pygame.quit()
        print("[App] Pygame closed")
        print("[App] Shutdown complete")

if __name__ == "__main__":
    run_engine()