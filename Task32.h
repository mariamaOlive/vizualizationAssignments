//---------------------------------------------------------------------------
#ifndef Task32H
#define Task32H
//---------------------------------------------------------------------------
#include "Experiment.h"
#include "LinearAlgebra.h"
#include "GLGeometryViewer.h"
#include "Field2.hpp"
//---------------------------------------------------------------------------


/// This is an example experiment.
///
/// The code is meant to demonstrate how
///  to use the GeoX framework
///
class Cell1{
	public:
		float v1, v2, v3, v4;
		Vector2f p1, p2, p3, p4; 
};

class Task32 : public Experiment
{
    GEOX_CLASS(Task32)

//Constructor / Destructor
public:
    Task32();
    virtual ~Task32();

//Methods
public:
	void DrawGrid(ScalarField2 field);
    void DrawScalarField();
	float calculateIntersection(float vMin, float vMax, float xMin, float xMax);
	void DrawIntersection(Cell1 cell);
    virtual QWidget* createViewer();

//Attributes
public:
    ///File name of the scalar field
	vector<Cell1> vectorCells;
	string ScalarfieldFilename;
	float isovalue;
	bool asymptotic;
	int N;

protected:
    GLGeometryViewer* viewer;
};


#endif
