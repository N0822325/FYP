#ifndef CAMERA
  #define CAMERA

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(__linux__)
  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#endif


class Camera
{
protected:
  glm::mat4 projectionMatrix;
  // glm::mat4 viewMatrix;

  glm::vec3 Position;

  glm::vec3 translation;
  glm::vec3 rotation;
  glm::vec3 scale;

public:
  Camera() {
    translation = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
  }

  virtual glm::mat4 getProjection() { return glm::mat4(1.0f); };
  virtual glm::mat4 getCamera() { return glm::mat4(1.0f); }
  virtual void setProjection(float x1, float x2, float y1, float y2, float z1, float z2) {};
  virtual void setProjection(float fov, float aspect, float nearPlane_, float farPlane_) {};

  virtual void cameraInput(XEvent events, int* mousePos) {};



  void setCamera(glm::vec3 position) {
    translation = position;
  }
  void moveCamera(glm::vec3 translate) {
    translation += translate;
  }
  void rotateCamera(glm::vec3 rotate) {
    rotation += rotate;
  }
  void scaleCamera(glm::vec3 scaleFactor) {
    scale += scaleFactor;
  }

};


// OrthoGraphic Camera Class
class OrthCamera : public Camera
{
private:
  glm::vec2 orthoX;
  glm::vec2 orthoY;
  glm::vec2 orthoZ;

  glm::vec3 midPoint;
  glm::vec3 focusPoint;

public:

  void setProjection(float x1, float x2, float y1, float y2, float z1, float z2) {
    orthoX = glm::vec2(x1,x2);
    orthoY = glm::vec2(y1,y2);
    orthoZ = glm::vec2(z1,z2);

    midPoint = glm::vec3( (x2-x1)/2, (y2-y1)/2, (z2-z1)/2 );
    focusPoint = midPoint;
  }
  glm::mat4 getProjection() {
    projectionMatrix = glm::ortho(orthoX.x, orthoX.y, orthoY.x, orthoY.y, orthoZ.x, orthoZ.y);
    return projectionMatrix;
  }

  glm::mat4 getCamera() {
    glm::mat4 viewMatrix = glm::mat4(1.0f);
              viewMatrix = glm::translate(viewMatrix, focusPoint);
              viewMatrix = glm::rotate(viewMatrix, rotation.x, glm::vec3(1,0,0));
              viewMatrix = glm::rotate(viewMatrix, rotation.y, glm::vec3(0,1,0));
              viewMatrix = glm::rotate(viewMatrix, rotation.z, glm::vec3(0,0,1));
              viewMatrix = glm::scale(viewMatrix, scale);
              viewMatrix = glm::translate(viewMatrix, translation);
              viewMatrix = glm::translate(viewMatrix, -focusPoint);

    return viewMatrix;
  }


  void changeFocusPoint(glm::vec3 focus) {
    focusPoint = focus;
  }
  // void setCamera(glm::vec3 position) {
  //   viewMatrix = glm::translate(glm::mat4(1.0f), position);
  //
  // }
  // void moveCamera(glm::vec3 translate) {
  //   viewMatrix = glm::translate(viewMatrix, translate);
  // }

};



// Perspective Camera Class
class ProCamera : public Camera
{
private:

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw = -90.0f;
    float Pitch = 0.0f;

    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    float mousePos[2] {0,0};

    bool moving = false;
    bool panning = false;

public:

    ProCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f))
    {
      Front = glm::vec3(0.0f, 0.0f, -1.0f);
      WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

      Position = position;

      updateCameraVectors();
    }

    glm::mat4 getCamera()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 getProjection() {
      projectionMatrix = glm::perspective(glm::radians(Zoom), 1.0f, 0.1f, 1000.0f);
      return projectionMatrix;
    }


    glm::vec3 getTranslation() {
      return Position;
    }
    float getYaw() {
      return Yaw;
    }
    float getPitch() {
      return Pitch;
    }


    virtual void cameraInput(XEvent events, int* mousePos)
    {
      if(events.type == KeyPress)
      {
        if (events.xkey.keycode == 25) {
          moveCam(glm::vec3(0,0,-1));
        }
        if (events.xkey.keycode == 39) {
          moveCam(glm::vec3(0,0,1));
        }
        if (events.xkey.keycode == 40) {
          moveCam(glm::vec3(1,0,0));
        }
        if (events.xkey.keycode == 38) {
          moveCam(glm::vec3(-1,0,0));
        }
      }

      if(events.type == ButtonPress) {
        switch (events.xbutton.button){
          case Button1:
          {
            moving = true;
            setMousePos(mousePos);
            break;
          }
          case Button3:
          {
            panning = true;
            setMousePos(mousePos);
            break;
          }
          case Button4:
            // Scroll Up
            ZoomCam(1.0f);
            break;

          case Button5:
            // Scroll Down
            ZoomCam(-1.0f);
            break;

          }
      }

      else if(events.type == ButtonRelease) {
        moving = false;
        panning = false;
      }

      if(events.type == MotionNotify && moving)
      {
        MouseMove(mousePos[0],mousePos[1]);
      }
      if(events.type == MotionNotify && panning)
      {
        MousePan(mousePos[0],mousePos[1]);
      }

    }




    void moveCam(glm::vec3 movement, float speed = 1)
    {
      float velocity = MovementSpeed * speed;
      if (movement[2] < 0)
          Position += Front * velocity;
      if (movement[2] > 0)
          Position -= Front * velocity;
      if (movement[0] < 0)
          Position -= Right * velocity;
      if (movement[0] > 0)
          Position += Right * velocity;
      if (movement[1] < 0)
          Position -= Up * velocity;
      if (movement[1] > 0)
          Position += Up * velocity;
    }

    void setPos(glm::vec3 pos)
    {
      Position = pos;
    }



private:

    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    void setMousePos(int* mouse)
    {
      mousePos[0] = mouse[0];
      mousePos[1] = mouse[1];
    }

    void MouseMove(float xPos, float yPos, bool constrainPitch = true)
    {
      float xoffset = mousePos[0] - xPos;
      float yoffset = mousePos[1] - yPos;

      mousePos[0] = xPos;
      mousePos[1] = yPos;

      xoffset *= MouseSensitivity;
      yoffset *= MouseSensitivity;

      Yaw   += xoffset;
      Pitch += yoffset;

      if (constrainPitch)
      {
          if (Pitch > 89.0f)
              Pitch = 89.0f;
          if (Pitch < -89.0f)
              Pitch = -89.0f;
      }

      updateCameraVectors();
    }
    void MousePan(float xPos, float yPos)
    {
      float xoffset = mousePos[0] - xPos;
      float yoffset = mousePos[1] - yPos;

      mousePos[0] = xPos;
      mousePos[1] = yPos;

      xoffset *= MouseSensitivity;
      yoffset *= MouseSensitivity;

      moveCam(glm::vec3(xoffset,yoffset,0), 0.1);

    }

    void ZoomCam(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
};



#endif /* end of include guard: RENDERER */
