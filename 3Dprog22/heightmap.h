#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "texture.h"
#include "visualobject.h"

class heightmap : public VisualObject
{
public:
    heightmap();
    heightmap(std::string fileName);
    ~heightmap() override;
    void init(GLint shader) override;
    void draw() override;
    void construct(float xmin, float xmax, float ymin, float ymax, float h);
    Texture *TextureClass;
};

#endif // HEIGHTMAP_H
