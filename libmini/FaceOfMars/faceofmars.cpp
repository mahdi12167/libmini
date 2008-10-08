// FaceOfMars Demo for libmini-terrain-engine with Irrlicht v.0.7
// Linux OpenGL version v.0.6 as of 25.Jan.2005
//
// v.0.1: initial Windows version (zenprogramming)
// v.0.2: fixed texture scaling and offset (zenprogramming)
// v.0.3: original Windows version (zenprogramming)
// v.0.4: fixed TCoord, FPS viewer, bounding box, texture orientation (roettger)
// v.0.5: fixed target direction (roettger)
// v.0.6: upgraded to libMini v5.2 (roettger)
//
// try to strafe with the cursor keys!
//
// libmini by Stefan Roettger
// http://www9.cs.fau.de/~roettger
//
// example and Irrlicht-interface-functions by zenprogramming
// mailto: zenprogramming at yahoo dot de
// http://zenprogramming.tripod.com

#include <stdio.h>
#include <stdlib.h>

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#include "ministub.hpp"


// mesh generation stuff:

int triangleindex = -1;
int vertex0 = 0;

SMeshBuffer *buffer = 0;
S3DVertex vertex;

// height field definition:

int size=0;
float dim=1.0f;
float scale=0.2f;
float res=5.0E5f;


// libmini callbacks:

// libmini->Irrlicht-interface-functions
// this function starts every trianglefan
void mybeginfan()
   {
   triangleindex=0;
   vertex0=buffer->getVertexCount();
   }

// libmini->Irrlicht-interface-functions
// this function computes one vertex
// vertices are already culled by the mini library
// so just add each vertex to a vertex buffer
void myfanvertex(float i,float y,float j)
   {
   vertex.Pos.set(dim*(i-size/2),
                  y*scale,
                  dim*(size/2-j));

   vertex.TCoords.set(1.0f-(float)i/(size-1),
                      1.0f-(float)j/(size-1));

   buffer->Vertices.push_back(vertex);

   if (triangleindex==2)
      {
      buffer->Indices.push_back(vertex0);
      buffer->Indices.push_back(buffer->getVertexCount()-2);
      buffer->Indices.push_back(buffer->getVertexCount()-1);
      return;
      }

   triangleindex++;
   }


// process keyboard events etc.
class MyEventReceiver : public IEventReceiver
   {
   public:

   virtual bool OnEvent(SEvent event)
      {
      if (event.EventType == EET_KEY_INPUT_EVENT)
         {
         switch(event.KeyInput.Key)
            {
            case KEY_KEY_Q:
               {
               exit(0);
               }
               return true;
            }
         }
      return false;
      }
   };

// application start
int main(int argc,char *argv[])
   {
   MyEventReceiver receiver;

   IrrlichtDevice *device = createDevice(EDT_OPENGL,core::dimension2d<s32>(640,480),16,false,false,false,&receiver);
   video::IVideoDriver *driver = device->getVideoDriver();
   scene::ISceneManager *smgr = device->getSceneManager();

   video::SMaterial material;

   material.Texture1=0;
   material.Lighting=false;
   driver->setMaterial(material);

   IImage *heightimage = driver->createImageFromFile("faceofmarsmap.bmp");
   size = heightimage->getDimension().Width;

   short int *hfield = new short int[size*size];

   for (int i=0; i<size; i++)
      for (int j=0; j<size; j++)
         hfield[i+j*size] = heightimage->getPixel(i,size-1-j).getRed();

   ministub *stub = new ministub(hfield,
                                 &size,&dim,scale,
                                 1.0f,0.0f,0.0f,0.0f,
                                 mybeginfan,myfanvertex);

   int lastfps = 0;

   vertex.Normal.set(0,1,0);
   vertex.TCoords.set(0,0);
   vertex.Color.set(255,255,255,255);

   scene::IAnimatedMeshSceneNode *terrainnode = 0;
   ITexture *terraintexture = driver->getTexture("faceofmars.jpg");

   scene::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0,50,50);
   camera->setPosition(vector3df(0.0f,0.0f,0.0f));
   camera->setTarget(vector3df(0.0f,0.0f,-1.0f));
   camera->setUpVector(vector3df(0.0f,1.0f,0.0f));

   while(device->run())
      {
      buffer = new SMeshBuffer();

      float aspect = camera->getAspectRatio();
      float fovy = 180.0f/PI*camera->getFOV();
      float nearp = camera->getNearValue();
      float farp = camera->getFarValue();

      vector3df pos = camera->getPosition();
      vector3df tgt = camera->getTarget();

      float glide = stub->getheight(pos.X,pos.Z)+10.0f*nearp;
      if (glide<0.0f) glide = 100.0f;

      camera->setPosition(vector3df(pos.X,glide,pos.Z));

      if (tgt.X==pos.X && tgt.Y==pos.Y && tgt.Z==pos.Z) tgt.Z-=1.0f;

      stub->draw(res,
                 pos.X,glide,pos.Z,
                 tgt.X-pos.X,tgt.Y-pos.Y,tgt.Z-pos.Z,
                 0.0f,1.0f,0.0f,
                 2.0f*fovy,aspect,
                 nearp,farp);

      SMesh* meshtmp = new SMesh();
      SAnimatedMesh *animatedMesh = new SAnimatedMesh();
      buffer->recalculateBoundingBox();
      meshtmp->addMeshBuffer(buffer);
      meshtmp->recalculateBoundingBox();
      animatedMesh->addMesh(meshtmp);
      animatedMesh->recalculateBoundingBox();
      scene::IAnimatedMesh *mesh = animatedMesh;
      buffer->drop();

      terrainnode = smgr->addAnimatedMeshSceneNode(mesh);
      terrainnode->setMaterialFlag(EMF_LIGHTING,false);
      // terrainnode->setMaterialFlag(EMF_WIREFRAME,true);
      terrainnode->setMaterialTexture(0,terraintexture);

      driver->beginScene(true,true,SColor(50,50,50,0));
      smgr->drawAll();
      driver->endScene();

      int fps = driver->getFPS();
      if (lastfps!=fps)
         {
         wchar_t tmp[1024];
         swprintf(tmp,1024,L"FaceOfMars Demo v.0.6: libmini and Irrlicht engine (fps:%d triangles:%d)",
                  fps,driver->getPrimitiveCountDrawn());
         device->setWindowCaption(tmp);
         lastfps = fps;
         }

      terrainnode->remove();
      meshtmp->drop();
      animatedMesh->drop();
      }

   delete(stub);
   delete(hfield);
   heightimage->drop();
   device->drop();

   return 0;
   }
