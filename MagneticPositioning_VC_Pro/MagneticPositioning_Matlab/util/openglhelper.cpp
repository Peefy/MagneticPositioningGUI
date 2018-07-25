
#include "stdafx.h"

#include <math.h>
#include <windows.h>
#include <gl/freeglut.h>  

#include "openglhelper.h"
#include "config.h"

#define ABS(x) (((x) > 0) ? (x) : (-x))
#define RANGE(x, min, max)   ((x)<(min) ? (min) : ( (x)>(max) ? (max):(x) ))
#define RANGE_AXIS_VALUE(x, last)  (ABS(x) > AXIS_MAX_VALUE ? last : x)

namespace util
{
    
    static float rotate = 0;
    static int times = 0;

    void DrawCubeWithColor(float length, float r = CUBE_COLOR_R, 
        float b = CUBE_COLOR_B, float g = CUBE_COLOR_G)
    {
        auto object = gluNewQuadric();
        gluQuadricNormals(object, GLU_SMOOTH); // 使用平滑法线
        gluQuadricTexture(object, GL_TRUE);
        glPushMatrix();
        //画第一个面 
        glColor3f(r, b, g); //     
        glRectf(0, 0, length, length);
        //画第二个面 
        //glColor3f(0, b, g); //   
        glRotatef(-90, 0, 1, 0);
        glRectf(0, 0, length, length);
        glRotatef(90, 0, 1, 0);
        //画第三个面 
       // glColor3f(r, b, 0); //   
        glRotatef(90, 1, 0, 0);
        glRectf(0, 0, length, length);
        glRotatef(-90, 1, 0, 0);
        //画第四个面 
        //glColor3f(0, 0, g); //   
        glTranslatef(0, 0, length);
        glRectf(0, 0, length, length);
        //画第五个面
        //glColor3f(0, b, 0); //   
        glTranslatef(length, length, -length);
        glRotatef(180, 0, 0, 1);
        glRotatef(-90, 0, 1, 0);
        glRectf(0, 0, length, length);
        glRotatef(90, 0, 1, 0);
        //画第六个面
        //glColor3f(r, 0, 0); // 
        glRotatef(90, 1, 0, 0);
        glRectf(0, 0, length, length);
        glRotatef(-90, 1, 0, 0);
        //绘图回到原点
        glRotatef(-180, 0, 0, 1);
        glTranslatef(-length, -length, 0);

        glPopMatrix();
        gluDeleteQuadric(object);

    }
 
    void DrawCubeWithPara(double x = 0, double y = 0, double z = 0, 
        double roll = 0, double yaw = 0, double pitch = 0)
    {
        auto movecenter = CUBE_WIDTH / 2.0;
        glPushMatrix();
        glTranslatef(-z / AXIS_MAX_VALUE, -x / AXIS_MAX_VALUE, y / AXIS_MAX_VALUE);
        glRotatef(roll, 1, 0, 0);
        glRotatef(yaw, 0, 1, 0);
        glRotatef(pitch, 0, 0, 1);
        glTranslatef(-movecenter, -movecenter, -movecenter);

        DrawCubeWithColor(CUBE_WIDTH);
        glPopMatrix();
    }

    void DrawSolidBoxWithColor(float a, float b, float c, 
        float red = CUBE_COLOR_R, 
        float blue = CUBE_COLOR_B, 
        float green = CUBE_COLOR_B)
    {
        auto object = gluNewQuadric();
        gluQuadricNormals(object, GLU_SMOOTH); 
        gluQuadricTexture(object, GL_TRUE);
        glPushMatrix();
        //画第一个面 
        glColor3f(red, blue, green);      
        glRectf(0, 0, a, b);
        //画第二个面 
        glRotatef(-90, 0, 1, 0);
        glRectf(0, 0, c, b);
        glRotatef(90, 0, 1, 0);
        //画第三个面 
        glRotatef(90, 1, 0, 0);
        glRectf(0, 0, a, c);
        glRotatef(-90, 1, 0, 0);
        //画第四个面 
        glTranslatef(0, 0, c);
        glRectf(0, 0, a, b);
        //画第五个面
        glTranslatef(a, b, -c);
        glRotatef(180, 0, 0, 1);
        glRotatef(-90, 0, 1, 0);
        glRectf(0, 0, c, b);
        glRotatef(90, 0, 1, 0);
        //画第六个面
        glRotatef(90, 1, 0, 0);
        glRectf(0, 0, a, c);
        glRotatef(-90, 1, 0, 0);
        //绘图回到原点
        glRotatef(-180, 0, 0, 1);
        glTranslatef(-a, -b, 0);

        glPopMatrix();
        gluDeleteQuadric(object);

    }

