#include <list>
#include <vector>
#include <QtGui>
#include <QtOpenGL/qgl.h>

#include <mini/minibase.h>
#include <mini/miniOGL.h>
#include <mini/miniearth.h>
#include <mini/miniterrain.h>
#include <mini/minirgb.h>
#include <mini/viewerbase.h>

#include "landscape.h"
#include "viewerconst.h"
#include "renderer.h"

using namespace std;

static const int numDiscVertex = 18;
static miniv3d discVertices[numDiscVertex+2];

void Renderer::initDiscVertices()
{
   int interval = 360/numDiscVertex;

   discVertices[0] = miniv3d(0, 0, 0);
   for (int i = 0; i < numDiscVertex; i++)
   {
      float angle = i * interval;

      float x = cos(angle * PI / 180.0f);
      float z = -sin(angle * PI / 180.0f);

      discVertices[i+1] = miniv3d(x, 0, z);
   }
   discVertices[numDiscVertex+1] = discVertices[1];
}

bool Renderer::isBoundingBoxVisible(const BoundingBox& bb)
{
   miniv3d points[8] =
   {
      bb.minPoint,
      miniv3d(bb.maxPoint.x, bb.minPoint.y, bb.minPoint.z),
      miniv3d(bb.maxPoint.x, bb.minPoint.y, bb.maxPoint.z),
      miniv3d(bb.minPoint.x, bb.minPoint.y, bb.maxPoint.z),
      miniv3d(bb.minPoint.x, bb.maxPoint.y, bb.minPoint.z),
      miniv3d(bb.maxPoint.x, bb.maxPoint.y, bb.minPoint.z),
      bb.maxPoint,
      miniv3d(bb.minPoint.x, bb.maxPoint.y, bb.maxPoint.z)
   };

   bool bVisible = true;
   for (int i = 0; i < 6; i++)
   {
      bool bPointInside = false;
      for (int j = 0; j < 8; j++)
      {
         miniv4d p(points[j].x, points[j].y, points[j].z, -1.0f);
         if (p * m_Camera.frustumPlanesGL[i] > 0)
         {
            bPointInside = true;
            break;
         }
      }
      if (!bPointInside)
      {
         bVisible = false;
         break;
      }
   }

   return bVisible;
}

bool Renderer::RayBoundingBoxIntersect(const BoundingBox& bb, const Ray& r)
{
   miniv3d parameters[2];
   parameters[0] = bb.minPoint;
   parameters[1] = bb.maxPoint;

   float t0 = 0.0f, t1 = MAXFLOAT;
   float tmin, tmax, tymin, tymax, tzmin, tzmax;

   tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
   tmax = (parameters[1-r.sign[0]].x - r.origin.x) * r.inv_direction.x;
   tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
   tymax = (parameters[1-r.sign[1]].y - r.origin.y) * r.inv_direction.y;

   if ( (tmin > tymax) || (tymin > tmax) )
      return false;

   if (tymin > tmin)
      tmin = tymin;
   if (tymax < tmax)
      tmax = tymax;

   tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
   tzmax = (parameters[1-r.sign[2]].z - r.origin.z) * r.inv_direction.z;

   if ( (tmin > tzmax) || (tzmin > tmax) )
      return false;

   if (tzmin > tmin)
      tmin = tzmin;
   if (tzmax < tmax)
      tmax = tzmax;

   return ( (tmin < t1) && (tmax > t0) );
}

