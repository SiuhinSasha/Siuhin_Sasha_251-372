#include <iostream>
#include <algorithm>
#include <cmath>

class Rectangle {
private:
    double x1, y1, x2, y2;

public:
    Rectangle(double x1 = 0, double y1 = 0, double x2 = 1, double y2 = 1)
        : x1(x1), y1(y1), x2(x2), y2(y2) {
        if (x1 > x2) std::swap(this->x1, this->x2);
        if (y1 > y2) std::swap(this->y1, this->y2);
    }

    double getX1() const { return x1; }
    double getY1() const { return y1; }
    double getX2() const { return x2; }
    double getY2() const { return y2; }

    double getWidth() const { return x2 - x1; }
    double getHeight() const { return y2 - y1; }

    void move(double dx, double dy) {
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
    }

    void resize(double dw, double dh) {
        if (getWidth() + dw <= 0) {
            x2 = x1 + 1;
        } else {
            x2 += dw;
        }
        if (getHeight() + dh <= 0) {
            y2 = y1 + 1;
        } else {
            y2 += dh;
        }
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);
    }

    Rectangle boundingBox(const Rectangle& other) const {
        double rx1 = std::min(x1, other.x1);
        double ry1 = std::min(y1, other.y1);
        double rx2 = std::max(x2, other.x2);
        double ry2 = std::max(y2, other.y2);
        return Rectangle(rx1, ry1, rx2, ry2);
    }

    Rectangle intersection(const Rectangle& other) const {
        double rx1 = std::max(x1, other.x1);
        double ry1 = std::max(y1, other.y1);
        double rx2 = std::min(x2, other.x2);
        double ry2 = std::min(y2, other.y2);
        if (rx1 >= rx2 || ry1 >= ry2) {
            return Rectangle(0, 0, 0, 0);
        }
        return Rectangle(rx1, ry1, rx2, ry2);
    }

    bool isValid() const {
        return x1 < x2 && y1 < y2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
        os << "(" << r.x1 << ", " << r.y1 << ") - (" << r.x2 << ", " << r.y2 << ")";
        return os;
    }
};

int main() {
    Rectangle rects[10];
    int count = 0;
    int choice;

    do {
        std::cout << "\n===== RECTANGLE MENU =====\n";
        std::cout << "1. Create rectangle\n";
        std::cout << "2. Move rectangle\n";
        std::cout << "3. Resize rectangle\n";
        std::cout << "4. Show rectangle\n";
        std::cout << "5. Show all rectangles\n";
        std::cout << "6. Bounding box of two rectangles\n";
        std::cout << "7. Intersection of two rectangles\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            if (count >= 10) {
                std::cout << "Max 10 rectangles reached.\n";
                break;
            }
            double x1, y1, x2, y2;
            std::cout << "Enter x1 y1 x2 y2: ";
            std::cin >> x1 >> y1 >> x2 >> y2;
            rects[count] = Rectangle(x1, y1, x2, y2);
            std::cout << "Rectangle #" << count << " created: " << rects[count] << "\n";
            count++;
            break;
        }
        case 2: {
            int idx;
            double dx, dy;
            std::cout << "Enter rectangle index (0-" << count - 1 << "): ";
            std::cin >> idx;
            if (idx < 0 || idx >= count) {
                std::cout << "Invalid index.\n";
                break;
            }
            std::cout << "Enter dx dy: ";
            std::cin >> dx >> dy;
            rects[idx].move(dx, dy);
            std::cout << "Moved: " << rects[idx] << "\n";
            break;
        }
        case 3: {
            int idx;
            double dw, dh;
            std::cout << "Enter rectangle index (0-" << count - 1 << "): ";
            std::cin >> idx;
            if (idx < 0 || idx >= count) {
                std::cout << "Invalid index.\n";
                break;
            }
            std::cout << "Enter dw dh: ";
            std::cin >> dw >> dh;
            rects[idx].resize(dw, dh);
            std::cout << "Resized: " << rects[idx] << "\n";
            break;
        }
        case 4: {
            int idx;
            std::cout << "Enter rectangle index (0-" << count - 1 << "): ";
            std::cin >> idx;
            if (idx < 0 || idx >= count) {
                std::cout << "Invalid index.\n";
                break;
            }
            std::cout << "Rectangle #" << idx << ": " << rects[idx] << "\n";
            std::cout << "  Width: " << rects[idx].getWidth()
                      << ", Height: " << rects[idx].getHeight() << "\n";
            break;
        }
        case 5: {
            for (int i = 0; i < count; i++) {
                std::cout << "#" << i << ": " << rects[i] << "\n";
            }
            break;
        }
        case 6: {
            int i1, i2;
            std::cout << "Enter two indices (0-" << count - 1 << "): ";
            std::cin >> i1 >> i2;
            if (i1 < 0 || i1 >= count || i2 < 0 || i2 >= count) {
                std::cout << "Invalid index.\n";
                break;
            }
            Rectangle bb = rects[i1].boundingBox(rects[i2]);
            std::cout << "Bounding box: " << bb << "\n";
            break;
        }
        case 7: {
            int i1, i2;
            std::cout << "Enter two indices (0-" << count - 1 << "): ";
            std::cin >> i1 >> i2;
            if (i1 < 0 || i1 >= count || i2 < 0 || i2 >= count) {
                std::cout << "Invalid index.\n";
                break;
            }
            Rectangle inter = rects[i1].intersection(rects[i2]);
            if (!inter.isValid()) {
                std::cout << "Rectangles do not intersect.\n";
            } else {
                std::cout << "Intersection: " << inter << "\n";
            }
            break;
        }
        case 0:
            std::cout << "Exiting.\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
