#ifndef __BOX_H__
#define __BOX_H__

#include <vector>
using namespace std;

class Box
{
public:
    Box(int x, int y, int z, int width, int height, int depth)
        : x(x),
          y(y),
          z(z),
          width(width),
          height(height),
          depth(depth)
    {
    }
    ~Box() {}
    inline int getX() { return x; }
    inline int getY() { return y; }
    inline int getZ() { return z; }
    inline int getWidth() { return width; }
    inline int getHeight() { return height; }
    inline int getDepth() { return depth; }
    Box intersect(const Box &other)
    {
        int minx = this->x < other.x ? other.x : this->x;
        int miny = this->y < other.y ? other.y : this->y;
        int minz = this->z < other.z ? other.z : this->z;

        int maxx = x + width > other.x + other.width ? other.x + other.width : x + width;
        int maxy = y + height > other.y + other.height ? other.y + other.height : y + height;
        int maxz = z + depth > other.z + other.depth ? other.z + other.depth : z + depth;

        return Box(minx, miny, minz, maxx - minx, maxy - miny, maxz - minz);
    }

    bool overlaps(const Box &other) const
    {
        if ((x > other.x + other.width) || (other.x > x + width))
        {
            return false;
        }

        if ((y > other.y + other.height) || (other.y > y + height))
        {
            return false;
        }

        if ((z > other.z + other.depth) || (other.z > z + depth))
        {
            return false;
        }
        return true;
    }

    // compute the part that is in this box but not in the other, when
    // other is contained inside this
    vector<Box> containedDifference(const Box &other)
    {
        vector<Box> result;
        int this_maxx, this_maxy, this_maxz, other_maxx, other_maxy, other_maxz;
        this_maxx = x + width;
        this_maxy = y + height;
        this_maxz = z + depth;

        other_maxx = other.x + other.width;
        other_maxy = other.y + other.height;
        other_maxz = other.z + other.depth;

        if (other.x > x)
        {
            result.push_back(Box(x, y, z, other.x - x, height, depth));
        }

        if (this_maxx > other_maxx)
        {
            result.push_back(Box(other_maxx, y, z,
                                 this_maxx - other_maxx, height, depth));
        }

        if (other.y > y)
        {
            result.push_back(Box(other.x, y, z, other.width, other.y - y, depth));
        }

        if (this_maxy > other_maxy)
        {
            result.push_back(Box(other.x, other_maxy, z, other.width, this_maxy - other_maxy, depth));
        }

        if (other.z > z)
        {
            result.push_back(Box(other.x, other.y, z, other.width, other.height, other.z - z));
        }

        if (this_maxz > other_maxz)
        {
            result.push_back(Box(other.x, other.y, other_maxz, other.width, other.height, this_maxz - other_maxz));
        }
        return result;
    }

private:
    int x, y, z, width, height, depth;
};

#endif