// Этот проект я сделал по приколу и поэтому код тут очень упрощенный и кривой
// Если вы хотите реальное 3д можете перейти в мой другой репозиторий
// https://github.com/Mimocake/Minecraft-Grib-Edition

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numbers>
#include <ranges>
#include <thread>
#include <vector>

using namespace std;

constexpr float pi = numbers::pi_v<float>;
constexpr int width_n = 121;
constexpr int width = 120;
constexpr int height = 30;
constexpr float asp = (float)width / (float)height;
constexpr float p_asp = 11.0f / 24.0f;
constexpr float fNear = 0.1;
constexpr float fFar = 1000;
constexpr float FOV = 90;

void draw_line(char* screen, int x1, int y1, int x2, int y2) {
    int x = x1;
    int y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = ((x2 - x1) == 0) ? 0 : ((x2 - x1) > 0 ? 1 : -1);
    int sy = ((y2 - y1) == 0) ? 0 : ((y2 - y1) > 0 ? 1 : -1);
    int in;
    if(dy > dx) {
        int t = dy;
        dy = dx;
        dx = t;
        in = 1;
    } else in = 0;
    float e = 2 * dy - dx;
    int a = 2 * dy;
    int b = 2 * dy - 2 * dx;
    if(y > 0 && y < height && x > 0 && x < width)
        screen[y * width_n + x] = '@';

    for(int i = 0; i < dx; i++) {
        if(e < 0) {
            if(in == 1) y += sy;
            else x += sx;
            e += a;
        } else {
            y += sy;
            x += sx;
            e += b;
        }
        if(y > 0 && y < height && x > 0 && x < width)
            screen[y * width_n + x] = '@';
    }
}

class mat4x4;

class vec3 {
public:
    float x{}, y{}, z{}, w{1};
    friend vec3 operator+(vec3 v1, vec3 v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }
    friend vec3 operator-(vec3 v1, vec3 v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }
    friend vec3 operator*(vec3 v, float f) { return {v.x * f, v.y * f, v.z * f}; }
    friend vec3 operator/(vec3 v, float f) { return {v.x / f, v.y / f, v.z / f}; }

    void operator+=(vec3 v) { x += v.x, y += v.y, z += v.z; }
    void operator-=(vec3 v) { x -= v.x, y -= v.y, z -= v.z; }
    void operator*=(float f) { x *= f, y *= f, z *= f; }
    void operator/=(float f) { x /= f, y /= f, z /= f; }

    friend float dot_prod(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
    friend vec3 cross_prod(vec3 a, vec3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
    void norm() { *this /= sqrt(x * x + y * y + z * z); }

    vec3 operator*(const mat4x4& m);
};

class mat4x4 {
public:
    float m[4][4]{};

    static mat4x4 rot_x(float fAngleRad) {
        mat4x4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[1][2] = sinf(fAngleRad);
        matrix.m[2][1] = -sinf(fAngleRad);
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static mat4x4 rot_y(float fAngleRad) {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][2] = sinf(fAngleRad);
        matrix.m[2][0] = -sinf(fAngleRad);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(fAngleRad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    static mat4x4 rot_z(float fAngleRad) {
        mat4x4 matrix;
        matrix.m[0][0] = cosf(fAngleRad);
        matrix.m[0][1] = sinf(fAngleRad);
        matrix.m[1][0] = -sinf(fAngleRad);
        matrix.m[1][1] = cosf(fAngleRad);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }
};

vec3 vec3::operator*(const mat4x4& m) {
    return {
        .x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0],
        .y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1],
        .z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2],
        .w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3],
    };
}

const mat4x4 proj_mat{
    ((1 / asp) / p_asp) / tanf(FOV / 2 / 180 * pi), 0, 0, 0,
    0, 1 / tanf(FOV / 2 / 180 * pi), 0, 0,
    0, 0, fFar / (fFar - fNear), 1,
    0, 0, -fFar* fNear / (fFar - fNear), 0 //
};

class Rect {
public:
    vec3 points[4]{};
    Rect() { }
    Rect(vec3 p1, vec3 p2, vec3 p3, vec3 p4)
        : points{p1, p2, p3, p4} { }