void Renderer::renderHUD()
{
   minilayer *nst=viewer->getearth()->getnearest(m_Camera.pos);

   double cameraElev=viewer->getearth()->getterrain()->getheight(m_Camera.pos);
   if (cameraElev==-MAXFLOAT) cameraElev=0.0f;

   minicoord cameraPosLLH = nst->map_g2t(m_Camera.pos);
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR) cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   // draw crosshair
   {
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glBindTexture(GL_TEXTURE_2D, m_CrosshairTextureId);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_CULL_FACE);

      float lx = 60.0f/(window->width());
      float ly = 60.0f/(window->height());

      glColor4f(1.0f,1.0f,1.0f, 1.0f);

      glBegin(GL_QUADS);
      glTexCoord2f(0.0f,  0.0f); glVertex2f(-lx, ly); // P0
      glTexCoord2f(1.0f,  0.0f); glVertex2f(lx, ly); // P1
      glTexCoord2f(1.0f,  1.0f); glVertex2f(lx, -ly); // P2
      glTexCoord2f(0.0f,  1.0f);  glVertex2f(-lx, -ly); // P3
      glEnd();

      glEnable(GL_DEPTH_TEST);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_CULL_FACE);
   }

   QString str;
   const QColor colorRed(255, 255, 255);
   window->qglColor(colorRed);
   int x = 10;
   int y = window->height() - 20;
   int line_space = -16;
   int second_column_offset = 90;

   str.sprintf("Camera Data");
   drawText(x, y, str);
   y+=line_space;

   str.sprintf("=============");
   drawText(x, y, str);
   y+=line_space;

   str.sprintf("Latitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.x/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Longitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.y/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Altitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f m", cameraPosLLH.vec.z);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Heading:");
   drawText(x, y, str);
   str.sprintf("%03d", (int)m_Camera.heading);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Pitch:");
   drawText(x, y, str);
   str.sprintf("%-6.0f", m_Camera.pitch);
   drawText(x+second_column_offset, y, str);
   y+=line_space;
}

void Renderer::drawCameraFrustum()
{
   glColor3f(1.0f,0.0f,0.0f);
   miniv3d* p = m_Camera.frustumPointsGL;
   drawLine(p[0], p[1]);
   drawLine(p[1], p[2]);
   drawLine(p[2], p[3]);
   drawLine(p[3], p[0]);

   drawLine(p[4], p[5]);
   drawLine(p[5], p[6]);
   drawLine(p[6], p[7]);
   drawLine(p[7], p[4]);

   drawLine(p[0], p[4]);
   drawLine(p[1], p[5]);
   drawLine(p[2], p[6]);
   drawLine(p[3], p[7]);
}

void Renderer::drawScreenAlignedDisc(const miniv3d& pos, float scale, const miniv4d& color)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glRotatef(-m_Camera.heading, 0.0, 1.0, 0.0);
   glRotatef(90.0f - m_Camera.pitch, 1.0, 0.0, 0.0);
   glScalef(scale, scale, scale);

   glColor4f(color.x, color.y, color.z, color.w);
   if (color.w != 1.0f)
   {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   glBegin(GL_TRIANGLE_FAN);
   for (int i = 0; i < numDiscVertex+2; i++)
   {
      glVertex3f(discVertices[i].x, discVertices[i].y, discVertices[i].z);
   }
   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
}

void Renderer::drawScreenAlignedQuadWithTex(const miniv3d& pos, float scale, const miniv4d& color, GLuint texId)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glRotatef(-m_Camera.heading, 0.0, 1.0, 0.0);
   glRotatef(90.0f - m_Camera.pitch, 1.0, 0.0, 0.0);
   glScalef(scale, scale, scale);

   glColor4f(color.x, color.y, color.z, color.w);
   if (color.w != 1.0f)
   {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   if (texId != 0)
   {
      glBindTexture(GL_TEXTURE_2D, texId);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_CULL_FACE);
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 1/256.0f);
   }

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f,  0.0f); glVertex3f(-1.0f, 0.0f, 1.0f); // P0
   glTexCoord2f(1.0f,  0.0f); glVertex3f(1.0f, 0.0f, 1.0f); // P1
   glTexCoord2f(1.0f,  1.0f); glVertex3f(1.0f, 0.0f, -1.0f); // P2
   glTexCoord2f(0.0f,  1.0f);  glVertex3f(-1.0f, 0.0f, -1.0f); // P3
   glEnd();

   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);
   glDisable(GL_ALPHA_TEST);
}

