//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task61.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"


#include <iostream>
using namespace std;

IMPLEMENT_GEOX_CLASS( Task61, 0)
{
    BEGIN_CLASS_INIT( Task61 );

    ADD_SEPARATOR("Vectorfield")
    ADD_FLOAT32_PROP(ArrowScale, 0)
    ADD_NOARGS_METHOD(Task61::DrawVectorField)

    ADD_SEPARATOR("Texture")
    ADD_STRING_PROP(ImageFilename, 0)
    ADD_BOOLEAN_PROP(bColoredTexture, 0)
    ADD_NOARGS_METHOD(Task61::DrawTexture)

	ADD_SEPARATOR("LIC")
	ADD_STRING_PROP(VectorfieldFilename, 0)
	ADD_INT32_PROP(SampleX, 0)
	ADD_INT32_PROP(SampleY, 0)
	ADD_INT32_PROP(KernelSize, 0)
	ADD_INT32_PROP(Seed, 0)
	ADD_BOOLEAN_PROP(RandomTexture, 0)
	ADD_BOOLEAN_PROP(FastLIC, 0)
	ADD_BOOLEAN_PROP(AutoContrast, 0)
	ADD_NOARGS_METHOD(Task61::LIC)
}

QWidget* Task61::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task61::Task61()
{
    viewer = NULL;
    VectorfieldFilename = "ANoise2CT4.am";
    ArrowScale = 0.1;
    ImageFilename = "";
    bColoredTexture = false;

	SampleX = 64;
	SampleY = 64;
	KernelSize = 1;
	Seed = 1;

	RandomTexture = true;
	FastLIC = false;
	AutoContrast = false;
	
	//Load the vector field
    VectorField2 field;
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	PositionStream(field, 0, 0, .05,10);
}

Task61::~Task61() {}


void Task61::DrawVectorField()
{
    //viewer->clear();

    //Load the vector field
    VectorField2 field;
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

    //Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=field.boundMax()[0]; x+=0.2)
    {
        for(float32 y=field.boundMin()[1]; y<=field.boundMax()[1]; y+=0.2)
        {
            Vector2f vec = field.sample(x,y);
            vec.normalize();

            viewer->addLine(x, y, x + ArrowScale*vec[0], y + ArrowScale*vec[1]);
        }
    }

    viewer->refresh();
}

namespace
{
    ///Returns the next power-of-two
    int32 NextPOT(int32 n)
    {
        n--;
        n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
        n |= n >> 2;   // and then or the results.
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n++;           // The result is a number of 1 bits equal to the number
                       // of bits in the original number, plus 1. That's the
                       // next highest power of 2.
        return n;
    }
}

