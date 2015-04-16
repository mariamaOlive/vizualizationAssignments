//---------------------------------------------------------------------------
#ifndef Task31H
#define Task31H
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
class Task31 : public Experiment
{
    GEOX_CLASS(Task31)

//Constructor / Destructor
public:
    Task31();
    virtual ~Task31();

//Methods
public:
	void DrawGrid(ScalarField2 field);
    void DrawScalarField();
    
    virtual QWidget* createViewer();

//Attributes
public:
    ///File name of the scalar field
    string ScalarfieldFilename;
	float isovalue;

protected:
    GLGeometryViewer* viewer;
};


#endif