void Renderer::drawDiscNoRotation(const miniv3d& pos, float scale, const miniv4d& color)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glScalef(scale, scale, scale);

   glColor4f(color.x, color.y, color.z, color.w);
   if (color.w != 1.0f)
   {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   glBegin(GL_TRIANGLE_FAN);
   for (int i = 0; i < numDiscVertex+2; i++)
   {
      glVertex3f(discVertices[i].x, discVertices[i].y, discVertices[i].z);
   }
   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
}

void Renderer::drawQuadWithTexNoRotation(const miniv3d& pos, float scale, const miniv4d& color, GLuint texId)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glScalef(scale, scale, scale);

   glColor4f(color.x, color.y, color.z, color.w);
   if (color.w != 1.0f)
   {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   if (texId != 0)
   {
      glBindTexture(GL_TEXTURE_2D, texId);
      glEnable(GL_TEXTURE_2D);
      glDisable(GL_CULL_FACE);
   }

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f,  0.0f); glVertex3f(-1.0f, 0.0f, 1.0f); // P0
   glTexCoord2f(1.0f,  0.0f); glVertex3f(1.0f, 0.0f, 1.0f); // P1
   glTexCoord2f(1.0f,  1.0f); glVertex3f(1.0f, 0.0f, -1.0f); // P2
   glTexCoord2f(0.0f,  1.0f);  glVertex3f(-1.0f, 0.0f, -1.0f); // P3
   glEnd();

   glPopMatrix();
   glDisable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);
}

void Renderer::drawCylinder(const miniv3d& pos, float scale, float height, const miniv4d& color)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glScalef(scale, 1.0f, scale);

   glColor4f(color.x, color.y, color.z, color.w);
   glBegin(GL_QUAD_STRIP);
   for (int i = 1; i < numDiscVertex+1; i++)
   {
      if (i == 1)
      {
         glVertex3f(discVertices[i].x, 0, discVertices[i].z);
         glVertex3f(discVertices[i].x, height, discVertices[i].z);
      }
      glVertex3f(discVertices[i+1].x, 0, discVertices[i+1].z);
      glVertex3f(discVertices[i+1].x, height, discVertices[i+1].z);
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   for (int i = 0; i < numDiscVertex+2; i++)
   {
      glVertex3f(discVertices[i].x, 0, discVertices[i].z);
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   for (int i = 0; i < numDiscVertex+2; i++)
   {
      glVertex3f(discVertices[i].x, height, discVertices[i].z);
   }
   glEnd();

   glPopMatrix();
}

void Renderer::drawBoundingBox(const BoundingBox& bb, miniv3d color)
{
   miniv3d p1 = bb.minPoint;
   miniv3d p2(bb.maxPoint.x, bb.minPoint.y, bb.minPoint.z);
   miniv3d p3(bb.maxPoint.x, bb.minPoint.y, bb.maxPoint.z);
   miniv3d p4(bb.minPoint.x, bb.minPoint.y, bb.maxPoint.z);
   miniv3d p5(bb.minPoint.x, bb.maxPoint.y, bb.minPoint.z);
   miniv3d p6(bb.maxPoint.x, bb.maxPoint.y, bb.minPoint.z);
   miniv3d p7 = bb.maxPoint;
   miniv3d p8(bb.minPoint.x, bb.maxPoint.y, bb.maxPoint.z);

   glColor3f(color.x,color.y,color.z);

   drawLine(p1, p2);
   drawLine(p2, p3);
   drawLine(p3, p4);
   drawLine(p4, p1);
   drawLine(p5, p6);
   drawLine(p6, p7);
   drawLine(p7, p8);
   drawLine(p8, p5);
   drawLine(p1, p5);
   drawLine(p2, p6);
   drawLine(p3, p7);
   drawLine(p4, p8);
}

void Renderer::drawText(float x, float y, QString& str, QColor color, bool bIsDoublePrint)
{
   int sx = x;
   int sy = window->height() - y;

   if (!bIsDoublePrint)
   {
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
   else
   {
      window->qglColor(QColor(0, 0, 0, 255));
      window->renderText(sx+2, sy+2, str);
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
}
