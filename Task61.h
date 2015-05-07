//---------------------------------------------------------------------------
#ifndef Task61H
#define Task61H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
//---------------------------------------------------------------------------

#include <limits>
#include "Field2.hpp"

/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///


//Class resposible for representing the values of the stream line points
class pStream{
	public:
	float x;
	float y;
};


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

	void LoadFiles();
	void LIC();
	vector<pStream> PositionStream(VectorField2 field, float startX, float startY, float pixelSize, float L);
	vector<pStream> RungeKuttaStreamlines(VectorField2 field, float startX, float startY,float stepSize, float length, bool backwards);

//Attributes
public:

    ///File name of the vector field
    string VectorfieldFilename;

	//Data
	ScalarField2 Gray;
	ScalarField2 Red;
	ScalarField2 Green;
	ScalarField2 Blue;
	VectorField2 field;

	//Values for LIC
	int SampleX;
	int SampleY;
	int KernelSize;
	int Seed;
	bool RandomTexture;
	bool FastLIC;
	bool AutoContrast;
	bool BWTexture;

	//Variabels RK
	float MaxLength;
	float MinSpeed;
	bool backwards;
	bool printComments;
	bool normal;
	float RKStepSize;
	int RKSteps;

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