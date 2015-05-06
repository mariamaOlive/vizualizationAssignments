//---------------------------------------------------------------------------
#ifndef Task61H
#define Task61H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
//---------------------------------------------------------------------------


/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///
class Task61 : public Experiment
{
    GEOX_CLASS(Task61)

//Constructor / Destructor
public:
    Task61();
    virtual ~Task61();

//Methods
public:

    void DrawVectorField();
    void DrawTexture();
    virtual QWidget* createViewer();

	void LIC();

//Attributes
public:

    ///File name of the vector field
    string VectorfieldFilename;

	//Values for LIC
	int SampleX;
	int SampleY;
	int KernelSize;
	int Seed;
	bool RandomTexture;
	bool FastLIC;
	bool AutoContrast;

    ///Length of the arrows
    float ArrowScale;

    ///File name of the image for the texture
    string ImageFilename;

    ///Whether to draw the texture in RGB or grayscale
    bool bColoredTexture;

protected:
    GLGeometryViewer* viewer;
};


#endif
