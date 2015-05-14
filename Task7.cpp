//---------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Task7.h"
//---------------------------------------------------------------------------
#include "Properties.h"
#include "GLGeometryViewer.h"
#include "GeoXOutput.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

IMPLEMENT_GEOX_CLASS( Task7, 0)
{
    BEGIN_CLASS_INIT( Task7 );

    ADD_SEPARATOR("Scalarfield")
    ADD_STRING_PROP(ScalarfieldFilename, 0)
    ADD_NOARGS_METHOD(Task7::DrawScalarField)

    ADD_SEPARATOR("Vectorfield")
    ADD_STRING_PROP(VectorfieldFilename, 0)
    ADD_FLOAT32_PROP(ArrowScale, 0)
    ADD_NOARGS_METHOD(Task7::DrawVectorField)

    ADD_SEPARATOR("Texture")
    ADD_STRING_PROP(ImageFilename, 0)
    ADD_BOOLEAN_PROP(bColoredTexture, 0)
    ADD_NOARGS_METHOD(Task7::DrawTexture)
	ADD_NOARGS_METHOD(Task7::RunFindingZero)
}

QWidget* Task7::createViewer()
{
    viewer = new GLGeometryViewer();
    return viewer;
}

Task7::Task7()
{
    viewer = NULL;
    ScalarfieldFilename = "";
    VectorfieldFilename = "ANoise2CT4.am";
    ArrowScale = 0.1;
    ImageFilename = "";
    bColoredTexture = true;

	
}

Task7::~Task7() {}

void Task7::RunFindingZero(){
	//Load the vector field
    if (!field.load(VectorfieldFilename))
    {
        output << "Error loading field file " << VectorfieldFilename << "\n";
        return;
    }

	
	/*Vector2f p1= makeVector2f(field.boundMin()[0], field.boundMax()[1]);
	Vector2f p2= makeVector2f(field.boundMax()[0], field.boundMax()[1]);
	Vector2f p3= makeVector2f(field.boundMax()[0], field.boundMin()[1]);
	Vector2f p4= makeVector2f(field.boundMin()[0], field.boundMin()[1]);
*/
	float gap=.05;
	//Draw vector directions (constant length)
    for(float32 x=field.boundMin()[0]; x<=field.boundMax()[0]; x+=gap)
    {
        for(float32 y=field.boundMin()[1]; y<=field.boundMax()[1]; y+=gap)
        {
			Vector2f p1= makeVector2f(x, y+gap);
			Vector2f p2= makeVector2f(x+gap,y+gap);
			Vector2f p3= makeVector2f(x+gap, y);
			Vector2f p4= makeVector2f(x, y);
			
			FindingZeros(p1,p2,p3,p4,viewer);
		}
	}
	
	viewer->refresh();
}


void Task7::DrawScalarField()
{
    viewer->clear();

    //Load scalar field
    ScalarField2 field;
    if (!field.load(ScalarfieldFilename))
    {
        output << "Error loading field file " << ScalarfieldFilename << "\n";
        return;
    }

    //Get the minimum/maximum value in that field
    float32 min = std::numeric_limits<float32>::max();
    float32 max = -std::numeric_limits<float32>::max();
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i< field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i,j);
            min = val < min ? val : min;
            max = val > max ? val : max;
        }
    }

    //Draw a point for each grid vertex.
    for(size_t j=0; j<field.dims()[1]; j++)
    {
        for(size_t i=0; i<field.dims()[0]; i++)
        {
            const float32 val = field.nodeScalar(i, j);
            const float32 c = (val - min) / (max - min);

            Point2D p;
            p.position  = field.nodePosition(i, j);
            p.size = 5;
            //Use a grayscale depending on the actual value
            p.color[0] = c; p.color[1] = c; p.color[2] = c;
            viewer->addPoint(p);
        }
    }

    viewer->refresh();
}