    void draw(char* screen) {
        draw_line(screen, (points[0].x + 1) * (width / 2), (points[0].y + 1) * (height / 2),
            (points[1].x + 1) * (width / 2), (points[1].y + 1) * (height / 2));
        draw_line(screen, (points[1].x + 1) * (width / 2), (points[1].y + 1) * (height / 2),
            (points[2].x + 1) * (width / 2), (points[2].y + 1) * (height / 2));
        draw_line(screen, (points[2].x + 1) * (width / 2), (points[2].y + 1) * (height / 2),
            (points[3].x + 1) * (width / 2), (points[3].y + 1) * (height / 2));
        draw_line(screen, (points[3].x + 1) * (width / 2), (points[3].y + 1) * (height / 2),
            (points[0].x + 1) * (width / 2), (points[0].y + 1) * (height / 2));
    }
    Rect project() {
        Rect temp = *this;
        for(auto&& point: temp.points) {
            point.z += 1.5;
            point = point * proj_mat;
            point.x /= point.z;
            point.y /= point.z;
        }
        return temp;
    }
    void rotate(float theta) {
        for(auto&& point: points) {
            point = point * mat4x4::rot_y(theta);
            point = point * mat4x4::rot_x(theta * 0.5);
            point = point * mat4x4::rot_z(theta * 0.5);
        }
    }
};

class Block {
public:
    Rect rects[6]{};
    Block(vec3 o)
        : rects{
              {{o.x + 0, o.y + 0, o.z + 0}, {o.x + 0, o.y + 1, o.z + 0}, {o.x + 1, o.y + 1, o.z + 0}, {o.x + 1, o.y + 0, o.z + 0}},
              {{o.x + 1, o.y + 0, o.z + 1}, {o.x + 1, o.y + 1, o.z + 1}, {o.x + 0, o.y + 1, o.z + 1}, {o.x + 0, o.y + 0, o.z + 1}},
              {{o.x + 0, o.y + 1, o.z + 0}, {o.x + 0, o.y + 1, o.z + 1}, {o.x + 1, o.y + 1, o.z + 1}, {o.x + 1, o.y + 1, o.z + 0}},
              {{o.x + 0, o.y + 0, o.z + 1}, {o.x + 0, o.y + 0, o.z + 0}, {o.x + 1, o.y + 0, o.z + 0}, {o.x + 1, o.y + 0, o.z + 1}},
              {{o.x + 0, o.y + 0, o.z + 1}, {o.x + 0, o.y + 1, o.z + 1}, {o.x + 0, o.y + 1, o.z + 0}, {o.x + 0, o.y + 0, o.z + 0}},
              {{o.x + 1, o.y + 0, o.z + 0}, {o.x + 1, o.y + 1, o.z + 0}, {o.x + 1, o.y + 1, o.z + 1}, {o.x + 1, o.y + 0, o.z + 1}},
    } { }
    Block project() {
        Block temp = *this;
        for(int i = 0; i < 6; i++) temp.rects[i] = temp.rects[i].project();
        return temp;
    }
    void rotate(float theta) {
        for(auto&& rect: rects) rect.rotate(theta);
    }
};

int main() {
    char screen[width_n * height + 1];
    auto clear = [&screen] {
        ranges::fill(screen, ' ');
        for(int w = 0; w < width_n * height; w += width_n) screen[w] = '\n';
        screen[width_n * height] = '\0';
    };

    Block block(vec3(-0.5, -0.5, -0.5));
    block.rotate(180 * 3.14159f / 180);
    for(;;) {
        clear();
        block.rotate(0.1 * 3.14159f / 180);
        Block temp = block.project();
        for(int i{}; auto&& rect: block.rects) {
            vec3 line1 = rect.points[1] - rect.points[0];
            vec3 line2 = rect.points[3] - rect.points[0];
            vec3 cam_dir(rect.points[0].x, rect.points[0].y, rect.points[0].z + 2);
            if(dot_prod(cam_dir, cross_prod(line1, line2)) < 0)
                temp.rects[i].draw(screen);
            ++i;
        }
        cout << screen << endl;
        this_thread::sleep_for(8ms); // 120fps
    }
    getchar();
}
