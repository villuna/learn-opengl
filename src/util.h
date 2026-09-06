#pragma once

struct colour {
    float r, g, b, a;

    colour(float r, float g, float b, float a) noexcept : r(r), g(g), b(b), a(a) {}
    colour() noexcept : colour(0, 0, 0, 0) {}
    colour(float r, float g, float b) noexcept : colour(r, g, b, 1.0) {}

    colour(int r, int g, int b, int a) noexcept
        : r((float)r / 255), g((float)g / 255), b((float)b / 255), a((float) a/255)
    {}
    colour(int r, int g, int b) : colour(r, g, b, 255) {}
};

void set_clear_colour(colour c);
