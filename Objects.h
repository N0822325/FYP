#ifndef OBJECTS
  #define OBJECTS

#include "Renderer.h"
#include <math.h>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Object
{
private:
  VertexArray va;
  VertexArrayList ls;
  VertexBuffer vb;
  ElementBuffer eb;

  Shader* shader;
  Renderer* renderer;
  Texture* texture;

  glm::vec3 offset = glm::vec3(0.0f);

protected:
  int drawOption;

  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;

public:

  Object(float vertices[], int vertexSize, unsigned int indices[],
    Shader* shader_, Renderer* renderer_, Texture* texture_, int drawOption_)
    : vb(vertices, vertexSize), eb(indices, 36), texture(texture_), drawOption(drawOption_)
  {
    shader = shader_;
    renderer = renderer_;

    ls.push(3);
    ls.push(4);
    ls.push(2);
    va.addBuffer(vb, ls);

    translation = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(0,0,0);
  }

  void setOffset(glm::vec3 offset_) { offset = offset_; }

// Override for custom bound and collision calculations
  virtual std::vector<glm::vec2> getBounds() { return std::vector<glm::vec2> {glm::vec2(1.0f)}; }
  virtual bool overlaps(Object* other)
  {
    std::vector<glm::vec2> a = getBounds();
    std::vector<glm::vec2> b = other->getBounds();

    std::vector<glm::vec2>::iterator aa = a.begin();
    std::vector<glm::vec2>::iterator bb = b.begin();

    for(int i = 0; i < 3; i++)
    {

      if( (aa->x <= bb->y && aa->x >= bb->x) || (aa->y >= bb->x && aa->y <= bb->y) || (bb->x <= aa->y && bb->x >= aa->x) || (bb->y >= aa->x && bb->y <= aa->y) )
      {
        aa++; bb++;
      }
      else { return false; }
    }

    return true;
  }

  void setTexture(Texture* newTexture) {
    texture = newTexture;
  }
  void setColour(float r, float g, float b, float a) {
    texture->col[0] = r;
    texture->col[1] = g;
    texture->col[2] = b;
    texture->col[3] = a;
  }


  void setTranslation(glm::vec3 newLocation) {
    translation = newLocation;
  }
  void setTranslation(int x, int y, int z) {
    translation = glm::vec3(x,y,z);
  }
  void addTranslation(int x, int y, int z) {
    translation += glm::vec3(x,y,z);
  }

  void setRotate(int x, int y, int z) {
    setRotate((float)x,(float)y,(float)z);
  }
  void setRotate(float x, float y, float z) {
    rotation = glm::vec3(glm::radians(x), glm::radians(y), glm::radians(z));
  }
  void addRotate(int x, int y, int z) {
    addRotate(glm::radians((float)x),glm::radians((float)y),glm::radians((float)z));
  }
  void addRotate(float x, float y, float z) {
    rotation += glm::vec3(x,y,z);
  }

  void setScale(int factor) {
    scale = glm::vec3(factor);
  }
  void addScale(int factor) {
    scale += glm::vec3(factor);
  }

  glm::vec3 getCenter()
  {
    return translation;
  }

  glm::mat4 getTransform()
  {
    glm::mat4 transform = glm::mat4(1.0f);
              transform = glm::translate(transform, -offset);
              transform = glm::translate(transform, translation);
              transform = glm::rotate(transform, rotation.x, glm::vec3(1,0,0));
              transform = glm::rotate(transform, rotation.y, glm::vec3(0,1,0));
              transform = glm::rotate(transform, rotation.z, glm::vec3(0,0,1));
              transform = glm::scale(transform, scale);
              transform = glm::translate(transform, offset);
    return transform;
  }

  void draw()
  {
    renderer->draw(va, vb, eb, shader, texture, getTransform(), drawOption);
  }

};


class LineObj : public Object
{
private:
  float width;
  float height;
  float depth;

public:

  LineObj(float vertices[], int vertexSize, unsigned int indices[],
    Shader* shader_, Renderer* renderer_,
      float width_, float height_, float depth_)
    : Object(vertices, vertexSize, indices, shader_, renderer_, new Texture(""), GL_LINES)
  {
    width = width_;
    height = height_;
    depth = depth_;
  }


