#include "heightmap.h"

heightmap::heightmap()
{

}

heightmap::heightmap(std::string fileName)
{
    TextureClass = new Texture(fileName);
    construct(0.0f, 254.0f, 0.0f, 254.0f, 1); // Endre verdier??
}

heightmap::~heightmap()
{

}

void heightmap::init(GLint shader)
{
    mMatrixUniform = shader;

    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);


    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    // Her har Mo noe som jeg ikke har i for eksempel triangelsurface.cpp

    /*
    // 4th attribute buffer : RGB
    glVertexAttribPointer(3, 3,  GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)( 8 * sizeof( GLfloat ) ));
    glEnableVertexAttribArray(3);
     */

    glBindVertexArray(0);
}

void heightmap::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
}

void heightmap::construct(float xmin, float xmax, float ymin, float ymax, float h)
{
    //get the bitmap, bytesize, collum size
        unsigned char *bitmap = TextureClass->bitmap();
        int bytesize = TextureClass->bytesPrPixel();
        int collum = TextureClass->columns();
        //float cR = 20.0f; // used for the shading, increase number to get lighter tops
        float heightRatio = 25.f; // z height ratio. to change the z height independently. 256 makes height 1
        float scale = 1.0f; //scales the whole map. 1 is normal, 0.5 is half, 2.0 is twice the size

        //vertex positions for later calc.
        QVector3D Vert1; QVector3D Vert2; QVector3D Vert3;
        //normals calculations
        QVector3D v1; QVector3D v2; QVector3D v3;
        float r = 1.f, g = 0.f, b=0.f;

        for (auto y=ymin; y<ymax; y+= h)
        {
            for (auto x=xmin; x<xmax; x+=h)
            {
                //here we get the height from bitmap
                float z=bitmap[(int)(((y*256)+x )*bytesize)]/heightRatio;

                //for barysentric coordiantes
                //vertex 1
                QVector3D* temp = new QVector3D();
                temp->setX(x);
                temp->setY(z);
                temp->setZ(y);
                SurfacePosArrxzy [(int)x][(int)y] = temp;
                Vert1.setX(temp->x());
                Vert1.setY(temp->y());
                Vert1.setZ(temp->z());
                //----------------- vertex 2
                z=bitmap[(int)(((y*256)+x+1)*bytesize)]/heightRatio;
                //for barysentric coordiantes
                temp->setX(x+h);
                temp->setY(z);
                temp->setZ(y);
                SurfacePosArrxzy [(int)(x+h)][(int)y] = temp;
                Vert2.setX(temp->x());
                Vert2.setY(temp->y());
                Vert2.setZ(temp->z());
                //------------------vertex 3
                z=bitmap[(int)(((y*256)+x+1+ collum)* bytesize)]/heightRatio;

                //for barysentric coordiantes
                temp->setX(x+h);
                temp->setY(z);
                temp->setZ(y+h);
                SurfacePosArrxzy [(int)(x+h)][(int)(y+h)] = temp;
                Vert3.setX(temp->x());
                Vert3.setY(temp->y());
                Vert3.setZ(temp->z());


                //start calculating normals
                QVector3D tVec1 = Vert2 - Vert1;//vector 1
                QVector3D tVec2 = Vert3 - Vert1;// and vector 2 for normals calculation at point vert1
                v1 = -CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert1.x()*scale,Vert1.y()*scale,Vert1.z()*scale,v1.x(),v1.y(),v1.z(),0.f,0.f,r,g,b});//bottom left
                tVec1 = Vert3 - Vert2;//vector 1
                tVec2 = Vert1 - Vert2;// and vector 2 for normals calculation at point vert2
                v2 = -CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert2.x()*scale,Vert2.y()*scale,Vert2.z()*scale,v2.x(),v2.y(),v2.z(),0.f,0.f,r,g,b});//bottom right
                tVec1 = Vert1 - Vert3;//vector 1
                tVec2 = Vert2 - Vert3;// and vector 2 for normals calculation at point vert3
                v3 = -CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert3.x()*scale,Vert3.y()*scale,Vert3.z()*scale,v3.x(),v3.y(),v3.z(),0.f,0.f,r,g,b});//top right


                //--------------------------------------------------// starting on second triangle

                //first
                //switch first vertex in first triangle with the last vertex of second triangle, as they share points
                Vert3.setX(Vert1.x());
                Vert3.setY(Vert1.y());
                Vert3.setZ(Vert1.z());
                //Vertex 1 starts at last vertex of first triangle
                Vert1.setX(temp->x());
                Vert1.setY(temp->y());
                Vert1.setZ(temp->z());

                //------------------
                z=bitmap[(int)(((y*256)+x+ collum) * bytesize)]/heightRatio;
                //for barysentric coordinates
                temp->setX(x);
                temp->setY(z);
                temp->setZ(y+h);
                SurfacePosArrxzy [(int)x][(int)(y+h)] = temp;
                Vert2.setX(temp->x());
                Vert2.setY(temp->y());
                Vert2.setZ(temp->z());
                //-------------------
                // Never read?
                z=bitmap[(int)(((y*256)+x)*bytesize)]/heightRatio;

                //START NORMALS AND TRIANGLE TWO
                tVec1 = Vert2 - Vert1;
                tVec2 = Vert3 - Vert1;
                v1 =- CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert1.x()*scale,Vert1.y()*scale,Vert1.z()*scale,v1.x(),v1.y(),v1.z(),0.f,0.f,r,g,b});//bottom left
                tVec1 = Vert3 - Vert2;
                tVec2 = Vert1 - Vert2;
                v2 = -CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert2.x()*scale,Vert2.y()*scale,Vert2.z()*scale,v2.x(),v2.y(),v2.z(),0.f,0.f,r,g,b});//bottom right
                tVec1 = Vert1 - Vert3;
                tVec2 = Vert2 - Vert3;
                v3 = -CalculateNormalVector(tVec1,tVec2);
                mVertices.push_back(Vertex{Vert3.x()*scale,Vert3.y()*scale,Vert3.z()*scale,v3.x(),v3.y(),v3.z(),0.f,0.f,r,g,b});//top right

            }
        }

        mMatrix.setToIdentity();
}
