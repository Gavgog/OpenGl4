/*========================================================================
* COSC 363  Computer Graphics (2019)
* Ray tracer 
* Gavin McGill
* 59048032
*=========================================================================
*/

//projected marks
//---------------------------------------------------
//BASIC:
//Lighting
//Shadows
//Reflections
//Box
//Planar surface
//
//Total       : 9/9
//---------------------------------------------------
//Extentions:
//AA          : 2pts
//Refreaction : 1pt
//Cone        : 1pt
//ITexturingNP: 1pt
//PTexturingNP: 1pt
//2 lights    : 1pt
//
//Total       : 7/7
//---------------------------------------------------
//Report      : 4/4 (hopefully)
//---------------------------------------------------
//Grand Total: 20/20


#include <iostream>
#include <cmath>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "shape.h"
#include "Plane.h"
#include <GL/glut.h>
#include "TextureBMP.h"

using namespace std;

const float WIDTH = 35.4;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 2000;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float ETA = 1.01;
const float transparency = 0.5;

TextureBMP texture1;
TextureBMP texture2;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
    glm::vec3 light2(20, 40, -6);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    glm::vec3 specular(0);
    glm::vec3 specular2(0);
    glm::vec3 white(1);
    glm::vec3 colorSum(0);

    float f = 20.0;
    float lightratio = 0.7;
    //----- ambient & diffuse lighting -----

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;//If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    
    glm::vec3 lightVector = light - ray.xpt;
    glm::vec3 lightVector2 = light2 - ray.xpt;
    
    
    float lightDistance = glm::length(lightVector); //distance to light
    float lightDistance2 = glm::length(lightVector2);
    
    
    lightVector = normalize(lightVector);
    lightVector2 = normalize(lightVector2);
    
    
    float lDotn = glm::dot(lightVector, normalVector);
    float lDotn2 = glm::dot(lightVector2, normalVector);
    
    glm::vec3 reflVector = reflect(-lightVector, normalVector);//rDotv
    glm::vec3 reflVector2 = reflect(-lightVector2, normalVector);//rDotv2
    
    float rDotv = glm::dot(reflVector,-ray.dir);
    rDotv = max(rDotv, 0.0f);
    float rDotv2 = glm::dot(reflVector2,-ray.dir);
    rDotv2 = max(rDotv, 0.0f);

    
    // shadows & specular---------------------------------------------------------
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);
    
    Ray shadow2(ray.xpt, lightVector2);
    shadow2.closestPt(sceneObjects);
    
    // light1 --
    if (rDotv >= 0) {specular = pow(rDotv,f)*white;}
    
    if ((lDotn<=0 )||((shadow.xindex>-1)&&(shadow.xdist < lightDistance))){//if in shadow
        colorSum += ambientCol*materialCol;
        }else{
        colorSum = (ambientCol*materialCol) + (lDotn*materialCol + specular)*lightratio;
    }
    
    // light2 --
    if (rDotv2 >= 0) {specular2 = pow(rDotv2,f)*white;}
    
    if ((lDotn2<=0 )||((shadow2.xindex>-1)&&(shadow2.xdist < lightDistance2))){//if in shadow
        colorSum += ambientCol*materialCol;
        }else{
        colorSum += (ambientCol*materialCol) + (lDotn2*materialCol + specular2)*(1-lightratio);
    }
    

     
    //refraction & tranceparency ------------------------------------------------------
    if(ray.xindex == 1 && step < MAX_STEPS){
        
        glm::vec3 refrDir = glm::refract(ray.dir,normalVector,1.0f/ETA);
        Ray refrRay(ray.xpt,refrDir);
        refrRay.closestPt(sceneObjects);
        if(refrRay.xindex == -1){
                return backgroundCol;
        }
        
        glm::vec3 normalVec = sceneObjects[refrRay.xindex]->normal(refrRay.xpt);
        
        glm::vec3 refrDir2 = glm::refract(refrDir, -normalVec, ETA);
        Ray refrRay2(refrRay.xpt,refrDir2);
        refrRay2.closestPt(sceneObjects);
        if(refrRay2.xindex == -1){
                return backgroundCol;
        }
        
        
        glm::vec3 refracCol = trace(refrRay2, step + 1);
        colorSum = colorSum * transparency + refracCol * (1-transparency);
        
        return colorSum;
    
	}
    
    
    //Patterns-------------------------------------------------------------------
    
    
     //sphere rexture
    if(ray.xindex == 3){
        glm::vec3 center(10.0, 4.0, -110.0);
        glm::vec3 diff = glm::normalize(ray.xpt-center);
        float why=0.5+asin(diff.y)/M_PI;
		float eks=(0.5-atan2(diff.z,diff.x)+M_PI)/(2*M_PI);
		colorSum = texture1.getColorAt(eks, why);
	}
    
    	if(ray.xindex == 6)
	{
		float s = (ray.xpt.x+150)/300;
		float t = (ray.xpt.y+20)/140;
		colorSum = texture2.getColorAt(s,t);
	}	

    //floor pattern
    if(ray.xindex == 4){
		int modx = (int)((ray.xpt.x + 50) /10) % 2;
		int modz = (int)((ray.xpt.z + 200) /10) % 2;
		
        
       if((modx && modz) || (!modx && !modz)){
		   sceneObjects[4]->setColor(glm::vec3(1,1,1));}
	   else{
		   sceneObjects[4]->setColor(glm::vec3(0,0,0));}
        }
     
    if(ray.xindex == 5){
       if((int(sin(ray.xpt.x)+sin(ray.xpt.y))-10)%2 == 0){
		   sceneObjects[5]->setColor(glm::vec3(1,0,0.5625));}
	   else{
		   sceneObjects[5]->setColor(glm::vec3(0.957,0.762,0.761));}
        }
     
    
        // Reflection ---------------------------------------------------------------------
    if(ray.xindex == 0 && step < MAX_STEPS)
     {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (0.8f*reflectedCol);
     }
    
    return colorSum;
}



