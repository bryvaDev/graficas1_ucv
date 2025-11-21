#include "PixelRender.h"
#include <iostream>

struct line
{
    int x0, y0;
    int x1, y1;
    RGBA color;
};

class CMyTest : public CPixelRender
{
    int m_x0 = -1, m_y0 = -1, m_x1 = -1, m_y1 = -1;
    RGBA white = { 255, 255, 255, 255 };
    RGBA RGBA_Current = white;
    float colorArray[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool drawing = false;
    bool real = false;
    std::vector<line> lines;
public:
    CMyTest() {};
    ~CMyTest() {};
    void drawlineReal(int x0, int y0, int x1, int y1, RGBA color) 
    {
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
                y += m * sx;
            }
            setPixel(x1, y1, color);
        }
        else {
            float m = dx / dy;
            float x = x0;

            int sy = (y0 < y1) ? 1 : -1;
            for (int y = y0; y != y1; y += sy) {
                setPixel(static_cast<int>(std::round(x)), y, color);
                x += m * sy;
            }
            setPixel(x1, y1, color);
        }
    }
    void ellipsePoints(int cx, int cy, int x, int y, RGBA c) {
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
        // modalidad 1
        x = 0;
        y = b;
        int64_t a2 = a * a;
        int64_t b2 = b * b;
        d = b * (4 * b - 4 * a2) + a2;
        ellipsePoints(cx, cy, x, y, c);
        while (b2 * ((x + 1) << 1) < a2 * ((y<<1) - 1)) {
            if (d < 0) {
                d = d + ((b2 * ((x << 1) + 3)) << 2);
            }
            else {
                d = d + ((b2 * ((x << 1) + 3) + a2 * ((~(y << 1) + 1) + 2)) << 2);
                y = y - 1;
            };
            x = x + 1;
            ellipsePoints(cx, cy, x, y, c);
        };
        // modalidad 2
        d = b2 * (4 * x * x + 4 * x + 1) + a2 * (4 * y * y - 8 * y + 4) - 4 * a2 * b2;
        while (y > 0) {
            if (d < 0) {
                d = d + (((b2 * ((x << 1) + 2)) + a2 * ((~(y << 1)+1) + 3)) << 2);
                //d = d + 4 * (b * b * (2 * x + 2) + a * a * (-2 * y + 3));
                x = x + 1;
            }
            else {
                d = d + (a2 << 2) * ((~(y << 1) + 1) + 3);
            };
            y = y - 1;
            ellipsePoints(cx, cy, x, y, c);
        };
        if (b==0) {
            //std::cout << "entra";
            drawlineInt(cx-a, cy, cx+a, cy, c);
        };

    };

    void drawEllipse2Optimized(int cx, int cy, int64_t a, int64_t b, RGBA c) {
        if (a <= 0 || b <= 0) return;

        int64_t x = 0, y = b;
        int64_t a2 = a * a, b2 = b * b;

        // Términos precomputados para modalidad 1
        int64_t d1 = (b2 << 2) - ((a2 * b) << 2) + a2;
        int64_t deltaE1 = (b2 * 3) << 2;  // 4b²(2*0+3) inicial
        int64_t deltaSE1 = ((b2 * 3) + (a2 * ((-(b << 1)) + 2))) << 2;

        ellipsePoints(cx, cy, x, y, c);

        // Modalidad 1
        while ((b2 * ((x + 1) << 1)) < (a2 * ((y << 1) - 1))) {
            if (d1 < 0) {
                d1 += deltaE1;
                deltaE1 += (b2 << 3);  // +8b²
                deltaSE1 += (b2 << 3); // +8b²
            }
            else {
                d1 += deltaSE1;
                deltaE1 += (b2 << 3);  // +8b²
                deltaSE1 += (b2 << 3) + (a2 << 3); // +8b² + 8a²
                y--;
            }
            x++;
            ellipsePoints(cx, cy, x, y, c);
        }

        // Modalidad 2 - inicialización optimizada
        int64_t d2 = (b2 * ((x * x) << 2 + (x << 2) + 1)) +
            (a2 * ((y * y) << 2 - (y << 3) + 4)) -
            (a2 * b2 << 2);

        int64_t deltaS2 = (a2 * ((-(y << 1)) + 3)) << 2;
        int64_t deltaSE2 = ((b2 * ((x << 1) + 2)) + (a2 * ((-(y << 1)) + 3))) << 2;

        while (y > 0) {
            if (d2 < 0) {
                d2 += deltaSE2;
                deltaS2 += (a2 << 3);   // +8a²
                deltaSE2 += (a2 << 3) + (b2 << 3); // +8a² + 8b²
                x++;
            }
            else {
                d2 += deltaS2;
                deltaS2 += (a2 << 3);   // +8a²
                deltaSE2 += (a2 << 3);  // +8a²
            }
            y--;
            ellipsePoints(cx, cy, x, y, c);
        }

        if (b == 0) {
            drawlineInt(cx - a, cy, cx + a, cy, c);
        }
    }

    /*void drawlineInt(int x0, int y0, int x1, int y1, RGBA color) {
        int dx, dy, x, y, d, incN, incS, incE, incNE, incSE;
        dx = x1 - x0;
        dy = y1 - y0;
        d = dx - 2*dy;
        incE = 2 * dy;
        incNE = 2 * (dx - dy);
        x = x0;
        y = y0;
        setPixel(x, y, color);
        while (x < x1) {
            if (d <= 0) {
                d = d + incNE;
                y = y + 1;
            }
            else {
                d = d + incE;
            };
            x = x + 1;
            setPixel(x, y, color);
        };
    }*/
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
        ImGui::Separator();
        if(ImGui::Button("Add 1000 lines")) {
            for (int i = 0; i < 1000; i++) {
                RGBA color = { (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
                int x0 = rand() % width;
                int y0 = rand() % height;
                int x1 = rand() % width;
                int y1 = rand() % height;
                lines.push_back(line{ x0, y0, x1, y1, color });
            };
        };
        ImGui::Separator();
        ImGui::Checkbox("Arimetica Real", &real);
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
            drawEllipse2Optimized(m_x0, m_y1, abs(m_x1 - m_x0), abs(m_y1 - m_y0), RGBA_Current);
        };
        /*for (auto& line : lines) {
            if (!real) drawlineInt(line.x0, line.y0, line.x1, line.y1, line.color);
            if (real) drawlineReal(line.x0, line.y0, line.x1, line.y1, line.color);
        };*/
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
    }

    void onKey(int key, int scancode, int action, int mods) 
    {
        if (action == GLFW_PRESS)
        {
            std::cout << "Key " << key << " pressed\n";
            if (key == GLFW_KEY_ESCAPE)
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
        else if (action == GLFW_RELEASE)
            std::cout << "Key " << key << " released\n";
    }


    void onMouseButton(int button, int action, int mods) 
    {
        if (button >= 0 && button < 3) 
        {
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            if (action == GLFW_PRESS)
            {   
                mouseButtonsDown[button] = true;
                drawing = true;
                // Obtener posición actual del cursor
                std::cout << "Mouse button " << button << " pressed at position (" << xpos << ", " << ypos << ")\n";
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
                lines.push_back(line{m_x0, m_y0, m_x1, m_y1, RGBA_Current});
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
