#pragma once
namespace esphome { struct Color { int v; Color(int r,int,int):v(r){} operator int() const {return v;} }; namespace display { constexpr int COLOR_OFF=0, COLOR_ON=1; struct BaseFont {}; enum class TextAlign { CENTER, CENTER_LEFT, CENTER_RIGHT }; } }
