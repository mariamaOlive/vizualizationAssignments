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
    VectorfieldFilename = "Cylinderclose2CT10.am";
    ArrowScale = 0.1;
    ImageFilename = "";
    bColoredTexture = true;

	SampleX = 64;
	SampleY = 64;
	KernelSize = 1;
	Seed = 1;

	RandomTexture = false;
	FastLIC = false;
	AutoContrast = false;
}

Task61::~Task61() {}

void Task61::DrawVectorField()
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

void Task61::DrawTexture()
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





void Task61::LIC(){
	output << "in LIC" << "\n";
}



