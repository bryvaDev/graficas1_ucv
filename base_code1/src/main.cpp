#include "PixelRender.h"
#include <iostream>
#include <algorithm>
#include <chrono>

struct Pixel {
    int x, y;
    bool operator==(const Pixel& other) const {
        return (x == other.x && y == other.y);
    };
    bool operator<(const Pixel& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    };
};
struct Line {
    int x0, y0;
    int x1, y1;
    RGBA color;
};
struct Elipse {
    int cx, cy;
    int a, b;
    RGBA color;
};
class Comparator {
    std::vector<Pixel> f1;
    std::vector<Pixel> f2;
    int goodTest = 0;
    int totalTest = 0;
public:
    Comparator() {};
    ~Comparator() {};
    void loadPixel(Pixel newPixel, int type) {
        if (type == 1) {
            f1.push_back(newPixel);
            return;
        }
        f2.push_back(newPixel);
    };
    void compare() {
        totalTest++;
        std::sort(f1.begin(), f1.end());
        std::sort(f2.begin(), f2.end());
        if (f1.size() != f2.size()) {
            std::cout << "Los metodos no generan la misma cantidad de pixeles\n";
            std::cout << f1.size() << " vs " << f2.size() << "\n";
            return;
        };
        for (size_t i = 0; i < f1.size(); i++) {
            if (!(f1[i] == f2[i])) {
                std::cout << "Test " << totalTest << ": Pixeles diferentes en posicion " << i << "\n";
                std::cout << "  Algoritmo 1: (" << f1[i].x << ", " << f1[i].y << ")\n";
                std::cout << "  Algoritmo 2: (" << f2[i].x << ", " << f2[i].y << ")\n";
                return;
            }
        }
        goodTest++;
        return;
    };
    void endTest(bool &endTest, std::vector<Elipse>&elipses){
        endTest = false;
        elipses.clear();
        std::cout << "\n===RESULTADOS FINALES===\n";
        std::cout << "Tests exitosos: " << goodTest << "/" << totalTest << "\n";

        if (goodTest == totalTest) {
            std::cout << "Ambos metodos generan la misma cantidad de pixeles, en las misma posiciones";
        } else {
            std::cout << "Los metodos tienen diferencias en "
                << (totalTest - goodTest) << " tests\n";
        }
        std::cout << "\nPresione ENTER para continuar...\n";
        /*if (goodTest == 10000) {
            std::cout << "Ambos metodos generan la misma cantidad de pixeles, en las misma posiciones";
        };*/
    };
    void newTest() {
        f1.clear();
        f2.clear();
       
    };
    void newSet() {
        goodTest = 0;
        totalTest = 0;
    };
};