void Task7::DrawVectorField()
{
    viewer->clear();

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


void Task7::DrawTexture()
{
    viewer->clear();

    //Load the texture using Qt
    QImage image(ImageFilename.c_str());

    //Get its (original) dimensions. Used as bounds later.
    const float fWidth = (float)image.width();
    const float fHeight = (float)image.height();

    //Resize to power-of-two and mirror.
    image = image.mirrored().scaled(NextPOT(image.width()), NextPOT(image.height()));

    //Get its new integer dimensions.
    const int iWidth = image.width();
    const int iHeight = image.height();

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
        Gray.init(makeVector2f(-fWidth, -fHeight), makeVector2f(fWidth, fHeight), makeVector2ui(iWidth, iHeight));

        //Set the values at the vertices
        for(size_t j=0; j<Gray.dims()[1]; j++)
        {
            for(size_t i=0; i<Gray.dims()[0]; i++)
            {
                Gray.setNodeScalar(i, j, (float)(qGray(image.pixel(i, j))) / 255.0 );
            }
        }

        //Set the texture in the viewer
        viewer->setTextureGray(Gray.getData());
    }

    viewer->refresh();
}

bool Task7::Sign(float num){
	if(num>0){
		return true;
	}else{
		return false;
	}
}

void Task7::FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4, GLGeometryViewer* viewer){

	//Points position
	//  p1___p2
	//  |    |
	//  p4___p3

	/*output<<p1<<" "<<p3<<"\n";*/

	//Test the size of the "square" and see if should be divided
	Vector2f v12=makeVector2f(p1[0]-p2[0], p1[1]-p2[1]);
	float v12Size= sqrt(v12.getSqrNorm());
	Vector2f v14=makeVector2f(p1[0]-p4[0], p1[1]-p4[1]);
	float v14Size= sqrt(v14.getSqrNorm());
	
	if(v12Size<.005|| v14Size<.005){
		//Time to test if the central value is ZERO	or close to it	
		float cX=p1[0]+v12Size/2;
		float cY=p1[1]-v14Size/2;

		Vector2f centerValueVector=field.sample(cX,cY);
		float centerValueNorm=sqrt(centerValueVector.getSqrNorm());
		/*output<<"final: "<<p1<<" "<<p3<<"\n";
		output<<"center norm:"<<centerValueNorm<<"\n";*/

		if(centerValueNorm<.01){
			//Add a point in the place since is zero 
			Vector2f centerPoint= makeVector2f(cX,cY);
			
			Point2D P(centerPoint[0], centerPoint[1]);
			P.size=5;
			viewer->addPoint(P);
		}	
		return;
	}

	//Collecting the sign of the points
	Vector2f p1V=field.sample(p1);
	Vector2f p2V=field.sample(p2);
	Vector2f p3V=field.sample(p3);
	Vector2f p4V=field.sample(p4);
	bool p1X=Sign(p1V[0]);
	bool p1Y=Sign(p1V[1]);
	bool p2X=Sign(p2V[0]);
	bool p2Y=Sign(p2V[1]);
	bool p3X=Sign(p3V[0]);
	bool p3Y=Sign(p3V[1]);
	bool p4X=Sign(p4V[0]);
	bool p4Y=Sign(p4V[1]);

	if(!(p1X==p2X && p1X==p3X && p1X==p4X && p2X==p3X && p2X==p4X && p3X==p4X) && 
		!(p1Y==p2Y && p1Y==p3Y && p1Y==p4Y && p2Y==p3Y && p2Y==p4Y && p3Y==p4Y)){
		//We should divide the area in four more
			
			Vector2f p1p2=makeVector2f(p1[0]+v12Size/2, p1[1]);
			Vector2f p1p4=makeVector2f(p1[0], p1[1]-v14Size/2);
			Vector2f p2p3=makeVector2f(p2[0], p2[1]-v14Size/2);
			Vector2f p3p4=makeVector2f(p4[0]+v12Size/2, p4[1]);
			Vector2f pCenter= makeVector2f(p1[0]+v12Size/2, p1[1]-v14Size/2);


			FindingZeros(p1,p1p2, pCenter,p1p4,viewer);
			FindingZeros(p1p2, p2,p2p3,pCenter,viewer);
			FindingZeros(pCenter, p2p3, p3, p3p4,viewer);
			FindingZeros(p1p4, pCenter,p3p4,p4,viewer);	
	}
}