  int inline GetIntersection( float fDst1, float fDst2, glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit) {
    if ( (fDst1 * fDst2) >= 0.0f) return 0;
    if ( fDst1 == fDst2) return 0;
    Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
    return 1;
  }
  int inline InBox( glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis) {
    if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
    if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
    if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
    return 0;
  }
  int CheckLineBox( glm::vec3 B1, glm::vec3 B2, glm::vec3 L1, glm::vec3 L2, glm::vec3 &Hit)
  {
    if (L2.x < B1.x && L1.x < B1.x) { return false; }
    if (L2.x > B2.x && L1.x > B2.x) { return false; }
    if (L2.y < B1.y && L1.y < B1.y) { return false; }
    if (L2.y > B2.y && L1.y > B2.y) { return false; }
    if (L2.z < B1.z && L1.z < B1.z) { return false; }
    if (L2.z > B2.z && L1.z > B2.z) {  return false; }
    if (L1.x > B1.x && L1.x < B2.x &&
        L1.y > B1.y && L1.y < B2.y &&
        L1.z > B1.z && L1.z < B2.z)
        {Hit = L1;
        return true;}

    if ( (GetIntersection( L1.x-B1.x, L2.x-B1.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
      || (GetIntersection( L1.y-B1.y, L2.y-B1.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
      || (GetIntersection( L1.z-B1.z, L2.z-B1.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 ))
      || (GetIntersection( L1.x-B2.x, L2.x-B2.x, L1, L2, Hit) && InBox( Hit, B1, B2, 1 ))
      || (GetIntersection( L1.y-B2.y, L2.y-B2.y, L1, L2, Hit) && InBox( Hit, B1, B2, 2 ))
      || (GetIntersection( L1.z-B2.z, L2.z-B2.z, L1, L2, Hit) && InBox( Hit, B1, B2, 3 )))
    	return true;

    return false;
  }


  std::vector<glm::vec2> getBounds()
  {

    glm::vec2 x = glm::vec2(-1.0f,1.0f) * width;
    glm::vec2 y = glm::vec2(-1.0f,1.0f) * height;
    glm::vec2 z = glm::vec2(-1.0f,1.0f) * depth;

    glm::mat4 transform = glm::mat4(1.0f);
              transform = glm::rotate(transform, rotation.x, glm::vec3(1,0,0));
              transform = glm::rotate(transform, rotation.y, glm::vec3(0,1,0));
              transform = glm::rotate(transform, rotation.z, glm::vec3(0,0,1));
              transform = glm::scale(transform, scale);

    glm::vec4 a1 = glm::vec4(x.x, y.x, z.x, 1.0f) * transform;
    glm::vec4 a2 = glm::vec4(x.y, y.y, z.y, 1.0f) * transform;

    std::vector <glm::vec4> list {a1,a2};

    glm::vec3 largest = glm::vec3(0.0f);
    for(glm::vec4 v: list)
    {
      if (glm::abs(v.x) > largest.x) largest.x = glm::abs(v.x);
      if (glm::abs(v.y) > largest.y) largest.y = glm::abs(v.y);
      if (glm::abs(v.z) > largest.z) largest.z = glm::abs(v.z);
    }

    glm::vec2 out1 = glm::vec2(-(largest.x)+translation.x, largest.x+translation.x);
    glm::vec2 out2 = glm::vec2(-(largest.y)+translation.y, largest.y+translation.y);
    glm::vec2 out3 = glm::vec2(-(largest.z)+translation.z, largest.z+translation.z);

    std::vector<glm::vec2> out {out1,out2,out3};

    return out;
  }


  bool overlaps(Object* other)
  {

    std::vector<glm::vec2> b = other->getBounds();
    glm::vec3 boxMin = glm::vec3(b[0].x,b[1].x,b[2].x);
    glm::vec3 boxMax = glm::vec3(b[0].y,b[1].y,b[2].y);

    std::vector<glm::vec2> bounds = getBounds();
    glm::vec3 start =  glm::vec3(bounds[0].x, bounds[1].x, bounds[2].x);
    glm::vec3 end   =  glm::vec3(bounds[0].y, bounds[1].y, bounds[2].y);

    // std::cout << "start  " << start.x << ", " << start.y << ", " << start.z << std::endl;
    // std::cout << "end  " << end.x << ", " << end.y << ", " << end.z << std::endl;

    glm::vec3 hitpoint;
    return CheckLineBox(boxMin, boxMax, start, end, hitpoint);
  }


};


class PlaneObj : public Object
{
private:
  float width;
  float height;

public:

  PlaneObj(float vertices[], int vertexSize, unsigned int indices[],
    Shader* shader_, Renderer* renderer_, Texture* texture_,
      float width_, float height_)
    : Object(vertices, vertexSize, indices, shader_, renderer_, texture_, GL_TRIANGLES)
  {
    width = width_;
    height = height_;
  }

  std::vector<glm::vec2> getBounds()
  {

    glm::vec2 x = glm::vec2(-1.0f,1.0f) * width;
    glm::vec2 y = glm::vec2(-1.0f,1.0f) * height;

    glm::mat4 transform = glm::mat4(1.0f);
              transform = glm::rotate(transform, rotation.x, glm::vec3(1,0,0));
              transform = glm::rotate(transform, rotation.y, glm::vec3(0,1,0));
              transform = glm::rotate(transform, rotation.z, glm::vec3(0,0,1));
              transform = glm::scale(transform, scale);

    glm::vec4 a1 = glm::vec4(x.x, y.x, 0.0f, 1.0f) * transform;
    glm::vec4 a2 = glm::vec4(x.x, y.y, 0.0f, 1.0f) * transform;
    glm::vec4 a3 = glm::vec4(x.y, y.x, 0.0f, 1.0f) * transform;
    glm::vec4 a4 = glm::vec4(x.y, y.y, 0.0f, 1.0f) * transform;

    std::vector <glm::vec4> list {a1,a2,a3,a4};

    glm::vec3 largest = glm::vec3(0.0f);
    for(glm::vec4 v: list)
    {
      if (glm::abs(v.x) > largest.x) largest.x = glm::abs(v.x);
      if (glm::abs(v.y) > largest.y) largest.y = glm::abs(v.y);
      if (glm::abs(v.z) > largest.z) largest.z = glm::abs(v.z);
    }

    glm::vec2 out1 = glm::vec2(-(largest.x)+translation.x, largest.x+translation.x);
    glm::vec2 out2 = glm::vec2(-(largest.y)+translation.y, largest.y+translation.y);
    glm::vec2 out3 = glm::vec2(-(largest.z)+translation.z, largest.z+translation.z);

    std::vector<glm::vec2> out {out1,out2,out3};

    return out;
  }

  bool overlaps(LineObj* other)
  {
    return other->overlaps(this);
  }
};


class CubeObj : public Object
{
private:
  float width;
  float height;
  float depth;

public:

  CubeObj(float vertices[], int vertexSize, unsigned int indices[],
    Shader* shader_, Renderer* renderer_, Texture* texture_,
      float width_, float height_, float depth_)
    : Object(vertices, vertexSize, indices, shader_, renderer_, texture_, GL_TRIANGLES)
  {
    width = width_;
    height = height_;
    depth = depth_;
  }

  std::vector<glm::vec2> getBounds()
  {

    glm::vec2 x = glm::vec2(-1.0f,1.0f) * width;
    glm::vec2 y = glm::vec2(-1.0f,1.0f) * height;
    glm::vec2 z = glm::vec2(-1.0f,1.0f) * depth;

    glm::mat4 transform = glm::mat4(1.0f);
              transform = glm::rotate(transform, rotation.x, glm::vec3(1,0,0));
              transform = glm::rotate(transform, rotation.y, glm::vec3(0,1,0));
              transform = glm::rotate(transform, rotation.z, glm::vec3(0,0,1));
              transform = glm::scale(transform, scale);

    glm::vec4 a1 = glm::vec4(x.x, y.x, z.x, 1.0f) * transform;
    glm::vec4 a2 = glm::vec4(x.x, y.y, z.x, 1.0f) * transform;
    glm::vec4 a3 = glm::vec4(x.x, y.x, z.y, 1.0f) * transform;
    glm::vec4 a4 = glm::vec4(x.x, y.y, z.y, 1.0f) * transform;
    glm::vec4 a5 = glm::vec4(x.y, y.x, z.x, 1.0f) * transform;
    glm::vec4 a6 = glm::vec4(x.y, y.y, z.x, 1.0f) * transform;
    glm::vec4 a7 = glm::vec4(x.y, y.x, z.y, 1.0f) * transform;
    glm::vec4 a8 = glm::vec4(x.y, y.y, z.y, 1.0f) * transform;

    std::vector <glm::vec4> list {a1,a2,a3,a4,a5,a6,a7,a8};

    glm::vec3 largest = glm::vec3(0.0f);
    for(glm::vec4 v: list)
    {
      if (glm::abs(v.x) > largest.x) largest.x = glm::abs(v.x);
      if (glm::abs(v.y) > largest.y) largest.y = glm::abs(v.y);
      if (glm::abs(v.z) > largest.z) largest.z = glm::abs(v.z);
    }

    glm::vec2 out1 = glm::vec2(-(largest.x)+translation.x, largest.x+translation.x);
    glm::vec2 out2 = glm::vec2(-(largest.y)+translation.y, largest.y+translation.y);
    glm::vec2 out3 = glm::vec2(-(largest.z)+translation.z, largest.z+translation.z);

    std::vector<glm::vec2> out {out1,out2,out3};

    return out;
  }

  bool overlaps(LineObj* other)
  {
    return other->overlaps(this);
  }
};


class SphereObj : public Object
{
public:
  SphereObj(float vertices[], int vertexSize, unsigned int indices[],
    Shader* shader_, Renderer* renderer_, Texture* texture_,
      float radius)
    : Object(vertices, vertexSize, indices, shader_, renderer_, texture_, GL_TRIANGLES)
  {

  }
};



#endif