class CMyTest : public CPixelRender {
    int m_x0 = -1, m_y0 = -1, m_x1 = -1, m_y1 = -1;
    RGBA white = { 255, 255, 255, 255 };
    RGBA RGBA_Current = white;
    float colorArray[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool drawing = false;
    bool test = false;
    bool test2 = false;
    bool real = false;
    bool waitEnter = false;
    bool test2Interface = false;
    bool useSlowElipse = false;
    int drawElipseType = 1;
    int numberOfCases[7] = {50000, 100000, 150000, 250000, 500000, 750000, 1000000};
    Comparator ac;
    std::vector<Line> lines;
    std::vector<Elipse> elipses;
public:
    CMyTest() {};
    ~CMyTest() {};
    void setPixelCatcher(int x, int y, const RGBA& color) {
        ac.loadPixel(Pixel{ x, y }, drawElipseType);
        setPixel(x, y, color);
    };
    void drawlineReal(int x0, int y0, int x1, int y1, RGBA color) {
        float dx = x1 - x0;
        float dy = y1 - y0;

        if (dx == 0 && dy == 0) {
            setPixel(x0, y0, color);
            return;
        }

        if (std::abs(dy) <= std::abs(dx)) {
            float m = dy / dx;
            float y = y0;

            int sx = (x0 < x1) ? 1 : -1;
            for (int x = x0; x != x1; x += sx) {
                setPixel(x, static_cast<int>(std::round(y)), color);
                if (sx == 1) {
                    y += m;
                }
                else {
                    y -= m;
                };
            }
            setPixel(x1, y1, color);
        }
        else {
            float m = dx / dy;
            float x = x0;

            int sy = (y0 < y1) ? 1 : -1;
            for (int y = y0; y != y1; y += sy) {
                setPixel(static_cast<int>(std::round(x)), y, color);
                if (sy == 1) {
                    x += m;
                }
                else {
                    x -= m;
                };
            }
            setPixel(x1, y1, color);
        }
    }
    void ellipsePointsToCompare(int cx, int cy, int x, int y, RGBA c) {
        setPixelCatcher(cx + x, cy + y, c);
        setPixelCatcher(cx + x, cy - y, c);
        setPixelCatcher(cx - x, cy + y, c);
        setPixelCatcher(cx - x, cy - y, c);
    };

    void ellipsePoints(int cx, int cy, int x, int y, RGBA c) {
        if (test) {
            ellipsePointsToCompare(cx, cy, x, y, c);
            return;
        };
        setPixel(cx + x, cy + y, c);
        setPixel(cx + x, cy - y, c);
        setPixel(cx - x, cy + y, c);
        setPixel(cx - x, cy - y, c);
    };

    void drawEllipse1(int cx, int cy, int64_t a, int64_t b, RGBA c) {
        int64_t x, y, d;

        // modalidad 1
        x = 0;
        y = b;
        d = b * (4 * b - 4 * a * a) + a * a;
        ellipsePoints(cx, cy, x, y, c);
        while (b * b * 2 * (x + 1) < a * a * (2 * y - 1)) {
            if (d < 0) {
                d = d + 4 * (b * b * (2 * x + 3));
            }
            else {
                d = d + 4 * (b * b * (2 * x + 3) + a * a * (-2 * y + 2));
                y = y - 1;
            };
            x = x + 1;
            ellipsePoints(cx, cy, x, y, c);
        };

        // modalidad 2
        d = b * b * (4 * x * x + 4 * x + 1) + a * a * (4 * y * y - 8 * y + 4) - 4 * a * a * b * b;
        while (y > 0) {
            if (d < 0) {
                d = d + 4 * (b * b * (2 * x + 2) + a * a * (-2 * y + 3));
                x = x + 1;
            }
            else {
                d = d + 4 * a * a * (-2 * y + 3);
            };
            y = y - 1;
            ellipsePoints(cx, cy, x, y, c);
        };
        if (b == 0) {
            //std::cout << "entra";
            drawlineInt(cx - a, cy, cx + a, cy, c);
        };
    };

    void drawEllipse2(int cx, int cy, int64_t a, int64_t b, RGBA c) {

        int64_t x, y, d;
        int64_t a2 = a * a;
        int64_t b2 = b * b;

        //Constantes Shifteadas

        int64_t b2_2 = b2 << 1;
        int64_t b2_4 = b2 << 2;
        int64_t b2_8 = b2 << 3;

        int64_t a2_2 = a2 << 1;
        int64_t a2_4 = a2 << 2;
        int64_t a2_8 = a2 << 3;

        int64_t a2b2_8 = (b2 + a2) << 3;


        // Modalidad 1
        x = 0;
        y = b;
        //d = (b2 << 2) * (1 - a) + a2;
        d = b * ((b << 2) - (a2_4)) + a2;
        int64_t deltaE = 12 * b2;
        int64_t deltaSE = ((3 * b2 + a2 * (-2 * b + 2)) << 2);
        int64_t cond_var = b2_2 + a2 - (a2_2) * b;
        ellipsePoints(cx, cy, x, y, c);
        while (cond_var < 0) {
            if (d < 0) {
                d += deltaE;
                deltaE += b2_8;
                deltaSE += b2_8;
                cond_var += b2_2;
            }
            else {
                d += deltaSE;
                deltaE += b2_8;
                deltaSE += a2b2_8;
                y = y - 1;
                cond_var += ((b2 + a2) << 1);
            };
            x = x + 1;
            ellipsePoints(cx, cy, x, y, c);
        };

        //Modalidad 2
        int64_t deltaS = (a2_4) * (-2 * y + 3);
        int64_t deltaSE2 = ((b2 * ((x << 1) + 2) + a2 * (-2 * y + 3)) << 2);
        d = b2 * ((x << 2) * x + (x << 2) + 1) + a2 * ((y << 2) * y - (y << 3) + 4) - (a2 * b2_4);
        while (y > 0) {
            if (d < 0) {
                d += deltaSE2;
                deltaS += a2_8;
                deltaSE2 += a2b2_8;
                x = x + 1;
            }
            else {
                d += deltaS;
                deltaS += a2_8;
                deltaSE2 += a2_8;
            };
            y = y - 1;
            ellipsePoints(cx, cy, x, y, c);
        }
        if (b == 0) {
            //std::cout << "entra";
            drawlineInt(cx - a, cy, cx + a, cy, c);
        }
    }

    void drawlineInt(int x0, int y0, int x1, int y1, RGBA color) {
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int incX = (x0 < x1) ? 1 : -1;
        int incY = (y0 < y1) ? 1 : -1;
        bool intercambio = (dy > dx);
        if (intercambio) {
            std::swap(dx, dy);
        }
        int d = (dy << 1) - dx;
        int incE = dy << 1;
        int incNE = (dy - dx) << 1;
        int x = x0;
        int y = y0;
        setPixel(x, y, color);
        for (int i = 0; i < dx; i++) {
            if (d <= 0) {
                d += incE;
                intercambio ? (y += incY) : (x += incX);
            }
            else {
                d += incNE;
                x += incX;
                y += incY;
            }
            setPixel(x, y, color);
        }
    }

    void drawInterface() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        center[1] += 150;
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));

        ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_Once);
        ImGui::Begin("Control Panel");

        ImGui::Text("Seleccionar Color:");

        // Mostrar selector y verificar cambios
        if (ImGui::ColorEdit4("##ColorPicker", colorArray, ImGuiColorEditFlags_NoInputs)) {
            RGBA_Current.r = static_cast<unsigned char>(colorArray[0]*255);
            RGBA_Current.g = static_cast<unsigned char>(colorArray[1]*255);
            RGBA_Current.b = static_cast<unsigned char>(colorArray[2]*255);
        };
        /*ImGui::Separator();
        if (ImGui::Button("Add 1000 lines")) {
            for (int i = 0; i < 1000; i++) {
                RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                int x0 = rand() % width;
                int y0 = rand() % height;
                int x1 = rand() % width;
                int y1 = rand() % height;
                lines.push_back(Line{ x0, y0, x1, y1, color });
            };
        };*/
        ImGui::Separator();
        ImGui::Checkbox("Usar Metodo 1 para dibujar Elipse", &useSlowElipse);
        ImGui::Separator();
        if (ImGui::Button("Iniciar Prueba")) {
            if (!waitEnter) {
                test = true;
            }else {
                std::cout << "Presione ENTER para continuar \n";
            };
        }
        /*if (test2Interface) {
            ImGui::Separator();
            //ImGui::SliderInt("Inserte numero de casos para el test2", &numberOfCases, 0, 1000000);
            int sliderValue = numberOfCases / 50000;
            if (ImGui::SliderInt("Casos para test2 (en 50k)", &sliderValue, 0, 20)) {
                numberOfCases = sliderValue * 50000;
            }
            ImGui::Text("Valor actual: %d casos", numberOfCases);
        };*/
        
        //ImGui::Checkbox("Arimetica Real", &real);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        char title[256];
        snprintf(title, sizeof(title), "CPixelRender - frames per second: %.2lf fps", ImGui::GetIO().Framerate);
        glfwSetWindowTitle(m_window, title);
    }

    void update()
    {
        std::fill(m_buffer.begin(), m_buffer.end(), RGBA{0,0,0,0});
        //pixelsModifiedThisSecond += m_nPixels;
        // tu código actual para modificar pixels...
        if (drawing) {
            //if (!real) drawlineInt(m_x0, m_y0, m_x1, m_y1, RGBA_Current);
            //if (real) drawlineReal(m_x0, m_y0, m_x1, m_y1, RGBA_Current);
            //drawlineReal(m_x0, m_y0, m_x1, m_y1, RGBA_Current);
            if (useSlowElipse) drawEllipse1(m_x0, m_y0, abs(m_x1 - m_x0), abs(m_y1 - m_y0), RGBA_Current);
            if (!useSlowElipse) drawEllipse2(m_x0, m_y0, abs(m_x1 - m_x0), abs(m_y1 - m_y0), RGBA_Current);

        };
        /*for (auto& line : lines) {
            if (!real) drawlineInt(line.x0, line.y0, line.x1, line.y1, line.color);
            if (real) drawlineReal(line.x0, line.y0, line.x1, line.y1, line.color);
        };*/
        if (!test && !test2) {
            for (auto& elipse : elipses) {
                if (useSlowElipse) drawEllipse1(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
                if (!useSlowElipse) drawEllipse2(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
            };
        };
        if (test && !waitEnter) {
            elipses.clear();
            ac.newSet();
            if (elipses.size() < 10000) {
                for (int i = 0; i < 10000; i++) {
                    RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                    int x0 = rand() % width;
                    int y0 = rand() % height;
                    int x1 = rand() % width;
                    int y1 = rand() % height;
                    elipses.push_back(Elipse{ x0, y0, abs(x1 - x0), abs(y1 - y0), color });
                };
            };
            for (auto& elipse : elipses) {
                ac.newTest();
                drawElipseType = 1;
                drawEllipse1(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
                drawElipseType = 2;
                drawEllipse2(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
                ac.compare();
            }
            ac.endTest(test, elipses);
            waitEnter = true;
            test2Interface = true;
        }
        else if (test2) {
            for (auto& caseNumber : numberOfCases) {
                elipses.clear();
                for (int i = 0; i < caseNumber; i++) {
                    RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                    int x0 = rand() % width;
                    int y0 = rand() % height;
                    int x1 = rand() % width;
                    int y1 = rand() % height;
                    elipses.push_back(Elipse{ x0, y0, abs(x1 - x0), abs(y1 - y0), color });
                };
                auto totalTime1 = std::chrono::duration<double, std::milli>::zero();
                auto totalTime2 = std::chrono::duration<double, std::milli>::zero();
                for (auto& elipse : elipses) {
                    auto startTime1 = std::chrono::high_resolution_clock::now();
                    drawEllipse1(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
                    auto endTime1 = std::chrono::high_resolution_clock::now();
                    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(endTime1 - startTime1);
                    totalTime1 += duration1;
                    //Metodo 2
                    auto startTime2 = std::chrono::high_resolution_clock::now();
                    drawEllipse2(elipse.cx, elipse.cy, elipse.a, elipse.b, elipse.color);
                    auto endTime2 = std::chrono::high_resolution_clock::now();
                    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(endTime2 - startTime2);
                    totalTime2 += duration2;
                }
                std::cout << "Para n = " << caseNumber << " - " << elipses.size() << "\n";
                std::cout << "Tiempo total drawEllipse1: " << totalTime1.count() << " ms\n";
                std::cout << "Tiempo total drawEllipse2: " << totalTime2.count() << " ms\n";
                std::cout << "Tiempo promedio por elipse - drawEllipse1: " << totalTime1.count() / elipses.size() << " ms\n";
                std::cout << "Tiempo promedio por elipse - drawEllipse2: " << totalTime2.count() / elipses.size() << " ms\n";
            };
            test2 = false;
            elipses.clear();
            std:: cout << "Prueba Terminada\n";
            //for(int i = 0; i < m_nPixels; ++i)
            {
                //RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                //std::cout << x0 << " - " << y0 << " ~ " << x1 << " " << y1 << "\n";
                //int x0 = rand() % width;
                //int y0 = rand() % height;
                //int x1 = rand() % width;
                //int y1 = rand() % height;
                //drawlineInt(x0, y0, x1, y1, color);
                //setPixel(x, y, color);
            }
         };
    }

    void onKey(int key, int scancode, int action, int mods) 
    {
        if (action == GLFW_PRESS)
        {
            std::cout << "Key " << key << " pressed\n";
            if (key == GLFW_KEY_ESCAPE)
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            if (key == GLFW_KEY_ENTER)
                waitEnter = false;
                test2 = true;
        }
        else if (action == GLFW_RELEASE);
            //std::cout << "Key " << key << " released\n";
    }

    void onMouseButton(int button, int action, int mods) 
    {

        if (ImGui::GetIO().WantCaptureMouse) return;

        if (button >= 0 && button < 3) 
        {
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            if (action == GLFW_PRESS)
            {   
                mouseButtonsDown[button] = true;
                drawing = true;
                // Obtener posición actual del cursor
                //std::cout << "Mouse button " << button << " pressed at position (" << xpos << ", " << ypos << ")\n";
                //RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                //setPixel((int)xpos, height - 1 - (int)ypos, color);
                //setPixel((int)xpos+1, height - 1 - (int)ypos, color);
                //setPixel((int)xpos, height - 1 - (int)ypos+1, color);
                //setPixel((int)xpos+1 , height - 1 - (int)ypos+1, color);
                m_x0 = xpos;
                m_x1 = xpos;
                m_y0 = height - ypos - 1;
                m_y1 = height - ypos - 1;
            }
            else if (action == GLFW_RELEASE)
            {
                mouseButtonsDown[button] = false;
                drawing = false;
                //std::cout << "Mouse button " << button << " released at position (" << xpos << ", " << ypos << ")\n";
                //lines.push_back(Line{m_x0, m_y0, m_x1, m_y1, RGBA_Current});
                elipses.push_back(Elipse{ (int)m_x0, (int)m_y0, abs((int)(m_x1 - m_x0)), abs((int)(m_y1 - m_y0)), RGBA_Current });
            }
        }
    }
    void onCursorPos(double xpos, double ypos) 
    {
        if (mouseButtonsDown[0] || mouseButtonsDown[1] || mouseButtonsDown[2]) 
        {
            //std::cout << "Mouse move at (" << xpos << ", " << ypos << ")\n";
            m_x1 = xpos;
            m_y1 = height - ypos - 1;
        }
    }
};

int main() {
    CMyTest test;
    if (!test.setup()) {
        fprintf(stderr, "Failed to setup CPixelRender\n");
        return -1;
    }

    test.mainLoop();

    return 0;
}
