#include "GUIMyFrame1.h"

#include <omp.h>
#include <stdlib.h>

#include <cmath>
#include <vector>

GUIMyFrame1::GUIMyFrame1(wxWindow* parent) : MyFrame1(parent) {
    PrepareData(0);
}

void GUIMyFrame1::PrepareData(int fun) {
    int i;
    float x, y;
    srand(376257);
    std::vector<Point> vect;

    switch (fun) {
        case 0:
            vect.push_back(Point(-1, 1, 0));
            vect.push_back(Point(1, 1, 25));
            vect.push_back(Point(1, -1, 5));
            vect.push_back(Point(-1, -1, 25));
            vect.push_back(Point(0, 0, 15));
            points = vect;
            break;
        case 1:
            for (i = 0; i < 20; i++) {
                x = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                y = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                vect.push_back(Point(x, y, 200 * (float(rand()) / RAND_MAX)));
            }
            points = vect;
            break;
        case 2:
            for (i = 0; i < 100; i++) {
                x = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                y = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                vect.push_back(
                    Point(x, y, x * (2 * x - 7) * (2 * y + 1) + 2 * y));
            }
            points = vect;
            break;
        case 3:
            for (i = 0; i < 10; i++) {
                x = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                y = 4.8 * (float(rand()) / RAND_MAX) - 2.4;
                vect.push_back(Point(x, y, x * sin(2 * y)));
            }
            points = vect;
            break;
        case 4:
            for (i = 0; i < 100; i++) {
                x = 2 * (float(rand()) / RAND_MAX) - 1;
                y = 2 * (float(rand()) / RAND_MAX) - 1;
                vect.push_back(Point(x, y, sin(8 * (x * x + y * y))));
            }
            points = vect;
            break;
    }
}

float GUIMyFrame1::weight(float x, float y, int i) {
    return 1 /
           (pow(x - rotatedPoints[i].x, 2) + pow(y - rotatedPoints[i].y, 2));
}

void GUIMyFrame1::rotatePoints(float alpha) {
    rotatedPoints.clear();
    float cosalpha = cos(alpha);
    float sinalpha = sin(alpha);
    for (int i = 0; i < points.size(); i++) {
        float x = points[i].x;
        float y = points[i].y;
        rotatedPoints.push_back(Point(x * cosalpha - y * sinalpha,
                                      x * sinalpha + y * cosalpha,
                                      points[i].z));
    }
}

float GUIMyFrame1::shepard(int i, int j) {
    float x = fun == 4 ? 3.5 * i / SIZE - 1.75 : 5. * i / SIZE - 2.5;
    float y = fun == 4 ? 3.5 * j / SIZE - 1.75 : 5. * j / SIZE - 2.5;

    // float x = 5. * i / SIZE - 2.5;
    // float y = 5. * j / SIZE - 2.5;

    float top = 0;
    float bottom = 0;
    for (int k = 0; k < rotatedPoints.size(); k++) {
        if (rotatedPoints[k].x == x && rotatedPoints[k].y == y)
            return rotatedPoints[k].z;
        else {
            top += weight(x, y, k) * rotatedPoints[k].z;
            bottom += weight(x, y, k);
        }
    }
    return top / bottom;
}

void GUIMyFrame1::calculateValues() {
    vmin = FLT_MAX;
    vmax = -FLT_MAX;
#pragma omp parallel for
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            values[i][j] = shepard(i, j);
            if (values[i][j] < vmin) vmin = values[i][j];
            if (values[i][j] > vmax) vmax = values[i][j];
        }
    }
}

void GUIMyFrame1::Repaint() {
    wxClientDC dc1(m_panel1);
    wxBufferedDC dc(&dc1);
    dc.Clear();

    calculateValues();
    rotatePoints(1. * m_s_rotation->GetValue() / 360 * 2 * M_PI);

    dc.SetPen(*wxTRANSPARENT_PEN);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            float weight = (values[i][j] - vmin) / (vmax - vmin);
            if (color)
                dc.SetBrush(
                    wxBrush(wxColor(weight * 255, 0, 255 - weight * 255)));
            else
                dc.SetBrush(
                    wxBrush(wxColor(weight * 255, weight * 255, weight * 255)));
            dc.DrawRectangle(
                70 + j * 4 - i * 0.5,
                100 + (1. + tilt / 40) * i - (15 + 50 * weight) + (100 - tilt),
                4, 15 + 50 * weight);
        }
    }
}

void GUIMyFrame1::WxPanel_Repaint(wxUpdateUIEvent& event) { Repaint(); }

void GUIMyFrame1::m_button1_click(wxCommandEvent& event) {
    fun = 1;
    PrepareData(1);
    Repaint();
}

void GUIMyFrame1::m_button2_click(wxCommandEvent& event) {
    fun = 2;
    PrepareData(2);
    Repaint();
}

void GUIMyFrame1::m_button3_click(wxCommandEvent& event) {
    fun = 3;
    PrepareData(3);
    Repaint();
}

void GUIMyFrame1::m_button4_click(wxCommandEvent& event) {
    fun = 4;
    PrepareData(4);
    Repaint();
}

void GUIMyFrame1::m_cb_color_click(wxCommandEvent& event) {
    color = m_cb_color->GetValue();
    Repaint();
}

void GUIMyFrame1::m_s_rotation_onscroll(wxScrollEvent& event) {
    m_st_rotation->SetLabel(
        wxString::Format("Obrot: %d stopni.", m_s_rotation->GetValue()));
    Repaint();
}

void GUIMyFrame1::m_s_tilt_onscroll(wxScrollEvent& event) {
    tilt = m_s_tilt->GetValue();
    Repaint();
}
