//---------------------------------------------------------------------------
#ifndef Task7H
#define Task7H
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
class Task7 : public Experiment
{
    GEOX_CLASS(Task7)

//Constructor / Destructor
public:
    Task7();
    virtual ~Task7();

//Methods
public:
    void DrawScalarField();
    void DrawVectorField();
    void DrawTexture();
	void FindingZeros(Vector2f p1,Vector2f p2,Vector2f p3,Vector2f p4);
	void RunFindingZero();
	void ClassifyPoints();
	void RungeKuttaStreamlines(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards);
	bool RKSingleStep(VectorField2 field, float startX, float startY, float RKStepSize, bool backwards);
	void DrawSeparatrices(Point2D P, Matrix2f eigenVecs);
	bool Sign(float num);
	Vector2f GetSampleField(bool scalarField, float X, float Y);
	void RunFindingZeroScalar();
    virtual QWidget* createViewer();

//Attributes
public:
    ///File name of the scalar field
    string ScalarfieldFilename;

    ///File name of the vector field
    string VectorfieldFilename;

    ///Length of the arrows
    float ArrowScale;

    ///File name of the image for the texture
    string ImageFilename;

    ///Whether to draw the texture in RGB or grayscale
    bool bColoredTexture;

	//Vector field to be loaded
	VectorField2 field;
	//Scalar field to be loaded
	ScalarField2 scalarField;
	//Boolean choosen if scalar field selected
	bool SFCheck;

	//Vector of critical points
	Vector2f lastCriticalPoint;
	vector<Point2D> critPts;

	//RK variables
	bool printComments;
	bool normal;
	float MinSpeed;
	bool fix;

protected:
    GLGeometryViewer* viewer;
};


#endif