void Task61::DrawTexture()
{
    viewer->clear();

	QImage image(ImageFilename.c_str());

    //Float dimensions for RandomTexture
	float fWidth = 8;
	float fHeight = 8;

	//Integer dimensions for RandomTexture
	int iWidth = 8;
	int iHeight = 8;

	if(!RandomTexture) {
		//Load the texture using Qt
		

		//Get its (original) dimensions. Used as bounds later.
		fWidth = (float)image.width();
		fHeight = (float)image.height();

		//Resize to power-of-two and mirror.
		image = image.mirrored().scaled(NextPOT(image.width()), NextPOT(image.height()));

		//Get its new integer dimensions.
		iWidth = image.width();
		iHeight = image.height();
	}
	else {
		//Initialize random seed
		srand (Seed);

	}


    if (bColoredTexture)
    {
        //Create three color channels for the texture
        //Each of them is represented using a scalar field
        ScalarField2 Red;
        Red.init(makeVector2f(-fWidth, -fHeight), makeVector2f(fWidth, fHeight), makeVector2ui(iWidth, iHeight));
        ScalarField2 Green;
        Green.init(makeVector2f(-fWidth, -fHeight), makeVector2f(fWidth, fHeight), makeVector2ui(iWidth, iHeight));
        ScalarField2 Blue;
        Blue.init(makeVector2f(-fWidth, -fHeight), makeVector2f(fWidth, fHeight), makeVector2ui(iWidth, iHeight));

        //Fill the scalar fields
        for(size_t j=0; j<Red.dims()[1]; j++)
        {
            for(size_t i=0; i<Red.dims()[0]; i++)
            {
                Red.setNodeScalar(i, j, (float)(qRed(image.pixel(i, j))) / 255.0 );
                Green.setNodeScalar(i, j, (float)(qGreen(image.pixel(i, j))) / 255.0 );
                Blue.setNodeScalar(i, j, (float)(qBlue(image.pixel(i, j))) / 255.0 );
            }
        }

        //Set the texture in the viewer
        viewer->setTextureRGB(Red.getData(), Green.getData(), Blue.getData());
    }
    else
    {
        //Create one gray color channel represented as a scalar field
        ScalarField2 Gray;
		float randGray;
		int randBW;

        Gray.init(makeVector2f(-fWidth, -fHeight), makeVector2f(fWidth, fHeight), makeVector2ui(iWidth, iHeight));

        //Set the values at the vertices
        for(size_t j=0; j<Gray.dims()[1]; j++)
        {
            for(size_t i=0; i<Gray.dims()[0]; i++)
            {
                //Gray.setNodeScalar(i, j, (float)(qGray(image.pixel(i, j))) / 255.0 );
				
				// Generate random intensities
				//randGray = (rand() % 256) / 255.0;
				randBW = rand() % 2;
				output << randBW << "\n";
				Gray.setNodeScalar(i, j, (float)(randBW));
            }
        }

        //Set the texture in the viewer
        viewer->setTextureGray(Gray.getData());
    }

    viewer->refresh();
}





void Task61::LIC(){
	output << "in LIC" << "\n";

	/* Algorithm
	
	Loop through all points in vector field (pixels)
	{
		Integrate streamline from each pixel
			- Get a vector of positions for each streamline (Mariama)
			- Towards and backwards (according to defined kernel length)
			- Fixed step size: min(height, width) of pixel (?)
			- Store each step point in a vector stepPtsVector
			Loop through each point in stepPtsVector
			{
				Compute integral (LIC) over the stepPtsVector
			}

		Set pixel intensity with result from LIC
	}
	
	
	*/

}

//Calculating position of the stream line
vector<pStream> Task61::PositionStream(VectorField2 field, float startX, float startY, float pixelSize, float L){
	
	//Final vector to be returned
	vector<pStream> finalVector;

	//Vectors that will contain the positions of the stream line (-L to L)
	vector<pStream> forward=RungeKuttaStreamlines(field, startX, startY, pixelSize, L, false);
	vector<pStream> backward=RungeKuttaStreamlines(field, startX, startY, pixelSize, L, true);
	
	//Reserving memory for the two sets of position
	finalVector.reserve(forward.size()+backward.size());
	finalVector.insert(finalVector.end(), forward.begin(), forward.end());
	finalVector.insert(finalVector.end(), backward.begin(), backward.end());

	//Adding a final point (the point byitself)
	pStream p;
	p.x=startX;
	p.y=startY;
	finalVector.push_back(p);

	return finalVector;
}