    void DrawAxisX(float red = AXIS_COLOR_R, float blue = AXIS_COLOR_B, 
        float green = AXIS_COLOR_G)
    {
        glPushMatrix();
        glTranslatef(-AXIS_LENGTH, 0, 0);
        DrawSolidBoxWithColor(AXIS_LENGTH * 2, AXIS_WIDTH, AXIS_WIDTH, red, blue, green);
        glTranslatef(AXIS_LENGTH, 0, 0);
        glPopMatrix();
    }

    void DrawAxisY(float red = AXIS_COLOR_R, float blue = AXIS_COLOR_B, 
        float green = AXIS_COLOR_G)
    {
        glPushMatrix();
        glTranslatef(0, -AXIS_LENGTH, 0);
        DrawSolidBoxWithColor(AXIS_WIDTH, AXIS_LENGTH * 2, AXIS_WIDTH, red, blue, green);
        glTranslatef(0, AXIS_LENGTH, 0);
        glPopMatrix();
    }

    void DrawAxisZ(float red = AXIS_COLOR_R, float blue = AXIS_COLOR_B, 
        float green = AXIS_COLOR_G)
    {
        glPushMatrix();
        glTranslatef(0, 0, -AXIS_LENGTH);
        DrawSolidBoxWithColor(AXIS_WIDTH, AXIS_WIDTH, AXIS_LENGTH * 2, red, blue, green);
        glTranslatef(0, 0, AXIS_LENGTH);
        glPopMatrix();
    }

    void DrawAxis(float red = AXIS_COLOR_R, float blue = AXIS_COLOR_B, 
        float green = AXIS_COLOR_G)
    {      
        DrawAxisX(red, blue, green);
        DrawAxisY(red, blue, green);
        DrawAxisZ(red, blue, green);
    }

    void DrawAxisAccodingToXYZ(double x = 0, double y = 0, double z = 0, 
        double roll = 0, double yaw = 0, double pitch = 0)
    {
        if (x >= 0 && y >= 0 && z >= 0)
        {
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
        }
        else if (x < 0 && y >= 0 && z >= 0)
        {
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
        }
        else if (x < 0 && y < 0 && z >= 0)
        {       
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G); 
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);       
        }
        else if (x < 0 && y < 0 && z < 0)
        {
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);          
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
        }
        else if (x < 0 && y >= 0 && z < 0)
        {
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);       
        }
        else if (x >= 0 && y < 0 && z < 0)
        {
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);                 
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
        }
        else if (x >= 0 && y >= 0 && z < 0)
        {
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);        
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
        }
        else if (x >= 0 && y < 0 && z >= 0)
        {
            DrawCubeWithPara(x, y, z, roll, yaw, pitch);
            DrawAxisY(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);          
            DrawAxisX(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
            DrawAxisZ(AXIS_COLOR_R, AXIS_COLOR_B, AXIS_COLOR_G);
        }
    }

    double x = 0;
    double y = 0;
    double z = 0;
    double roll = 0;
    double yaw = 0;
    double pitch = 0;

    void renderScene()
    {
        x = RANGE_AXIS_VALUE(repose[0] * 10, x);
        y = RANGE_AXIS_VALUE(repose[1] * 10, y);
        z = RANGE_AXIS_VALUE(repose[2] * 10, z);
        roll = repose[3];
        yaw = repose[4];
        pitch = repose[5];

        glClearColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_B, BACKGROUND_COLOR_G, BACKGROUND_COLOR_A);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();

        glTranslatef(CAMERA_TRANSLATE_X, CAMERA_TRANSLATE_Y, CAMERA_TRANSLATE_Z);  
        glScalef(CAMERA_SCALE_X, CAMERA_SCALE_Y, CAMERA_SCALE_Z);    

        times++;
        
        if (times % 100 == 0)
        {
            rotate += 0.3;
        }
#ifdef IS_DEMO  
        x = sin(times / 5000.0);
        y = cos(times / 5000.0);
        yaw += 0.01;
        if (yaw >= 360)
            yaw = 0;
#endif
        if (flag == 1)
        {
            glRotatef(CAMERA_ROTATE_Y, 0, 1, 0);
            glRotatef(CAMERA_ROTATE_X, 1, 0, 0);
            glRotatef(CAMERA_ROTATE_Z, 0, 0, 1);

            DrawAxisAccodingToXYZ(x, y, z, roll, yaw, pitch);
        }


        glPopMatrix();
        glutSwapBuffers();
    }

    void MagPositionOpenGlInit(int argc, char ** argv)
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
        glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
        glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
        glutCreateWindow(WINDOW_TITLE);
        glutDisplayFunc(renderScene);
        glutIdleFunc(renderScene);
        glutMainLoop();
    }

}