glm::vec3 anti_aliasing(glm::vec3 eye, float pixel_size, float xp, float yp){
	
	glm::vec3 colorSum(0);
	glm::vec3 avg(0.25);
    //nums are
    //0.25
    //0.75
	
	Ray ray = Ray(eye, glm::vec3(xp + pixel_size * 0.25, yp + pixel_size * 0.25, -EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.25, yp + pixel_size * 0.75, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.75, yp + pixel_size * 0.25, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.75, yp + pixel_size * 0.75, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	colorSum*= avg;
	return colorSum;
}



glm::vec3 anti_aliasing_16times(glm::vec3 eye, float pixel_size, float xp, float yp){
	
	glm::vec3 colorSum(0);
	glm::vec3 avg(0.0625);
    
    //top left
    //0.125
    //0.375
    //0.625
    //0.875
	
	Ray ray = Ray(eye, glm::vec3(xp + pixel_size * 0.125, yp + pixel_size * 0.125, -EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.125, yp + pixel_size * 0.375, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.125, yp + pixel_size * 0.625, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.125, yp + pixel_size * 0.875, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
    
    //top right
    
    ray = Ray(eye, glm::vec3(xp + pixel_size * 0.375, yp + pixel_size * 0.125, -EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.375, yp + pixel_size * 0.375, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.375, yp + pixel_size * 0.625, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.375, yp + pixel_size * 0.875, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
    
    
    //bottom left
	
    ray = Ray(eye, glm::vec3(xp + pixel_size * 0.625, yp + pixel_size * 0.125, -EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.625, yp + pixel_size * 0.375, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.625, yp + pixel_size * 0.625, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.625, yp + pixel_size * 0.875, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
    //bottom right
    
    ray = Ray(eye, glm::vec3(xp + pixel_size * 0.875, yp + pixel_size * 0.125, -EDIST));		
    ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.875, yp + pixel_size * 0.375, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.875, yp + pixel_size * 0.625, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
	ray = Ray(eye, glm::vec3(xp + pixel_size * 0.875, yp + pixel_size * 0.875, -EDIST));
	ray.normalize();				
	colorSum+=trace(ray,1);
	
    
	colorSum*= avg;
	return colorSum;
	
	     
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
            
            
            //glm::vec3 col = anti_aliasing_Ntimes(eye,cellX,xp,yp,4);
            //glm::vec3 col = anti_aliasing_16times(eye,cellX,xp,yp);
            //glm::vec3 col = anti_aliasing(eye,cellX,xp,yp);
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}

void drawCube(float a, float b, float c, float d, float e, float f, glm::vec3 color){
    
    glm::vec3 g = glm::vec3(a,  b,  c);
    glm::vec3 h = glm::vec3(a+d,b,  c);
    glm::vec3 i = glm::vec3(a+d,b+f,c);
    glm::vec3 k = glm::vec3(a,  b+f,c);
    glm::vec3 l = glm::vec3(a+d,b,  c-e);
    glm::vec3 m = glm::vec3(a+d,b+f,c-e);
    glm::vec3 n = glm::vec3(a,  b+f,c-e);
    glm::vec3 o = glm::vec3(a,  b,  c-e);
    
    Plane *face1 = new Plane(g,h,i,k,color);
    Plane *face2 = new Plane(h,l,m,i,color);
    Plane *face3 = new Plane(l,o,n,m,color);
    Plane *face4 = new Plane(k,n,o,g,color);
    Plane *face5 = new Plane(k,i,m,n,color);
    Plane *face6 = new Plane(o,l,h,g,color);
    
    sceneObjects.push_back(face1);//frount
    sceneObjects.push_back(face2);//right
    sceneObjects.push_back(face3);//back
    sceneObjects.push_back(face4);//left
    sceneObjects.push_back(face5);//top
    sceneObjects.push_back(face6);//bottom
    
    
    }


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------


void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    
    //-- load the texture
    texture1 = TextureBMP((char*)"image.bmp");
    texture2 = TextureBMP((char*)"bg.bmp");
    
    //-- floor n stuff
    Plane *plane = new Plane (
     glm::vec3(-120., -20, 180), //Point A
     glm::vec3(120., -20, 180), //Point B
     glm::vec3(120., -20, -400), //Point C
     glm::vec3(-120., -20, -400), //Point D
     glm::vec3(0.5, 0.5, 0)); //Colour
     
     
	Plane *wall = new Plane(glm::vec3(-400., -20, -300),//Point A
							glm::vec3(400., -20, -300),//Point B
							glm::vec3(400., 250, -300),//Point C
							glm::vec3(-400., 250, -300),//Point D
							glm::vec3(0.2,0.2,0.2));//Colour
    
	//-- Create a pointer to a sphere objects
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -95.0), 14.0, glm::vec3(1, 0, 0.5));
    Sphere *sphere2 = new Sphere(glm::vec3(5.0, -15.0, -80.0), 3.5, glm::vec3(0.2, 0.2, 1));
    Sphere *sphere3 = new Sphere(glm::vec3(18.0, 0.0, -80.0), 6.0, glm::vec3(0, 1, 0));
    Sphere *sphere4 = new Sphere(glm::vec3(10.0, 4.0, -110.0), 6.0, glm::vec3(0, 1, 0));
    Cone *cone4 = new Cone(glm::vec3(-20, -20, -85), 6.0, 16.0, glm::vec3(.508,.256,.312));
    
	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1); //0
    sceneObjects.push_back(sphere2); //1
    sceneObjects.push_back(sphere3); //2
    sceneObjects.push_back(sphere4); //3
    sceneObjects.push_back(plane); //4
    sceneObjects.push_back(cone4); //5
    sceneObjects.push_back(wall); //6
    drawCube(8, -10.0, -60.0,4,8,4,glm::vec3(0.9, 0.29, 0.1));//6-11
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 450);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