//Runge-Kutta
 vector<pStream> Task61::RungeKuttaStreamlines(VectorField2 field, float startX, float startY, float RKStepSize, float L, bool backwards){

	 //The vector to be returned with the positions
	 vector<pStream> positions;

	//Defining the start point
	Vector2f startPoint = makeVector2f(startX,startY);
	
	Vector2f x = makeVector2f(startPoint[0], startPoint[1]);

	float RKStepSize2 = backwards ? -RKStepSize : RKStepSize; // if "backwards" is checked, inverts sign of integration step
	float a, b, length, speed;
	float arcLength = 0;
	bool outOfBounds = false;
	bool tooSlow = false;
	bool zeroSpeed = false;
	float MaxLength=L;
	bool fix=false;

	//Checks if initial point is out of boundaries
	if ((x[0] < field.boundMin()[0])||(x[0] > field.boundMax()[0])||(x[1] < field.boundMin()[1])||(x[1] > field.boundMax()[1])) {
		outOfBounds = true;
	//	if(printComments)
	//	  output << "Out of bounds! \n";
	}

	//Verifing if starts at zeroSpeed
	Vector2f sampleVector = field.sample(x[0], x[1]);
	speed = sqrt(pow(sampleVector[0],2)+pow(sampleVector[1],2));

	if(speed==0){
		zeroSpeed=true;
	}

	//bugFix
	float past=0;
	for(int i = 0; ((arcLength < MaxLength) && (!outOfBounds) && (!tooSlow) && (!zeroSpeed) && (!fix)); i++){

	//The 4 vectors of th RK method
		Vector2f v1 = field.sample(x[0],x[1]);
		float test=v1.getSqrNorm();
		v1.normalize();
		

		Vector2f v2p = makeVector2f((x[0]+RKStepSize2*v1[0]/2),(x[1]+RKStepSize2*v1[1]/2));
		Vector2f v2 = field.sample(v2p[0],v2p[1]);
		test=v2.getSqrNorm();
		v2.normalize();
		

		Vector2f v3p = makeVector2f((x[0]+RKStepSize2*v2[0]/2),(x[1]+RKStepSize2*v2[1]/2));
		Vector2f v3 = field.sample(v3p[0],v3p[1]);
		test=v3.getSqrNorm();
		v3.normalize();


		Vector2f v4p = makeVector2f((x[0]+RKStepSize2*v3[0]),(x[1]+RKStepSize2*v3[1]));
		Vector2f v4 = field.sample(v4p[0],v4p[1]);
		test=v4.getSqrNorm();
		v4.normalize();

	//Combine the 4 vectors to get the end position
		Vector2f x1 = makeVector2f(x[0]+RKStepSize2*(v1[0]/6 + v2[0]/3 + v3[0]/3 + v4[0]/6), x[1]+RKStepSize2*(v1[1]/6 + v2[1]/3 + v3[1]/3 + v4[1]/6));

	//Calculates arc length
		a = x1[0]-x[0];
		b = x1[1]-x[1];
		length = sqrt(pow(a,2)+pow(b,2));
		arcLength += length;
	
		//Trick to fix crazy bug
		if(past>length && i!=0){
			if((past-length)>.001){
				fix=true;
			}
		}else{
			if((past-length)<-.001 && i!=0){
				fix=true;
			}
		}
		past=length;

	//Calculates speed (actual vector size) by sampling
		Vector2f sampleVector = field.sample(x1[0], x1[1]);
		a = sampleVector[0]-x1[0];
		b = sampleVector[1]-x1[1];
		speed = sqrt(pow(a,2)+pow(b,2));
		//output << "speed: " << speed << "\n";
		
		//Checks boundary limits
		if ((x1[0] < field.boundMin()[0])||(x1[0] > field.boundMax()[0])||(x1[1] < field.boundMin()[1])||(x1[1] > field.boundMax()[1])) {
			outOfBounds = true;
			//if(printComments)
			  output << "Out of bounds! \n";
		}else if(speed==0){
			zeroSpeed=true;
			output<<"zero speed"<< "\n";
		}
		//else if (speed < MinSpeed) {
		//	tooSlow = true;
		////	//if(printComments)
		////	  //output << "Vector speed too slow... \n";
		//}
		else {
			//Adding the point in the vector of positions
			pStream point;
			point.x=x1[0];
			point.y=x1[1];
			
			positions.push_back(point);
			x = x1;
		}
	}
	
	return positions;
}





