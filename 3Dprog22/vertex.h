// 3D-programmering 2022
#ifndef VERTEX_H
#define VERTEX_H
#include <fstream>

class Vertex {
   //! Overloaded ostream operator which writes all vertex data on an open textfile stream
   friend std::ostream& operator<< (std::ostream&, const Vertex&);

   //! Overloaded ostream operator which reads all vertex data from an open textfile stream
   friend std::istream& operator>> (std::istream&, Vertex&);

private:
   float m_xyz[3];       	// position i 3D
   float m_normal[3];    // normal in 3D or rgb colors
   float m_st[2];        	// texture coordinates if used

   // heightmaps
   float m_RGB[3];

public:
   Vertex();
   Vertex(float x, float y, float z, float r, float g, float b);
   Vertex(float x, float y, float z, float r, float g, float b, float u, float v);
   Vertex(float x, float y, float z, float n1, float n2, float n3, float u, float v, float r, float g, float b);
};


#endif // VERTEX_H
