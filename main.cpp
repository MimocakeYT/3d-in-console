// Ётот проект € сделал по приколу и поэтому код тут очень упрощенный и кривой
// ≈сли вы хотите реальное 3д можете перейти в мой другой репозиторий
// https://github.com/Mimocake/Minecraft-Grib-Edition

#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

using namespace std;

constexpr float pi = numbers::pi_v<float>;
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
        screen[y * width + x] = '@';

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
            screen[y * width + x] = '@';
    }
}

class vec3 {
public:
    float x, y, z, w;
    vec3()
        : x(0), y(0), z(0), w(1) { }
    vec3(float xx, float yy, float zz)
        : x(xx), y(yy), z(zz), w(1) { }
    friend vec3 operator+(vec3 v1, vec3 v2) { return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
    friend vec3 operator-(vec3 v1, vec3 v2) { return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
    friend vec3 operator*(vec3 v, float f) { return vec3(v.x * f, v.y * f, v.z * f); }
    friend vec3 operator/(vec3 v, float f) { return vec3(v.x / f, v.y / f, v.z / f); }
    void operator+=(vec3 v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    void operator-=(vec3 v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
    void operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
    }
    void operator/=(float f) { x /= f, y /= f, z /= f; }
    friend float dot_prod(vec3 v1, vec3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
    friend vec3 cross_prod(vec3 a, vec3 b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
    void norm() { *this /= sqrt(x * x + y * y + z * z); }
};

class mat4x4 {
public:
    float m[4][4] = {0};
    mat4x4() { }
    mat4x4(vector<vector<float>> v) {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                m[i][j] = v[i][j];
    }
};

vec3 mat4x4_mult(vec3 i, mat4x4 m) {
    vec3 v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
}

const mat4x4 proj_mat(std::vector<std::vector<float>>{
    {((1 / asp) / p_asp) / tanf(FOV / 2 / 180 * pi),                            0,                             0, 0},
    {                                             0, 1 / tanf(FOV / 2 / 180 * pi),                             0, 0},
    {                                             0,                            0,         fFar / (fFar - fNear), 1},
    {                                             0,                            0, -fFar* fNear / (fFar - fNear), 0}
});

mat4x4 rot_x(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 rot_y(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

mat4x4 rot_z(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

class Rect {
public:
    vec3 points[4] = {vec3()};
    Rect() {
        for(int i = 0; i < 4; i++) points[i] = vec3();
    }
    Rect(vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
        points[0] = p1;
        points[1] = p2;
        points[2] = p3;
        points[3] = p4;
    }
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
        for(int i = 0; i < 4; i++) {
            temp.points[i].z += 1.5;
            temp.points[i] = mat4x4_mult(temp.points[i], proj_mat);
            temp.points[i].x /= temp.points[i].z;
            temp.points[i].y /= temp.points[i].z;
        }
        return temp;
    }
    void rotate(float theta) {
        for(int i = 0; i < 4; i++) {
            points[i] = mat4x4_mult(points[i], rot_y(theta));
            points[i] = mat4x4_mult(points[i], rot_x(theta * 0.5));
            points[i] = mat4x4_mult(points[i], rot_z(theta * 0.5));
        }
    }
};

class Block {
public:
    Rect rects[6];
    Block(vec3 o) {
        rects[0] = Rect(vec3(o.x, o.y, o.z), vec3(o.x, o.y + 1, o.z), vec3(o.x + 1, o.y + 1, o.z), vec3(o.x + 1, o.y, o.z));
        rects[1] = Rect(vec3(o.x + 1, o.y, o.z + 1), vec3(o.x + 1, o.y + 1, o.z + 1), vec3(o.x, o.y + 1, o.z + 1), vec3(o.x, o.y, o.z + 1));
        rects[2] = Rect(vec3(o.x, o.y + 1, o.z), vec3(o.x, o.y + 1, o.z + 1), vec3(o.x + 1, o.y + 1, o.z + 1), vec3(o.x + 1, o.y + 1, o.z));
        rects[3] = Rect(vec3(o.x, o.y, o.z + 1), vec3(o.x, o.y, o.z), vec3(o.x + 1, o.y, o.z), vec3(o.x + 1, o.y, o.z + 1));
        rects[4] = Rect(vec3(o.x, o.y, o.z + 1), vec3(o.x, o.y + 1, o.z + 1), vec3(o.x, o.y + 1, o.z), vec3(o.x, o.y, o.z));
        rects[5] = Rect(vec3(o.x + 1, o.y, o.z), vec3(o.x + 1, o.y + 1, o.z), vec3(o.x + 1, o.y + 1, o.z + 1), vec3(o.x + 1, o.y, o.z + 1));
    }
    Block project() {
        Block temp = *this;
        for(int i = 0; i < 6; i++) temp.rects[i] = temp.rects[i].project();
        return temp;
    }
    void rotate(float theta) {
        for(int i = 0; i < 6; i++)
            rects[i].rotate(theta);
    }
};

int main() {
    char screen[width * height + 1];
    screen[width * height] = '\0';
    for(int i = 0; i < width * height; i++)
        screen[i] = ' ';
    Block block(vec3(-0.5, -0.5, -0.5));
    block.rotate(180 * 3.14159f / 180);
    for(;;) {
        for(int j = 0; j < width * height; j++)
            screen[j] = ' ';
        block.rotate(0.1 * 3.14159f / 180);
        Block temp = block.project();
        for(int i = 0; i < 6; i++) {
            vec3 line1 = block.rects[i].points[1] - block.rects[i].points[0];
            vec3 line2 = block.rects[i].points[3] - block.rects[i].points[0];
            vec3 cam_dir(block.rects[i].points[0].x, block.rects[i].points[0].y, block.rects[i].points[0].z + 2);
            if(dot_prod(cam_dir, cross_prod(line1, line2)) < 0)
                temp.rects[i].draw(screen);
        }
        cout << screen;
    }
    getchar();
}
